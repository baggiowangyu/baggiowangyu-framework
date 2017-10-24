#ifndef _BG_LIVE_INFO_H_
#define _BG_LIVE_INFO_H_

#include <string>

class LiveAppInfo
{
public:
	LiveAppInfo();
	~LiveAppInfo();

public:
	std::string app_name_;		// 应用名称
	std::string app_id_;		// 应用ID

};

#endif