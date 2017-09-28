#ifndef _BG_MEDIA_OBJECT_H_
#define _BG_MEDIA_OBJECT_H_

#include <string>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#ifdef __cplusplus
};
#endif

enum AVCodecID;
enum AVPixelFormat;
enum AVColorRange;
enum AVColorSpace;

struct AVFormatContext;
struct AVStream;
struct AVCodecContext;
struct AVCodec;
struct AVRational;

class bgMediaObject
{
public:
	bgMediaObject();
	~bgMediaObject();

public:
	int Open(std::string media_url);
	void Close();

public:
	// 总体信息
	std::string GetFormatName();
	int64_t GetDuration();
	int64_t GetOverallBitRate();

public:
	// 视频流信息
	AVCodecID GetVideoCodecID();
	std::string GetVideoCodecName();
	std::string GetVideoCodecLongName();
	int64_t GetVideoDuration();
	int64_t GetVideoBitRate();
	int GetVideoWidth();
	int GetVideoHeight();
	AVRational GetVideoFrameRate();
	AVColorSpace GetVideoColorSpace();
	AVColorRange GetVideoColorRange();

public:
	// 音频流信息
	AVCodecID GetAudioCodecID();
	std::string GetAudioCodecName();
	std::string GetAudioCodecLongName();
	int64_t GetAudioDuration();
	int64_t GetAudioBitRate();
	int GetAudioChannels();
	int GetAudioSampleRate();
	AVRational GetAudioFrameRate();

private:
	int DecodeMedia(AVCodec *decoder);

private:
	AVFormatContext *media_format_context_;

	int video_stream_index_;
	int audio_stream_index_;

	AVStream *video_stream_;
	AVStream *audio_stream_;

	AVCodecContext *video_codec_context_;
	AVCodecContext *audio_codec_context_;

	AVCodec *video_codec_;
	AVCodec *audio_codec_;

private:
	int64_t format_duration_;
	int64_t format_overall_bit_rate_;
	AVCodecID format_codec_id_;

private:
	int video_resolution_w_;
	int video_resolution_h_;

	int64_t video_duration_;
	int64_t video_bit_rate_;
	AVRational video_frame_rate_;
	AVPixelFormat video_pixel_format_;
	AVColorRange video_color_range_;
	AVColorSpace video_color_space_;

private:
	int audio_sample_rate_;
	int audio_channel_count_;

	int64_t audio_duration_;
	int64_t audio_bit_rate_;
	AVRational audio_frame_rate_;
};

#endif
