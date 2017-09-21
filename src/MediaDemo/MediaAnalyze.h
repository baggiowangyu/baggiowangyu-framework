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
	// ��ȡ��Ƶ��ָ��
	AVStream* GetVideoStream();

	// ��ȡ��Ƶ��ָ��
	AVStream* GetAudioStream();

	// ��ȡ��Ƶ�����������
	AVCodecContext* GetVideoCodecContext();

	// ��ȡ��Ƶ�����������
	AVCodecContext* GetAudioCodecContext();

	// ��ȡ��Ƶ������
	const AVCodec* GetVideoCodec();

	// ��ȡ��Ƶ������
	const AVCodec* GetAudioCodec();

	// ��ȡ��������Ƶ�ؼ�֡
	AVFrame* GetIFrameByIndex(int i_frame_index = 0);

public:
	// ��ȡý������
	wyMediaType GetMediaType();

	// ��ȡý����ʱ����������Ƶ�������ȷ�����Ƶ��ʱ��
	__int64 GetMediaDuration();

private:
	int AnalyzeVideoStream(AVStream *av_stream);
	int AnalyzeAudioStream(AVStream *av_stream);

private:
	wyMediaType media_type_;
	__int64 media_duration_;

private:
	AVFormatContext *avformat_context;

	// ��Ƶ���������
	int video_stream_index;
	AVStream *video_stream;
	AVCodecContext *video_codec_context;
	const AVCodec *video_codec;
	int video_width;
	int video_height;
	__int64 video_duration;

	// ��Ƶ���������
	int audio_stream_index;
	AVStream *audio_stream;
	AVCodecContext *audio_codec_context;
	const AVCodec *audio_codec;
	__int64 audio_duration;
};

#endif
