#ifndef _BG_NETWORK_HTTP_
#define _BG_NETWORK_HTTP_

#include "base/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/string16.h"
#include "net/base/io_buffer.h"
#include "net/base/load_flags.h"
#include "net/base/net_errors.h"
#include "net/base/request_priority.h"
#include "net/disk_cache/disk_cache.h"
#include "net/http/http_cache.h"
#include "net/http/http_transaction.h"
#include "net/http/http_transaction_factory.h"
#include "net/http/http_request_info.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_response_info.h"

#include <string>

//
// ’‚li
//

class bgNetworkHttpLayer;

typedef void (*bgTransactionHandler)(const net::HttpRequestInfo* request,
	std::string* response_status,
	std::string* response_headers,
	std::string* response_data);

struct bgTransaction {
	const char* url;
	const char* method;
	// If |request_time| is unspecified, the current time will be used.
	base::Time request_time;
	const char* request_headers;
	int load_flags;
	const char* status;
	const char* response_headers;
	// If |response_time| is unspecified, the current time will be used.
	base::Time response_time;
	const char* data;
	int test_mode;
	bgTransactionHandler handler;
	net::CertStatus cert_status;
	// Value returned by MockNetworkTransaction::Start (potentially
	// asynchronously if |!(test_mode & TEST_MODE_SYNC_NET_START)|.)
	net::Error return_code;
};

// returns the mock transaction for the given URL
const bgTransaction* FindbgTransaction(const GURL& url);

// Add/Remove a mock transaction that can be accessed via FindMockTransaction.
// There can be only one MockTransaction associated with a given URL.
void AddbgTransaction(const bgTransaction* trans);
void RemovebgTransaction(const bgTransaction* trans);

struct ScopedbgTransaction : bgTransaction {
	ScopedbgTransaction() {
		AddbgTransaction(this);
	}
	explicit ScopedbgTransaction(const bgTransaction& t)
		: bgTransaction(t) {
			AddbgTransaction(this);
	}
	~ScopedbgTransaction() {
		RemovebgTransaction(this);
	}
};

//-----------------------------------------------------------------------------
// mock http request

class bgHttpRequest : public net::HttpRequestInfo {
public:
	explicit bgHttpRequest(const bgTransaction& t);
};

//-----------------------------------------------------------------------------
// use this class to test completely consuming a transaction
class bgNetworkHttpTransactionClient
	//: public net::HttpTransaction
	//, public base::SupportsWeakPtr<bgNetworkHttpTransactionClient>
{
public:
	bgNetworkHttpTransactionClient(net::RequestPriority priority, net::HttpTransactionFactory* factory);
	virtual ~bgNetworkHttpTransactionClient();

public:
	void Start(const net::HttpRequestInfo* request,
		const net::BoundNetLog& net_log);

	bool is_done() const { return state_ == DONE; }
	int error() const { return error_; }

	const net::HttpResponseInfo* response_info() const {
		return trans_->GetResponseInfo();
	}

	const std::string& content() const { return content_; }

private:
	enum State {
		IDLE,
		STARTING,
		READING,
		DONE
	};

private:
	void DidStart(int result);
	void DidRead(int result);
	void DidFinish(int result);
	void Read();

	void OnIOComplete(int result);

private:
	State state_;
	scoped_ptr<net::HttpTransaction> trans_;
	std::string content_;
	scoped_refptr<net::IOBuffer> read_buf_;
	int error_;

	int quit_counter_;
};


// This transaction class inspects the available set of mock transactions to
// find data for the request URL.  It supports IO operations that complete
// synchronously or asynchronously to help exercise different code paths in the
// HttpCache implementation.
class bgNetworkHttpTransaction
	: public net::HttpTransaction,
	public base::SupportsWeakPtr<bgNetworkHttpTransaction> {
public:
	bgNetworkHttpTransaction(net::RequestPriority priority,
		bgNetworkHttpLayer* factory);
	virtual ~bgNetworkHttpTransaction();

	virtual int Start(const net::HttpRequestInfo* request,
		const net::CompletionCallback& callback,
		const net::BoundNetLog& net_log) OVERRIDE;

	virtual int RestartIgnoringLastError(
		const net::CompletionCallback& callback) OVERRIDE;

	virtual int RestartWithCertificate(
		net::X509Certificate* client_cert,
		const net::CompletionCallback& callback) OVERRIDE;

	virtual int RestartWithAuth(
		const net::AuthCredentials& credentials,
		const net::CompletionCallback& callback) OVERRIDE;

	virtual bool IsReadyToRestartForAuth() OVERRIDE;

	virtual int Read(net::IOBuffer* buf, int buf_len,
		const net::CompletionCallback& callback) OVERRIDE;

	virtual void StopCaching() OVERRIDE;

	virtual void DoneReading() OVERRIDE;

	virtual const net::HttpResponseInfo* GetResponseInfo() const OVERRIDE;

	virtual net::LoadState GetLoadState() const OVERRIDE;

	virtual net::UploadProgress GetUploadProgress() const OVERRIDE;

	virtual bool GetLoadTimingInfo(
		net::LoadTimingInfo* load_timing_info) const OVERRIDE;

	virtual void SetPriority(net::RequestPriority priority) OVERRIDE;

	net::RequestPriority priority() const { return priority_; }

private:
	void CallbackLater(const net::CompletionCallback& callback, int result);
	void RunCallback(const net::CompletionCallback& callback, int result);

	base::WeakPtrFactory<bgNetworkHttpTransaction> weak_factory_;
	net::HttpResponseInfo response_;
	std::string data_;
	int data_cursor_;
	int test_mode_;
	net::RequestPriority priority_;
	base::WeakPtr<bgNetworkHttpLayer> transaction_factory_;
};


class bgNetworkHttpLayer : public net::HttpTransactionFactory,
	public base::SupportsWeakPtr<bgNetworkHttpLayer> {
public:
	bgNetworkHttpLayer();
	virtual ~bgNetworkHttpLayer();

	int transaction_count() const { return transaction_count_; }
	bool done_reading_called() const { return done_reading_called_; }
	void TransactionDoneReading();

	// Returns the last priority passed to CreateTransaction, or
	// DEFAULT_PRIORITY if it hasn't been called yet.
	net::RequestPriority last_create_transaction_priority() const {
		return last_create_transaction_priority_;
	}

	// Returns the last transaction created by
	// CreateTransaction. Returns a NULL WeakPtr if one has not been
	// created yet, or the last transaction has been destroyed, or
	// ClearLastTransaction() has been called and a new transaction
	// hasn't been created yet.
	base::WeakPtr<bgNetworkHttpTransaction> last_transaction() {
		return last_transaction_;
	}

	// Makes last_transaction() return NULL until the next transaction
	// is created.
	void ClearLastTransaction() {
		last_transaction_.reset();
	}

	// net::HttpTransactionFactory:
	virtual int CreateTransaction(
		net::RequestPriority priority,
		scoped_ptr<net::HttpTransaction>* trans,
		net::HttpTransactionDelegate* delegate) OVERRIDE;
	virtual net::HttpCache* GetCache() OVERRIDE;
	virtual net::HttpNetworkSession* GetSession() OVERRIDE;

private:
	int transaction_count_;
	bool done_reading_called_;
	net::RequestPriority last_create_transaction_priority_;
	base::WeakPtr<bgNetworkHttpTransaction> last_transaction_;
};

#endif
