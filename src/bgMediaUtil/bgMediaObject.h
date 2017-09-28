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

// 媒体对象
// 作为媒体文件的抽象
class bgMediaObject
{
public:
	bgMediaObject();
	~bgMediaObject();

public:
	/**
	 * 打开媒体文件
	 * 参数：
	 *	@media_url		媒体路径
	 */
	int Open(std::string media_url);

	/**
	 * 关闭媒体文件
	 */
	void Close();

public:
	// 总体信息

	// 格式名称
	std::string GetFormatName();

	// 格式全称
	std::string GetFormatLongName();

	// 总体耗时
	int64_t GetDuration();

	// 全局平均比特率
	int64_t GetOverallBitRate();

public:
	// 视频流信息

	// 视频编码ID
	AVCodecID GetVideoCodecID();

	// 编码名称
	std::string GetVideoCodecName();

	// 编码全程
	std::string GetVideoCodecLongName();

	// 视频时长
	int64_t GetVideoDuration();

	// 视频比特率
	int64_t GetVideoBitRate();

	// 视频源宽度
	int GetVideoWidth();

	// 视频源高度
	int GetVideoHeight();

	// 视频帧率
	AVRational GetVideoFrameRate();

	// 视频像素格式
	AVPixelFormat GetVideoPixelFormat();

	// 视频色彩空间
	AVColorSpace GetVideoColorSpace();

	// 视频色彩范围
	AVColorRange GetVideoColorRange();

public:
	// 音频流信息

	// 音频编码ID
	AVCodecID GetAudioCodecID();

	// 音频编码名称
	std::string GetAudioCodecName();

	// 音频编码全名
	std::string GetAudioCodecLongName();

	// 音频时长
	int64_t GetAudioDuration();

	// 音频比特率
	int64_t GetAudioBitRate();

	// 音频音轨数量
	int GetAudioChannels();

	// 音频采样率
	int GetAudioSampleRate();

	// 音频帧率
	AVRational GetAudioFrameRate();

private:
	/**
	 * 解码媒体
	 * 参数：
	 *	@decoder		解码器
	 */
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
