#ifndef _BG_NETWORK_HTTP_V3_H_
#define _BG_NETWORK_HTTP_V3_H_

#include <vector>
#include <string>
#include <WinInet.h>

class bgNetworkHttpV3
{
public:
	bgNetworkHttpV3();
	virtual ~bgNetworkHttpV3();

public:
	int ConnectToServer(const char *server, int port = 80, const char *proxy = nullptr);

public:
	void AddExternHeader(const char *header);
	void CleanupExternHeaders();
	int SendGetRequest(const char *object, HINTERNET *request_handle);
	int SendPostRequest(const char *object, const unsigned char *post_data, int post_len, HINTERNET *request_handle);

	// 响应头内存在函数内分配，在外部自行释放
	int GetResponseRawHeader(HINTERNET request_handle, TCHAR **raw_header);
	int GetResponseCookie(HINTERNET request_handle, TCHAR **cookie, DWORD *index);
	int GetResponseContentLength(HINTERNET request_handle, DWORD *length, DWORD *index);
	int GetResponseRawData(HINTERNET request_handle, unsigned char **raw_data, DWORD *byte_to_read);
	
private:
	std::vector<std::string> headers_;
	std::string response_header_;
	HINTERNET internet_handle_;
	HINTERNET connect_handle_;
};
#endif//_BG_NETWORK_HTTP_V3_H_