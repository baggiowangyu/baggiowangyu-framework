#include "stdafx.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#ifdef __cplusplus
	};
#endif

#include "MediaAnalyze.h"

MediaAnalyze::MediaAnalyze()
	: avformat_context(nullptr)
	, video_stream(nullptr)
	, audio_stream(nullptr)
{

}

MediaAnalyze::~MediaAnalyze()
{

}

int MediaAnalyze::Open(const char *media_url)
{
	int errCode = 0;

	// ��ý���ļ�
	errCode = avformat_open_input(&avformat_context, media_url, nullptr, nullptr);
	if (errCode != 0)
		return errCode;

	// ��������Ϣ
	errCode = avformat_find_stream_info(avformat_context, nullptr);
	if (errCode < 0)
	{
		avformat_close_input(&avformat_context);
		return errCode;
	}

	// ��������Ϣ
	avformat_name = avformat_context->iformat->name;
	avformat_long_name = avformat_context->iformat->long_name;
	avformat_keep_duration = avformat_context->duration;
	avformat_bit_rate = avformat_context->bit_rate;

	// �ֱ��ҵ���Ƶ������Ƶ��
	int number_of_streams = avformat_context->nb_streams;
	for (int index = 0; index < number_of_streams; ++index)
	{
		if (avformat_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_stream_index = index;
			video_stream = avformat_context->streams[video_stream_index];
			errCode = this->AnalyzeVideoStream(video_stream);
		}
		else if (avformat_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_stream_index = index;
			audio_stream = avformat_context->streams[audio_stream_index];
			errCode = this->AnalyzeAudioStream(audio_stream);
		}
	}

	return errCode;
}

void MediaAnalyze::Close()
{

}

std::string MediaAnalyze::GetFormatName()
{
	return avformat_name;
}

std::string MediaAnalyze::GetFormatLongName()
{
	return avformat_long_name;
}

__int64 MediaAnalyze::GetKeepDuration()
{
	return avformat_keep_duration / 1000 / 1000;
}

int MediaAnalyze::GetMixBitRate()
{
	return avformat_bit_rate;
}

bool MediaAnalyze::HasVideoInfo()
{
	if (video_stream)
		return true;
	else
		return false;
}

bool MediaAnalyze::HasAudioInfo()
{
	if (audio_stream)
		return true;
	else
		return false;
}

int MediaAnalyze::GetVideoCodecId()
{
	return (int)video_codec_id;
}

int MediaAnalyze::GetAudioCodecId()
{
	return (int)audio_codec_id;
}

std::string MediaAnalyze::GetVideoCodecName()
{
	return video_codec_name;
}

std::string MediaAnalyze::GetAudioCodecName()
{
	return audio_codec_name;
}

std::string MediaAnalyze::GetVideoCodecLongName()
{
	return video_codec_long_name;
}

std::string MediaAnalyze::GetAudioCodecLongName()
{
	return audio_codec_long_name;
}

__int64 MediaAnalyze::GetVideoDuration()
{
	return video_duration;
}

__int64 MediaAnalyze::GetAudioDuration()
{
	return audio_duration;
}

int MediaAnalyze::GetVideoWidth()
{
	return video_width;
}

int MediaAnalyze::GetVideoHeight()
{
	return video_height;
}

int MediaAnalyze::GetAudioBitRate()
{
	return audio_bit_rate;
}

int MediaAnalyze::GetAudioChannelCount()
{
	return audio_sound_track;
}

int MediaAnalyze::GetAudioSanplingRate()
{
	return audio_sampling_rate;
}

//////////////////////////////////////////////////////////////////////////
AVStream* MediaAnalyze::GetVideoStream()
{
	return video_stream;
}

AVStream* MediaAnalyze::GetAudioStream()
{
	return audio_stream;
}

AVCodecContext* MediaAnalyze::GetVideoCodecContext()
{
	return video_codec_context;
}

AVCodecContext* MediaAnalyze::GetAudioCodecContext()
{
	return audio_codec_context;
}

const AVCodec* MediaAnalyze::GetVideoCodec()
{
	return video_codec;
}

const AVCodec* MediaAnalyze::GetAudioCodec()
{
	return audio_codec;
}

AVFrame* MediaAnalyze::GetIFrameByIndex(int i_frame_index /* = 0 */)
{
	AVCodec *avcodec = avcodec_find_decoder(video_codec_context->codec_id);
	avcodec_open2(video_codec_context, avcodec, nullptr);

	// ����Ҫ��ȡ��index���ؼ�֡
	int key_frame_index = 0;
	AVFrame *av_frame = av_frame_alloc();
	AVPacket av_packet;
	while (av_read_frame(avformat_context, &av_packet) == 0)
	{
		// ������Ӧ����Ҫ�������֡
		if (av_packet.stream_index == video_stream_index)
		{
			int got_frame = 0;
			avcodec_decode_video2(video_codec_context, av_frame, &got_frame, &av_packet);

			if (got_frame)
			{
				if (av_frame->key_frame)
				{
					// �ǹؼ�֡
					if (key_frame_index == i_frame_index)
					{
						// ��ô��������֡����
						return av_frame;
					}
					else
					{
						++key_frame_index;
						continue;
					}
				}
			}
		}
	}

	av_frame_free(&av_frame);
	return nullptr;
}

int MediaAnalyze::AnalyzeVideoStream(AVStream *av_stream)
{
	// ���￪ʼ������Ƶ��
	int errCode = 0;

	// ���ȵõ������������ģ������������������õ���Ƶ��Ⱥ͸߶�
	video_codec_context = av_stream->codec;

	video_codec_id = video_codec_context->codec_id;
	video_width = video_codec_context->width;
	video_height = video_codec_context->height;
	video_duration = video_stream->duration * av_q2d(video_stream->time_base);

	// �õ���Ƶ���������������п����õ����������ơ�ȫ��������
	video_codec = avcodec_find_decoder(video_codec_id);
	video_codec_name = video_codec->name;
	video_codec_long_name = video_codec->long_name;

	// ִ�н���
	errCode = avcodec_open2(video_codec_context, video_codec, nullptr);
	if (0 != errCode)
		return errCode;

	while (true)
	{
		// ��Ƶ�����洢δ�������Ƶ֡
		AVPacket av_packet;

		errCode = av_read_frame(avformat_context, &av_packet);
		if (0 != errCode)
			break;

		AVFrame *av_frame = av_frame_alloc();

		int got_picture = 0;
		avcodec_decode_video2(video_codec_context, av_frame, &got_picture, &av_packet);

		if (!got_picture)
			continue;

		// ����ɹ��ˣ�Ӧ����һЩ�ֶ��Ѿ��������������
		if (av_frame->key_frame)
		{
			// �ǹؼ�֡
			video_pix_format = video_codec_context->pix_fmt;
			av_frame_free(&av_frame);
			break;
		}

		av_frame_free(&av_frame);
	}

	return errCode;
}

int MediaAnalyze::AnalyzeAudioStream(AVStream *av_stream)
{
	// ���￪ʼ������Ƶ��
	int errCode = 0;

	audio_codec_context = av_stream->codec;

	audio_bit_rate = audio_codec_context->bit_rate;
	audio_sound_track = audio_codec_context->channels;
	audio_sampling_rate = audio_codec_context->sample_rate;
	audio_codec_id = audio_codec_context->codec_id;
	audio_duration = audio_stream->duration * av_q2d(audio_stream->time_base);

	audio_codec = avcodec_find_decoder(audio_codec_id);
	audio_codec_name = audio_codec->name;
	audio_codec_long_name = audio_codec->long_name;

	

	return errCode;
}

wyMediaType MediaAnalyze::GetMediaType()
{
	return media_type_;
}