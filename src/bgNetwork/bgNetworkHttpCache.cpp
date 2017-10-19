#include "stdafx.h"

#include "bgNetworkHttpCache.h"
#include "bgCompletionCallback.h"
#include "gtest/gtest.h"

//struct bgTransaction {
//	const char* url;
//	const char* method;
//	// If |request_time| is unspecified, the current time will be used.
//	base::Time request_time;
//	const char* request_headers;
//	int load_flags;
//	const char* status;
//	const char* response_headers;
//	// If |response_time| is unspecified, the current time will be used.
//	base::Time response_time;
//	const char* data;
//	int test_mode;
//	bgTransactionHandler handler;
//	net::CertStatus cert_status;
//	// Value returned by MockNetworkTransaction::Start (potentially
//	// asynchronously if |!(test_mode & TEST_MODE_SYNC_NET_START)|.)
//	net::Error return_code;
//};

namespace {

	// We can override the test mode for a given operation by setting this global
	// variable.
	int g_test_mode = 0;

	int GetTestModeForEntry(const std::string& key) {
		// 'key' is prefixed with an identifier if it corresponds to a cached POST.
		// Skip past that to locate the actual URL.
		//
		// TODO(darin): It breaks the abstraction a bit that we assume 'key' is an
		// URL corresponding to a registered MockTransaction.  It would be good to
		// have another way to access the test_mode.
		GURL url;
		if (isdigit(key[0])) {
			size_t slash = key.find('/');
			DCHECK(slash != std::string::npos);
			url = GURL(key.substr(slash + 1));
		} else {
			url = GURL(key);
		}
		const bgTransaction* t = FindbgTransaction(url);
		DCHECK(t);
		return t->test_mode;
	}

	void CallbackForwader(const net::CompletionCallback& callback, int result) {
		callback.Run(result);
	}

}  // namespace

//-----------------------------------------------------------------------------

struct bgDiskEntry::CallbackInfo {
	scoped_refptr<bgDiskEntry> entry;
	net::CompletionCallback callback;
	int result;
};

bgDiskEntry::bgDiskEntry()
	: test_mode_(0), doomed_(false), sparse_(false)
	, fail_requests_(false), busy_(false), delayed_(false) {
}

bgDiskEntry::bgDiskEntry(const std::string& key)
	: test_mode_(0), key_(key), doomed_(false), sparse_(false)
	, fail_requests_(false), busy_(false), delayed_(false) {
		//test_mode_ = GetTestModeForEntry(key);
}

void bgDiskEntry::Doom() {
	doomed_ = true;
}

void bgDiskEntry::Close() {
	Release();
}

std::string bgDiskEntry::GetKey() const {
	return key_;
}

base::Time bgDiskEntry::GetLastUsed() const {
	return base::Time::FromInternalValue(0);
}

base::Time bgDiskEntry::GetLastModified() const {
	return base::Time::FromInternalValue(0);
}

int32 bgDiskEntry::GetDataSize(int index) const {
	DCHECK(index >= 0 && index < kNumCacheEntryDataIndices);
	return static_cast<int32>(data_[index].size());
}

int bgDiskEntry::ReadData(
	int index, int offset, net::IOBuffer* buf, int buf_len,
	const net::CompletionCallback& callback) {
		DCHECK(index >= 0 && index < kNumCacheEntryDataIndices);
		DCHECK(!callback.is_null());

		if (fail_requests_)
			return net::ERR_CACHE_READ_FAILURE;

		if (offset < 0 || offset > static_cast<int>(data_[index].size()))
			return net::ERR_FAILED;
		if (static_cast<size_t>(offset) == data_[index].size())
			return 0;

		int num = (std::min)(buf_len, static_cast<int>(data_[index].size()) - offset);
		memcpy(buf->data(), &data_[index][offset], num);

		if (bgNetworkHttpCache::GetTestMode(test_mode_) & TEST_MODE_SYNC_CACHE_READ)
			return num;

		CallbackLater(callback, num);
		return net::ERR_IO_PENDING;
}

int bgDiskEntry::WriteData(
	int index, int offset, net::IOBuffer* buf, int buf_len,
	const net::CompletionCallback& callback, bool truncate) {
		DCHECK(index >= 0 && index < kNumCacheEntryDataIndices);
		DCHECK(!callback.is_null());
		DCHECK(truncate);

		if (fail_requests_) {
			CallbackLater(callback, net::ERR_CACHE_READ_FAILURE);
			return net::ERR_IO_PENDING;
		}

		if (offset < 0 || offset > static_cast<int>(data_[index].size()))
			return net::ERR_FAILED;

		data_[index].resize(offset + buf_len);
		if (buf_len)
			memcpy(&data_[index][offset], buf->data(), buf_len);

		if (bgNetworkHttpCache::GetTestMode(test_mode_) & TEST_MODE_SYNC_CACHE_WRITE)
			return buf_len;

		CallbackLater(callback, buf_len);
		return net::ERR_IO_PENDING;
}

int bgDiskEntry::ReadSparseData(int64 offset, net::IOBuffer* buf, int buf_len,
	const net::CompletionCallback& callback) {
		DCHECK(!callback.is_null());
		if (!sparse_ || busy_)
			return net::ERR_CACHE_OPERATION_NOT_SUPPORTED;
		if (offset < 0)
			return net::ERR_FAILED;

		if (fail_requests_)
			return net::ERR_CACHE_READ_FAILURE;

		DCHECK(offset < kint32max);
		int real_offset = static_cast<int>(offset);
		if (!buf_len)
			return 0;

		int num = (std::min)(static_cast<int>(data_[1].size()) - real_offset,
			buf_len);
		memcpy(buf->data(), &data_[1][real_offset], num);

		if (bgNetworkHttpCache::GetTestMode(test_mode_) & TEST_MODE_SYNC_CACHE_READ)
			return num;

		CallbackLater(callback, num);
		busy_ = true;
		delayed_ = false;
		return net::ERR_IO_PENDING;
}

int bgDiskEntry::WriteSparseData(int64 offset, net::IOBuffer* buf,
	int buf_len,
	const net::CompletionCallback& callback) {
		DCHECK(!callback.is_null());
		if (busy_)
			return net::ERR_CACHE_OPERATION_NOT_SUPPORTED;
		if (!sparse_) {
			if (data_[1].size())
				return net::ERR_CACHE_OPERATION_NOT_SUPPORTED;
			sparse_ = true;
		}
		if (offset < 0)
			return net::ERR_FAILED;
		if (!buf_len)
			return 0;

		if (fail_requests_)
			return net::ERR_CACHE_READ_FAILURE;

		DCHECK(offset < kint32max);
		int real_offset = static_cast<int>(offset);

		if (static_cast<int>(data_[1].size()) < real_offset + buf_len)
			data_[1].resize(real_offset + buf_len);

		memcpy(&data_[1][real_offset], buf->data(), buf_len);
		if (bgNetworkHttpCache::GetTestMode(test_mode_) & TEST_MODE_SYNC_CACHE_WRITE)
			return buf_len;

		CallbackLater(callback, buf_len);
		return net::ERR_IO_PENDING;
}

int bgDiskEntry::GetAvailableRange(int64 offset, int len, int64* start,
	const net::CompletionCallback& callback) {
		DCHECK(!callback.is_null());
		if (!sparse_ || busy_)
			return net::ERR_CACHE_OPERATION_NOT_SUPPORTED;
		if (offset < 0)
			return net::ERR_FAILED;

		if (fail_requests_)
			return net::ERR_CACHE_READ_FAILURE;

		*start = offset;
		DCHECK(offset < kint32max);
		int real_offset = static_cast<int>(offset);
		if (static_cast<int>(data_[1].size()) < real_offset)
			return 0;

		int num = (std::min)(static_cast<int>(data_[1].size()) - real_offset, len);
		int count = 0;
		for (; num > 0; num--, real_offset++) {
			if (!count) {
				if (data_[1][real_offset]) {
					count++;
					*start = real_offset;
				}
			} else {
				if (!data_[1][real_offset])
					break;
				count++;
			}
		}
		if (bgNetworkHttpCache::GetTestMode(test_mode_) & TEST_MODE_SYNC_CACHE_WRITE)
			return count;

		CallbackLater(callback, count);
		return net::ERR_IO_PENDING;
}

bool bgDiskEntry::CouldBeSparse() const {
	return sparse_;
}

void bgDiskEntry::CancelSparseIO() {
	cancel_ = true;
}

int bgDiskEntry::ReadyForSparseIO(const net::CompletionCallback& callback) {
	if (!cancel_)
		return net::OK;

	cancel_ = false;
	DCHECK(!callback.is_null());
	if (bgNetworkHttpCache::GetTestMode(test_mode_) & TEST_MODE_SYNC_CACHE_READ)
		return net::OK;

	// The pending operation is already in the message loop (and hopefully
	// already in the second pass).  Just notify the caller that it finished.
	CallbackLater(callback, 0);
	return net::ERR_IO_PENDING;
}

// If |value| is true, don't deliver any completion callbacks until called
// again with |value| set to false.  Caution: remember to enable callbacks
// again or all subsequent tests will fail.
// Static.
void bgDiskEntry::IgnoreCallbacks(bool value) {
	if (ignore_callbacks_ == value)
		return;
	ignore_callbacks_ = value;
	if (!value)
		StoreAndDeliverCallbacks(false, NULL, net::CompletionCallback(), 0);
}

bgDiskEntry::~bgDiskEntry() {
}

// Unlike the callbacks for MockHttpTransaction, we want this one to run even
// if the consumer called Close on the MockDiskEntry.  We achieve that by
// leveraging the fact that this class is reference counted.
void bgDiskEntry::CallbackLater(const net::CompletionCallback& callback,
	int result) {
		if (ignore_callbacks_)
			return StoreAndDeliverCallbacks(true, this, callback, result);
		MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
			&bgDiskEntry::RunCallback, this, callback, result));
}

void bgDiskEntry::RunCallback(
	const net::CompletionCallback& callback, int result) {
		if (busy_) {
			// This is kind of hacky, but controlling the behavior of just this entry
			// from a test is sort of complicated.  What we really want to do is
			// delay the delivery of a sparse IO operation a little more so that the
			// request start operation (async) will finish without seeing the end of
			// this operation (already posted to the message loop)... and without
			// just delaying for n mS (which may cause trouble with slow bots).  So
			// we re-post this operation (all async sparse IO operations will take two
			// trips through the message loop instead of one).
			if (!delayed_) {
				delayed_ = true;
				return CallbackLater(callback, result);
			}
		}
		busy_ = false;
		callback.Run(result);
}

// When |store| is true, stores the callback to be delivered later; otherwise
// delivers any callback previously stored.
// Static.
void bgDiskEntry::StoreAndDeliverCallbacks(
	bool store, bgDiskEntry* entry, const net::CompletionCallback& callback,
	int result) {
		static std::vector<CallbackInfo> callback_list;
		if (store) {
			CallbackInfo c = {entry, callback, result};
			callback_list.push_back(c);
		} else {
			for (size_t i = 0; i < callback_list.size(); i++) {
				CallbackInfo& c = callback_list[i];
				c.entry->CallbackLater(c.callback, c.result);
			}
			callback_list.clear();
		}
}

// Statics.
bool bgDiskEntry::cancel_ = false;
bool bgDiskEntry::ignore_callbacks_ = false;

//-----------------------------------------------------------------------------

bgDiskCache::bgDiskCache()
	: open_count_(0), create_count_(0), fail_requests_(false),
	soft_failures_(false), double_create_check_(true) {
}

bgDiskCache::~bgDiskCache() {
	ReleaseAll();
}

net::CacheType bgDiskCache::GetCacheType() const {
	return net::DISK_CACHE;
}

int32 bgDiskCache::GetEntryCount() const {
	return static_cast<int32>(entries_.size());
}

int bgDiskCache::OpenEntry(const std::string& key, disk_cache::Entry** entry,
	const net::CompletionCallback& callback) {
		DCHECK(!callback.is_null());
		if (fail_requests_)
			return net::ERR_CACHE_OPEN_FAILURE;

		EntryMap::iterator it = entries_.find(key);
		if (it == entries_.end())
			return net::ERR_CACHE_OPEN_FAILURE;

		if (it->second->is_doomed()) {
			it->second->Release();
			entries_.erase(it);
			return net::ERR_CACHE_OPEN_FAILURE;
		}

		open_count_++;

		it->second->AddRef();
		*entry = it->second;

		if (soft_failures_)
			it->second->set_fail_requests();

		if (GetTestModeForEntry(key) & TEST_MODE_SYNC_CACHE_START)
			return net::OK;

		CallbackLater(callback, net::OK);
		return net::ERR_IO_PENDING;
}

int bgDiskCache::CreateEntry(const std::string& key,
	disk_cache::Entry** entry,
	const net::CompletionCallback& callback) {
		DCHECK(!callback.is_null());
		if (fail_requests_)
			return net::ERR_CACHE_CREATE_FAILURE;

		EntryMap::iterator it = entries_.find(key);
		if (it != entries_.end()) {
			if (!it->second->is_doomed()) {
				if (double_create_check_)
					NOTREACHED();
				else
					return net::ERR_CACHE_CREATE_FAILURE;
			}
			it->second->Release();
			entries_.erase(it);
		}

		create_count_++;

		bgDiskEntry* new_entry = new bgDiskEntry(key);

		new_entry->AddRef();
		entries_[key] = new_entry;

		new_entry->AddRef();
		*entry = new_entry;

		if (soft_failures_)
			new_entry->set_fail_requests();

		if (GetTestModeForEntry(key) & TEST_MODE_SYNC_CACHE_START)
			return net::OK;

		CallbackLater(callback, net::OK);
		return net::ERR_IO_PENDING;
}

int bgDiskCache::DoomEntry(const std::string& key,
	const net::CompletionCallback& callback) {
		DCHECK(!callback.is_null());
		EntryMap::iterator it = entries_.find(key);
		if (it != entries_.end()) {
			it->second->Release();
			entries_.erase(it);
		}

		if (GetTestModeForEntry(key) & TEST_MODE_SYNC_CACHE_START)
			return net::OK;

		CallbackLater(callback, net::OK);
		return net::ERR_IO_PENDING;
}

int bgDiskCache::DoomAllEntries(const net::CompletionCallback& callback) {
	return net::ERR_NOT_IMPLEMENTED;
}

int bgDiskCache::DoomEntriesBetween(const base::Time initial_time,
	const base::Time end_time,
	const net::CompletionCallback& callback) {
		return net::ERR_NOT_IMPLEMENTED;
}

int bgDiskCache::DoomEntriesSince(const base::Time initial_time,
	const net::CompletionCallback& callback) {
		return net::ERR_NOT_IMPLEMENTED;
}

int bgDiskCache::OpenNextEntry(void** iter, disk_cache::Entry** next_entry,
	const net::CompletionCallback& callback) {
		return net::ERR_NOT_IMPLEMENTED;
}

void bgDiskCache::EndEnumeration(void** iter) {
}

void bgDiskCache::GetStats(
	std::vector<std::pair<std::string, std::string> >* stats) {
}

void bgDiskCache::OnExternalCacheHit(const std::string& key) {
}

void bgDiskCache::ReleaseAll() {
	EntryMap::iterator it = entries_.begin();
	for (; it != entries_.end(); ++it)
		it->second->Release();
	entries_.clear();
}

void bgDiskCache::CallbackLater(const net::CompletionCallback& callback,
	int result) {
		MessageLoop::current()->PostTask(
			FROM_HERE, base::Bind(&CallbackForwader, callback, result));
}

//-----------------------------------------------------------------------------

int bgBackendFactory::CreateBackend(net::NetLog* net_log,
	disk_cache::Backend** backend,
	const net::CompletionCallback& callback) {
		*backend = new bgDiskCache();
		return net::OK;
}

//-----------------------------------------------------------------------------

bgNetworkHttpCache::bgNetworkHttpCache()
	: http_cache_(new bgNetworkHttpLayer(), NULL, new bgBackendFactory()) {
}

bgNetworkHttpCache::bgNetworkHttpCache(net::HttpCache::BackendFactory* disk_cache_factory)
	: http_cache_(new bgNetworkHttpLayer(), NULL, disk_cache_factory) {
}

bgDiskCache* bgNetworkHttpCache::disk_cache() {
	bgCompletionCallback cb;
	disk_cache::Backend* backend;
	int rv = http_cache_.GetBackend(&backend, cb.callback());
	rv = cb.GetResult(rv);
	return (rv == net::OK) ? static_cast<bgDiskCache*>(backend) : NULL;
}

bool bgNetworkHttpCache::ReadResponseInfo(disk_cache::Entry* disk_entry,
	net::HttpResponseInfo* response_info,
	bool* response_truncated) {
		int size = disk_entry->GetDataSize(0);

		bgCompletionCallback cb;
		scoped_refptr<net::IOBuffer> buffer(new net::IOBuffer(size));
		int rv = disk_entry->ReadData(0, 0, buffer, size, cb.callback());
		rv = cb.GetResult(rv);
		EXPECT_EQ(size, rv);

		return net::HttpCache::ParseResponseInfo(buffer->data(), size,
			response_info,
			response_truncated);
}

bool bgNetworkHttpCache::WriteResponseInfo(
	disk_cache::Entry* disk_entry, const net::HttpResponseInfo* response_info,
	bool skip_transient_headers, bool response_truncated) {
		Pickle pickle;
		response_info->Persist(
			&pickle, skip_transient_headers, response_truncated);

		bgCompletionCallback cb;
		scoped_refptr<net::WrappedIOBuffer> data(new net::WrappedIOBuffer(
			reinterpret_cast<const char*>(pickle.data())));
		int len = static_cast<int>(pickle.size());

		int rv =  disk_entry->WriteData(0, 0, data, len, cb.callback(), true);
		rv = cb.GetResult(rv);
		return (rv == len);
}

bool bgNetworkHttpCache::OpenBackendEntry(const std::string& key,
	disk_cache::Entry** entry) {
		bgCompletionCallback cb;
		int rv = disk_cache()->OpenEntry(key, entry, cb.callback());
		return (cb.GetResult(rv) == net::OK);
}

bool bgNetworkHttpCache::CreateBackendEntry(const std::string& key,
	disk_cache::Entry** entry,
	net::NetLog* net_log) {
		bgCompletionCallback cb;
		int rv = disk_cache()->CreateEntry(key, entry, cb.callback());
		return (cb.GetResult(rv) == net::OK);
}

// Static.
int bgNetworkHttpCache::GetTestMode(int test_mode) {
	if (!g_test_mode)
		return test_mode;

	return g_test_mode;
}

// Static.
void bgNetworkHttpCache::SetTestMode(int test_mode) {
	g_test_mode = test_mode;
}