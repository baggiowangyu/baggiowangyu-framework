#include "stdafx.h"

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/memory/weak_ptr.h"
#include "base/string16.h"
#include "base/stringprintf.h"
#include "net/base/io_buffer.h"
#include "net/base/load_flags.h"
#include "net/base/net_errors.h"
#include "net/base/request_priority.h"
#include "net/base/test_completion_callback.h"
#include "net/disk_cache/disk_cache.h"
#include "net/http/http_cache.h"
#include "net/http/http_request_info.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_response_info.h"

#include "bgNetworkHttp.h"
#include "bgNetworkHttpCache.h"

typedef base::hash_map<std::string, const bgTransaction*> bgTransactionMap;
static bgTransactionMap bg_transactions;

const bgTransaction* FindMockTransaction(const GURL& url) {
	// look for overrides:
	bgTransactionMap::const_iterator it = bg_transactions.find(url.spec());
	if (it != bg_transactions.end())
		return it->second;

	return NULL;
}

void AddMockTransaction(const bgTransaction* trans) {
	bg_transactions[GURL(trans->url).spec()] = trans;
}

void RemoveMockTransaction(const bgTransaction* trans) {
	bg_transactions.erase(GURL(trans->url).spec());
}

bgHttpRequest::bgHttpRequest(const bgTransaction& t) {
	url = GURL(t.url);
	method = t.method;
	extra_headers.AddHeadersFromString(t.request_headers);
	load_flags = t.load_flags;
}

// -------------------------------------------------------------------------------------------

bgNetworkHttpTransactionClient::bgNetworkHttpTransactionClient(net::RequestPriority priority, net::HttpTransactionFactory* factory)
	: state_(IDLE)
	, trans_(nullptr)
	, error_(net::OK)
{
	factory->CreateTransaction(priority, &trans_, nullptr);
	++quit_counter_;
}

bgNetworkHttpTransactionClient::~bgNetworkHttpTransactionClient()
{

}

void bgNetworkHttpTransactionClient::Start(const net::HttpRequestInfo* request, const net::BoundNetLog& net_log)
{
	state_ = STARTING;
	int result = trans_->Start(request, base::Bind(&bgNetworkHttpTransactionClient::OnIOComplete, base::Unretained(this)), net_log);

	if (result != net::ERR_IO_PENDING)
		DidStart(result);
}

void bgNetworkHttpTransactionClient::DidStart(int result)
{
	if (result != net::OK)
	{
		DidFinish(result);
	}
	else
	{
		Read();
	}
}

void bgNetworkHttpTransactionClient::DidRead(int result)
{
	if (result <= 0)
	{
		DidFinish(result);
	}
	else
	{
		content_.append(read_buf_->data(), result);
		Read();
	}
}

void bgNetworkHttpTransactionClient::DidFinish(int result)
{
	state_ = DONE;
	error_ = result;
	if (--quit_counter_ == 0)
		MessageLoop::current()->Quit();
}

void bgNetworkHttpTransactionClient::Read()
{
	state_ = READING;
	read_buf_ = new net::IOBuffer(1024);
	int result = trans_->Read(read_buf_, 1024, base::Bind(&bgNetworkHttpTransactionClient::OnIOComplete, base::Unretained(this)));

	if (result != net::ERR_IO_PENDING)
		DidRead(result);
}

void bgNetworkHttpTransactionClient::OnIOComplete(int result)
{
	switch (state_)
	{
	case STARTING:
		DidStart(result);
		break;
	case READING:
		DidRead(result);
		break;
	default:
		NOTREACHED();
	}
}

// -----------------------------------------------------------------------------------------

bgNetworkHttpTransaction::bgNetworkHttpTransaction(
	net::RequestPriority priority,
	bgNetworkHttpLayer* factory)
	: weak_factory_(this),
	data_cursor_(0),
	priority_(priority),
	transaction_factory_(factory->AsWeakPtr()) {
}

bgNetworkHttpTransaction::~bgNetworkHttpTransaction() {}

int bgNetworkHttpTransaction::Start(const net::HttpRequestInfo* request,
	const net::CompletionCallback& callback,
	const net::BoundNetLog& net_log) {
		const bgTransaction* t = FindbgTransaction(request->url);
		if (!t)
			return net::ERR_FAILED;

		test_mode_ = t->test_mode;

		// Return immediately if we're returning in error.
		if (net::OK != t->return_code) {
			if (test_mode_ & TEST_MODE_SYNC_NET_START)
				return t->return_code;
			CallbackLater(callback, t->return_code);
			return net::ERR_IO_PENDING;
		}

		std::string resp_status = t->status;
		std::string resp_headers = t->response_headers;
		std::string resp_data = t->data;
		if (t->handler)
			(t->handler)(request, &resp_status, &resp_headers, &resp_data);

		std::string header_data = base::StringPrintf(
			"%s\n%s\n", resp_status.c_str(), resp_headers.c_str());
		std::replace(header_data.begin(), header_data.end(), '\n', '\0');

		response_.request_time = base::Time::Now();
		if (!t->request_time.is_null())
			response_.request_time = t->request_time;

		response_.was_cached = false;
		response_.network_accessed = true;

		response_.response_time = base::Time::Now();
		if (!t->response_time.is_null())
			response_.response_time = t->response_time;

		response_.headers = new net::HttpResponseHeaders(header_data);
		response_.vary_data.Init(*request, *response_.headers);
		response_.ssl_info.cert_status = t->cert_status;
		data_ = resp_data;

		if (test_mode_ & TEST_MODE_SYNC_NET_START)
			return net::OK;

		CallbackLater(callback, net::OK);
		return net::ERR_IO_PENDING;
}

int bgNetworkHttpTransaction::RestartIgnoringLastError(
	const net::CompletionCallback& callback) {
		return net::ERR_FAILED;
}

int bgNetworkHttpTransaction::RestartWithCertificate(
	net::X509Certificate* client_cert,
	const net::CompletionCallback& callback) {
		return net::ERR_FAILED;
}

int bgNetworkHttpTransaction::RestartWithAuth(
	const net::AuthCredentials& credentials,
	const net::CompletionCallback& callback) {
		return net::ERR_FAILED;
}

bool bgNetworkHttpTransaction::IsReadyToRestartForAuth() {
	return false;
}

int bgNetworkHttpTransaction::Read(net::IOBuffer* buf, int buf_len,
	const net::CompletionCallback& callback) {
		int data_len = static_cast<int>(data_.size());
		int num = (std::min)(buf_len, data_len - data_cursor_);
		if (num) {
			memcpy(buf->data(), data_.data() + data_cursor_, num);
			data_cursor_ += num;
		}
		if (test_mode_ & TEST_MODE_SYNC_NET_READ)
			return num;

		CallbackLater(callback, num);
		return net::ERR_IO_PENDING;
}

void bgNetworkHttpTransaction::StopCaching() {}

void bgNetworkHttpTransaction::DoneReading() {
	if (transaction_factory_)
		transaction_factory_->TransactionDoneReading();
}

const net::HttpResponseInfo* bgNetworkHttpTransaction::GetResponseInfo() const {
	return &response_;
}

net::LoadState bgNetworkHttpTransaction::GetLoadState() const {
	if (data_cursor_)
		return net::LOAD_STATE_READING_RESPONSE;
	return net::LOAD_STATE_IDLE;
}

net::UploadProgress bgNetworkHttpTransaction::GetUploadProgress() const {
	return net::UploadProgress();
}

bool bgNetworkHttpTransaction::GetLoadTimingInfo(
	net::LoadTimingInfo* load_timing_info) const {
		return false;
}

void bgNetworkHttpTransaction::SetPriority(net::RequestPriority priority) {
	priority_ = priority;
}

void bgNetworkHttpTransaction::CallbackLater(
	const net::CompletionCallback& callback, int result) {
		MessageLoop::current()->PostTask(
			FROM_HERE, base::Bind(&bgNetworkHttpTransaction::RunCallback,
			weak_factory_.GetWeakPtr(), callback, result));
}

void bgNetworkHttpTransaction::RunCallback(
	const net::CompletionCallback& callback, int result) {
		callback.Run(result);
}

bgNetworkHttpLayer::bgNetworkHttpLayer()
	: transaction_count_(0),
	done_reading_called_(false),
	last_create_transaction_priority_(net::DEFAULT_PRIORITY) {}

bgNetworkHttpLayer::~bgNetworkHttpLayer() {}

void bgNetworkHttpLayer::TransactionDoneReading() {
	done_reading_called_ = true;
}

int bgNetworkHttpLayer::CreateTransaction(
	net::RequestPriority priority,
	scoped_ptr<net::HttpTransaction>* trans,
	net::HttpTransactionDelegate* delegate) {
		transaction_count_++;
		last_create_transaction_priority_ = priority;
		scoped_ptr<bgNetworkHttpTransaction> mock_transaction(
			new bgNetworkHttpTransaction(priority, this));
		last_transaction_ = mock_transaction->AsWeakPtr();
		*trans = mock_transaction.Pass();
		return net::OK;
}

net::HttpCache* bgNetworkHttpLayer::GetCache() {
	return NULL;
}

net::HttpNetworkSession* bgNetworkHttpLayer::GetSession() {
	return NULL;
}