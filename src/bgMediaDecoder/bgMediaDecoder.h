#ifndef _BG_MEDIA_DECODER_H_
#define _BG_MEDIA_DECODER_H_

#include "base/file_util.h"
#include "base/threading/thread.h"
#include "base/synchronization/lock.h"

struct AVFrame;
struct AVFormatContext;
struct AVCodecContext;
struct AVCodec;

class bgMediaDecoderCallback
{
public:
	virtual void VideoSizeCallback(int width, int height) = 0;
	virtual void MediaDecoderCallback(AVFrame* frame_data) = 0;
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
	int UpdatePlayerScreenSize(int width, int height);

public:
	static void Working(bgMediaDecoder *decoder);

public:
	std::string media_url_a;
	bgMediaDecoderCallback *decoder_callback_;
	base::Thread *decode_thread_;

	base::Lock img_convert_ctx_lock;
	int current_player_screen_width;
	int current_player_screen_height;
	int player_screen_width;
	int player_screen_height;
	struct SwsContext *img_convert_ctx;

	AVFormatContext *avformat_input_context;
	AVCodecContext *video_codec_context;
	AVCodecContext *audio_codec_context;
	AVCodec *video_codec;
	AVCodec *audio_codec;
};

#endif