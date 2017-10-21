#include "stdafx.h"
#include <tchar.h>
#include <Windows.h>
#include <WinInet.h>
#include <atlconv.h>
#include "bgNetworkHttp_V3.h"

const TCHAR* sz_agent = _T("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.99 Safari/537.36");
const TCHAR* sz_post_header = _T("Content-Type: application/x-www-form-urlencoded");

bgNetworkHttpV3::bgNetworkHttpV3()
	: internet_handle_(nullptr)
{

}

bgNetworkHttpV3::~bgNetworkHttpV3()
{

}

int bgNetworkHttpV3::ConnectToServer(const char *server, int port, const char *proxy)
{
	int errCode = ERROR_SUCCESS;

	USES_CONVERSION;
	internet_handle_ = InternetOpen(sz_agent, INTERNET_OPEN_TYPE_DIRECT, A2T(proxy), A2T(proxy), 0);
	if (internet_handle_ == nullptr)
	{
		errCode = GetLastError();
		return errCode;
	}

	connect_handle_ = InternetConnect(internet_handle_, A2T(server), port, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
	if (connect_handle_ == nullptr)
	{
		errCode = GetLastError();
		InternetCloseHandle(internet_handle_);
		internet_handle_ = nullptr;
		return errCode;
	}

	return errCode;
}

void bgNetworkHttpV3::AddExternHeader(const char *header)
{
	headers_.push_back(header);
}

void bgNetworkHttpV3::CleanupExternHeaders()
{
	headers_.clear();
}

int bgNetworkHttpV3::SendGetRequest(const char *object, HINTERNET *request_handle)
{
	int errCode = ERROR_SUCCESS;

	if (!request_handle)
		return ERROR_INVALID_HANDLE;

	// 创建请求
	USES_CONVERSION;
	*request_handle = HttpOpenRequest(connect_handle_, _T("GET"), A2T(object), HTTP_VERSION, nullptr, nullptr, INTERNET_FLAG_KEEP_CONNECTION, 0);
	if (*request_handle == NULL)
	{
		errCode = GetLastError();
		return errCode;
	}

	// 附加请求头部
	std::vector<std::string>::iterator iter;
	for (iter = headers_.begin(); iter != headers_.end(); ++iter)
		HttpAddRequestHeaders(*request_handle, A2T(iter->c_str()), iter->size(), HTTP_ADDREQ_FLAG_COALESCE_WITH_SEMICOLON);

	// 发送请求
	BOOL result = HttpSendRequest(*request_handle, NULL, 0, NULL, 0);
	errCode = GetLastError();

	return errCode;
}

int bgNetworkHttpV3::SendPostRequest(const char *object, const unsigned char *post_data, int post_len, HINTERNET *request_handle)
{
	int errCode = ERROR_SUCCESS;

	if (!request_handle)
		return ERROR_INVALID_HANDLE;

	USES_CONVERSION;
	*request_handle = HttpOpenRequest(connect_handle_, _T("POST"), A2T(object), HTTP_VERSION, nullptr, nullptr, INTERNET_FLAG_KEEP_CONNECTION, 0);
	if (*request_handle == NULL)
	{
		errCode = GetLastError();
		return errCode;
	}

	// 附加请求头部
	std::vector<std::string>::iterator iter;
	for (iter = headers_.begin(); iter != headers_.end(); ++iter)
		HttpAddRequestHeaders(*request_handle, A2T(iter->c_str()), iter->size(), HTTP_ADDREQ_FLAG_COALESCE_WITH_SEMICOLON);

	// 发送请求
	BOOL result = HttpSendRequest(*request_handle, sz_post_header, -1, (LPVOID)post_data, post_len);
	errCode = GetLastError();

	return errCode;
}

int bgNetworkHttpV3::GetResponseRawHeader(HINTERNET request_handle, TCHAR **raw_header)
{
	int errCode = ERROR_SUCCESS;
	
	DWORD cbNeed = 0;
	DWORD index = 0;
	BOOL result = HttpQueryInfo(request_handle, HTTP_QUERY_RAW_HEADERS_CRLF, NULL, &cbNeed, &index);
	unsigned char *buffer = new unsigned char[cbNeed];
	result = HttpQueryInfo(request_handle, HTTP_QUERY_RAW_HEADERS_CRLF, buffer, &cbNeed, &index);
	errCode = GetLastError();
	if (errCode != 0)
	{
		delete [] buffer;
		buffer = NULL;
	}
	else
		*raw_header = (TCHAR *)buffer;

	return errCode;
}

int bgNetworkHttpV3::GetResponseCookie(HINTERNET request_handle, TCHAR **cookie, DWORD *index)
{
	int errCode = ERROR_SUCCESS;

	DWORD cbNeed = 0;
	BOOL result = HttpQueryInfo(request_handle, HTTP_QUERY_SET_COOKIE, NULL, &cbNeed, index);
	unsigned char *buffer = new unsigned char[cbNeed];
	result = HttpQueryInfo(request_handle, HTTP_QUERY_SET_COOKIE, buffer, &cbNeed, index);
	errCode = GetLastError();
	if (errCode != 0)
	{
		delete [] buffer;
		buffer = NULL;
	}
	else
		*cookie = (TCHAR *)buffer;

	return errCode;
}

int bgNetworkHttpV3::GetResponseContentLength(HINTERNET request_handle, DWORD *length, DWORD *index)
{
	int errCode = ERROR_SUCCESS;

	DWORD cbNeed = 0;
	BOOL result = HttpQueryInfo(request_handle, HTTP_QUERY_CONTENT_LENGTH, NULL, &cbNeed, index);
	unsigned char *buffer = new unsigned char[cbNeed];
	result = HttpQueryInfo(request_handle, HTTP_QUERY_CONTENT_LENGTH, buffer, &cbNeed, index);
	errCode = GetLastError();
	if (errCode != 0)
	{
		delete [] buffer;
		buffer = NULL;
	}
	else
		*length = _ttoi((TCHAR*)buffer);

	return errCode;
}

int bgNetworkHttpV3::GetResponseRawData(HINTERNET request_handle, unsigned char **raw_data, DWORD *byte_to_read)
{
	int errCode = ERROR_SUCCESS;

	DWORD readed = 0;
	BOOL result = InternetReadFile(request_handle, *raw_data, *byte_to_read, &readed);
	errCode = GetLastError();

	return errCode;
}