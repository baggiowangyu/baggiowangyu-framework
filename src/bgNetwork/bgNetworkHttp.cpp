#include "stdafx.h"

#include "bgNetworkHttp.h"

bgNetworkHttp::bgNetworkHttp(net::RequestPriority priority, net::HttpTransactionFactory* factory)
	: state_(IDLE)
	, trans_(nullptr)
	, error_(net::OK)
{
	factory->CreateTransaction(priority, &trans_, nullptr);
	++quit_counter_;
}