#include "stdafx.h"

#include "bgNetworkHttp.h"

bgNetworkHttpTransaction::bgNetworkHttpTransaction(net::RequestPriority priority, net::HttpTransactionFactory* factory)
	: state_(IDLE)
	, trans_(nullptr)
	, error_(net::OK)
{
	factory->CreateTransaction(priority, &trans_, nullptr);
	++quit_counter_;
}

bgNetworkHttpTransaction::~bgNetworkHttpTransaction()
{

}

void bgNetworkHttpTransaction::Start(const net::HttpRequestInfo* request, const net::BoundNetLog& net_log)
{
	state_ = STARTING;
	int result = trans_->Start(request, base::Bind(&bgNetworkHttpTransaction::OnIOComplete, base::Unretained(this)), net_log);

	if (result != net::ERR_IO_PENDING)
		DidStart(result);
}

void bgNetworkHttpTransaction::DidStart(int result)
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

void bgNetworkHttpTransaction::DidRead(int result)
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

void bgNetworkHttpTransaction::DidFinish(int result)
{
	state_ = DONE;
	error_ = result;
	if (--quit_counter_ == 0)
		MessageLoop::current()->Quit();
}

void bgNetworkHttpTransaction::Read()
{
	state_ = READING;
	read_buf_ = new net::IOBuffer(1024);
	int result = trans_->Read(read_buf_, 1024, base::Bind(&bgNetworkHttpTransaction::OnIOComplete, base::Unretained(this)));

	if (result != net::ERR_IO_PENDING)
		DidRead(result);
}

void bgNetworkHttpTransaction::OnIOComplete(int result)
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