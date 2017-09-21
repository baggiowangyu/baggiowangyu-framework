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

	// 打开媒体文件
	errCode = avformat_open_input(&avformat_context, media_url, nullptr, nullptr);
	if (errCode != 0)
		return errCode;

	// 查找流信息
	errCode = avformat_find_stream_info(avformat_context, nullptr);
	if (errCode < 0)
	{
		avformat_close_input(&avformat_context);
		return errCode;
	}

	// 解析音视频流
	int number_of_streams = avformat_context->nb_streams;
	for (int index = 0; index < number_of_streams; ++index)
	{
		AVStream *av_stream = avformat_context->streams[index];
		if (av_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_stream_index = index;
			video_stream = av_stream;
			errCode = this->AnalyzeVideoStream(video_stream);
		}
		else if (av_stream->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_stream_index = index;
			audio_stream = av_stream;
			errCode = this->AnalyzeAudioStream(audio_stream);
		}
	}

	return errCode;
}

void MediaAnalyze::Close()
{

}

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

	// 这里要读取第index个关键帧
	int key_frame_index = 0;
	AVFrame *av_frame = av_frame_alloc();
	AVPacket av_packet;
	while (av_read_frame(avformat_context, &av_packet) == 0)
	{
		// 接下来应该是要解码这个帧
		if (av_packet.stream_index == video_stream_index)
		{
			int got_frame = 0;
			avcodec_decode_video2(video_codec_context, av_frame, &got_frame, &av_packet);

			if (got_frame)
			{
				if (av_frame->key_frame)
				{
					// 是关键帧
					if (key_frame_index == i_frame_index)
					{
						// 那么将解码后的帧返回
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
	int errCode = 0;

	video_codec_context = av_stream->codec;
	video_codec = video_codec_context->codec;
	video_width = video_codec_context->width;
	video_height = video_codec_context->height;
	video_duration = av_stream->duration * av_q2d(av_stream->time_base);

	return errCode;
}

int MediaAnalyze::AnalyzeAudioStream(AVStream *av_stream)
{
	int errCode = 0;

	audio_codec_context = av_stream->codec;
	audio_codec = audio_codec_context->codec;
	audio_duration = av_stream->duration * av_q2d(av_stream->time_base);

	return errCode;
}

wyMediaType MediaAnalyze::GetMediaType()
{
	return media_type_;
}

__int64 MediaAnalyze::GetMediaDuration()
{
	return media_duration_;
}