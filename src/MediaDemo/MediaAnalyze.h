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
	// �ϲ�ӿ�
	//
	//////////////////////////////////////////////////////////////////////////

	// ��ȡ��ʽ����
	std::string GetFormatName();

	// ��ȡ��ʽȫ��
	std::string GetFormatLongName();

	// ��ȡý�����ʱ��
	__int64 GetKeepDuration();

	// ��ȡý��ƽ���������
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
	// FFmpeg��ؽӿ�
	//
	//////////////////////////////////////////////////////////////////////////

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

private:
	int AnalyzeVideoStream(AVStream *av_stream);
	int AnalyzeAudioStream(AVStream *av_stream);

private:
	wyMediaType media_type_;

private:
	AVFormatContext *avformat_context;	// ��ʽ������
	std::string avformat_name;			// ��ʽ����
	std::string avformat_long_name;		// ��ʽȫ��
	__int64 avformat_keep_duration;		// ����ʱ��
	int avformat_bit_rate;				// ƽ���������

	// ��Ƶ���������
	int video_stream_index;				// ��Ƶ������
	AVStream *video_stream;				// ��Ƶ��
	AVCodecContext *video_codec_context;// ��Ƶ������������
	int video_width;					// ��Ƶ���
	int video_height;					// ��Ƶ�߶�
	__int64 video_duration;				// ��Ƶ����ʱ��
	AVCodecID video_codec_id;
	const AVCodec *video_codec;			// ��Ƶ������
	std::string video_codec_name;
	std::string video_codec_long_name;

	// δ����Ĳ�����
	// ��Ƶ����
	// �������
	// ɫ�ʿռ�
	// ɫ�ȳ���

	// ��Ƶ���������
	int audio_stream_index;				// ��Ƶ������
	AVStream *audio_stream;				// ��Ƶ��
	AVCodecContext *audio_codec_context;// ��Ƶ������������
	__int64 audio_duration;				// ��Ƶ����ʱ��
	AVCodecID audio_codec_id;
	const AVCodec *audio_codec;			// ��Ƶ������
	std::string audio_codec_name;
	std::string audio_codec_long_name;

	// ����ģʽ
	int audio_bit_rate;					// ��Ƶ����
	int audio_sound_track;				// ����
	int audio_sampling_rate;			// ������
};

#endif
