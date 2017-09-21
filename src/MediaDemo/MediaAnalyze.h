#ifndef _MEDIA_ANALYZE_H_
#define _MEDIA_ANALYZE_H_

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

	// 获取媒体总时长，存在视频流则优先返回视频流时长
	__int64 GetMediaDuration();

private:
	int AnalyzeVideoStream(AVStream *av_stream);
	int AnalyzeAudioStream(AVStream *av_stream);

private:
	wyMediaType media_type_;
	__int64 media_duration_;

private:
	AVFormatContext *avformat_context;

	// 视频流相关数据
	int video_stream_index;
	AVStream *video_stream;
	AVCodecContext *video_codec_context;
	const AVCodec *video_codec;
	int video_width;
	int video_height;
	__int64 video_duration;

	// 音频流相关数据
	int audio_stream_index;
	AVStream *audio_stream;
	AVCodecContext *audio_codec_context;
	const AVCodec *audio_codec;
	__int64 audio_duration;
};

#endif
