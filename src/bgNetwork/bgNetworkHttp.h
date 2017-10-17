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

class bgNetworkHttp
{
public:
	bgNetworkHttp(net::RequestPriority priority, net::HttpTransactionFactory* factory);
	virtual ~bgNetworkHttp();

public:
	void Start(const net::HttpRequestInfo* request, const net::BoundNetLog& net_log);
	bool is_done();
	int error();

	const net::HttpResponseInfo* response_info();

	std::string& content();

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

	static int quit_counter_;
};

#endif
