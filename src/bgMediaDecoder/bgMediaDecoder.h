#ifndef _BG_MEDIA_DECODER_H_
#define _BG_MEDIA_DECODER_H_

#include "base/file_util.h"
#include "base/threading/thread.h"

class bgMediaDecoderCallback
{
public:
	virtual void VideoSizeCallback(int width, int height) = 0;
	virtual void MediaDecoderCallback(void* frame_data, int frame_data_len) = 0;
};

class bgMediaDecoder
{
public:
	bgMediaDecoder(bgMediaDecoderCallback *callback);
	~bgMediaDecoder();

public:
	int OpenMedia(base::FilePath media_url);
	int StartDecode();
	void StopDecode();

public:
	static void Working(bgMediaDecoder *decoder);

public:
	std::string media_url_a;
	bgMediaDecoderCallback *decoder_callback_;
	base::Thread *decode_thread_;
};

#endif