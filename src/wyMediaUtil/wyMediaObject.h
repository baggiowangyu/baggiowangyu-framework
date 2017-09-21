#ifndef _WY_MEDIA_OBJECT_H_
#define _WY_MEDIA_OBJECT_H_

class wyMediaObject
{
public:
	typedef enum _WY_MediaType_
	{
		Media_Video,	// 视频媒体
		Media_Audio		// 音频媒体

	} wyMediaType;

public:
	wyMediaObject();
	~wyMediaObject();

public:
	int Open(const char *media_path);
	int Close();

private:
	wyMediaType media_type_;	// 媒体类型
	int64 media_time_;			// 媒体时长
	std::string codec_name_;	// 编码格式
};

#endif
