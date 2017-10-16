#ifndef _BG_MEDIA_DECODER_V3_H_
#define _BG_MEDIA_DECODER_V3_H_

#include "base/string_util.h"
#include "base/threading/thread.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
#ifdef __cplusplus
	};
#endif

/*

媒体解码器V3设计思路：

1. 支持本地媒体文件、本地摄像头采集数据与网络媒体流
2. 在解码过程中同时分析出媒体基本信息
3. 提供回调接口，将解码后的数据帧返回给上层

*/

#ifndef _Struct_MediaVideoInfo_
#define _Struct_MediaVideoInfo_
typedef struct _MediaVideoInfo_
{
	AVPixelFormat pixel_format_;
	int codec_width_;		// 源视频宽度
	int codec_height_;		// 源视频高度
	int64_t bit_rate_;			// 比特率，这里仅供参考，解码时
	int gop_size_;			// 图组中图片的数量，解码的时候应该是没用的，编码的时候由用户摄这是
	AVRational time_base_;	// 
	int qmin_;				// 
	int qmax_;				// 
	int max_b_frames_;		// 
} MediaVideoInfo, *PMediaVideoInfo;
#endif

class bgMediaDecoderV3Notify
{
public:
	virtual void StateNotify(std::string statestr, int stateindex) = 0;
	virtual void ErrorNotify(std::string errstr, int errcode) = 0;
	virtual void DecodeNotify(AVFrame *frame_data, int frame_type) = 0;
	virtual void VideoInfoNotify(MediaVideoInfo video_info) = 0;
	virtual void AudioInfoNotify() = 0;
};

class bgMediaDecoderV3
{
public:
	bgMediaDecoderV3(bgMediaDecoderV3Notify *notifer);
	~bgMediaDecoderV3();

public:
	int OpenMedia(const char *url);
	void Close();

public:
	static void DecodeTask(bgMediaDecoderV3 *decoder);

public:
	base::Thread *decode_thread_;
	bgMediaDecoderV3Notify *notifer_;

	std::string url_;
	AVFormatContext *input_format_context_;

	AVCodecContext *input_video_codec_context_;
	AVCodecContext *input_audio_codec_context_;
	AVCodecContext *input_subtitle_codec_context_;

	AVCodec *input_video_codec_;
	AVCodec *input_audio_codec_;
	AVCodec *input_subtitle_codec_;

	int input_video_stream_index_;
	int input_audio_stream_index_;
	int input_subtitle_stream_index_;

};

#endif