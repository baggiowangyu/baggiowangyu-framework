#ifndef _MEDIA_ANALYZE_H_
#define _MEDIA_ANALYZE_H_

#include <string>
#include "MediaDataInfo.h"

class MediaAnalyze
{
public:
	MediaAnalyze();
	~MediaAnalyze();

public:
	int Open(const char *media_url);
	void Close();

public:
	//////////////////////////////////////////////////////////////////////////
	//
	// 上层接口
	//
	//////////////////////////////////////////////////////////////////////////

	// 获取格式名称
	std::string GetFormatName();

	// 获取格式全程
	std::string GetFormatLongName();

	// 获取媒体持续时间
	__int64 GetKeepDuration();

	// 获取媒体平均混合码率
	int GetMixBitRate();

	bool HasVideoInfo();
	bool HasAudioInfo();

	int GetVideoCodecId();
	int GetAudioCodecId();

	std::string GetVideoCodecName();
	std::string GetAudioCodecName();

	std::string GetVideoCodecLongName();
	std::string GetAudioCodecLongName();

	__int64 GetVideoDuration();
	__int64 GetAudioDuration();

	int GetVideoWidth();
	int GetVideoHeight();

	int GetAudioBitRate();
	int GetAudioChannelCount();
	int GetAudioSanplingRate();

public:
	//////////////////////////////////////////////////////////////////////////
	//
	// FFmpeg相关接口
	//
	//////////////////////////////////////////////////////////////////////////

	// 获取视频流指针
	AVStream* GetVideoStream();

	// 获取音频流指针
	AVStream* GetAudioStream();

	// 获取视频编解码上下文
	AVCodecContext* GetVideoCodecContext();

	// 获取音频编解码上下文
	AVCodecContext* GetAudioCodecContext();

	// 获取视频解码器
	const AVCodec* GetVideoCodec();

	// 获取音频解码器
	const AVCodec* GetAudioCodec();

	// 获取解码后的视频关键帧
	AVFrame* GetIFrameByIndex(int i_frame_index = 0);

	

public:
	// 获取媒体类型
	wyMediaType GetMediaType();

private:
	int AnalyzeVideoStream(AVStream *av_stream);
	int AnalyzeAudioStream(AVStream *av_stream);

private:
	wyMediaType media_type_;

private:
	AVFormatContext *avformat_context;	// 格式上下文
	std::string avformat_name;			// 格式名称
	std::string avformat_long_name;		// 格式全名
	__int64 avformat_keep_duration;		// 持续时间
	int avformat_bit_rate;				// 平均混合码率

	// 视频流相关数据
	int video_stream_index;				// 视频流索引
	AVStream *video_stream;				// 视频流
	AVCodecContext *video_codec_context;// 视频解码器上下文
	int video_width;					// 视频宽度
	int video_height;					// 视频高度
	__int64 video_duration;				// 视频持续时间
	AVCodecID video_codec_id;
	const AVCodec *video_codec;			// 视频解码器
	std::string video_codec_name;
	std::string video_codec_long_name;

	// 未处理的参数：
	// 视频码率
	// 画面比例
	// 色彩空间
	// 色度抽样

	// 音频流相关数据
	int audio_stream_index;				// 音频流索引
	AVStream *audio_stream;				// 音频流
	AVCodecContext *audio_codec_context;// 音频解码器上下文
	__int64 audio_duration;				// 音频持续时间
	AVCodecID audio_codec_id;
	const AVCodec *audio_codec;			// 音频解码器
	std::string audio_codec_name;
	std::string audio_codec_long_name;

	// 码率模式
	int audio_bit_rate;					// 音频码率
	int audio_sound_track;				// 声道
	int audio_sampling_rate;			// 采样率
};

#endif
