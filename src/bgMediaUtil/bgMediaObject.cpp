#include "stdafx.h"

#include "base/logging.h"

#include "bgMediaObject.h"

bgMediaObject::bgMediaObject()
	: media_format_context_(avformat_alloc_context())
	, video_stream_index_(-1)
	, audio_stream_index_(-1)
	, video_stream_(nullptr)
	, audio_stream_(nullptr)
	, video_codec_context_(nullptr)
	, audio_codec_context_(nullptr)
	, video_codec_(nullptr)
	, audio_codec_(nullptr)
{

}

bgMediaObject::~bgMediaObject()
{

}

int bgMediaObject::Open(std::string media_url)
{
	int errCode = 0;

	av_register_all();
	avformat_network_init();
	avcodec_register_all();

	errCode = avformat_open_input(&media_format_context_, media_url.c_str(), nullptr, nullptr);
	if (errCode != 0)
	{
		LOG(ERROR)<<"Open media failed. errCode : "<<errCode;
		return errCode;
	}
	
	errCode = avformat_find_stream_info(media_format_context_, nullptr);
	if (errCode < 0)
	{
		avformat_close_input(&media_format_context_);
		LOG(ERROR)<<"Find stream info failed. errCode : "<<errCode;
		return errCode;
	}

	// 这里可以提取总体信息了
	format_duration_ = media_format_context_->duration;
	format_overall_bit_rate_ = media_format_context_->bit_rate;
	format_codec_id_ = media_format_context_->data_codec_id;

	int stream_count = media_format_context_->nb_streams;
	for (int index = 0; index < stream_count; ++index)
	{
		if (media_format_context_->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			video_stream_index_ = index;

		if (media_format_context_->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
			audio_stream_index_ = index;
	}

	if (video_stream_index_ > -1)
	{
		video_stream_ = media_format_context_->streams[video_stream_index_];
		video_codec_context_ = video_stream_->codec;

		video_codec_ = avcodec_find_decoder(video_codec_context_->codec_id);
		if (video_codec_ != nullptr)
		{
			errCode = avcodec_open2(video_codec_context_, video_codec_, nullptr);
			if (errCode == 0)
			{
				// 解码一部分，拿到视频流更细节的信息
				errCode = DecodeMedia(video_codec_);
			}
			else
				LOG(ERROR)<<"Open video decoder failed. errCode : "<<errCode;
		}
		else
		{
			LOG(ERROR)<<"Not found video decoder...";
			errCode = -3;
		}
	}
	
	if (audio_stream_index_ > -1)
	{
		audio_stream_ = media_format_context_->streams[audio_stream_index_];
		audio_codec_context_ = audio_stream_->codec;

		audio_codec_ = avcodec_find_decoder(audio_codec_context_->codec_id);
		if (audio_codec_ != nullptr)
		{
			errCode = avcodec_open2(audio_codec_context_, audio_codec_, nullptr);
			if (errCode == 0)
			{
				// 解码一部分，拿到音频流更细节的信息
				errCode = DecodeMedia(audio_codec_);
			}
			else
				LOG(ERROR)<<"Open video decoder failed. errCode : "<<errCode;
		}
		else
		{
			LOG(ERROR)<<"Not found audio decoder...";
			errCode = -3;
		}
	}

	return errCode;
}

void bgMediaObject::Close()
{

}

std::string bgMediaObject::GetFormatName()
{
	return media_format_context_->iformat->name;
}

std::string bgMediaObject::GetFormatLongName()
{
	return media_format_context_->iformat->long_name;
}

int64_t bgMediaObject::GetDuration()
{
	return media_format_context_->duration;
}

int64_t bgMediaObject::GetOverallBitRate()
{
	return format_overall_bit_rate_;
}

AVCodecID bgMediaObject::GetVideoCodecID()
{
	return video_codec_->id;
}

std::string bgMediaObject::GetVideoCodecName()
{
	return video_codec_->name;
}

std::string bgMediaObject::GetVideoCodecLongName()
{
	return video_codec_->long_name;
}

int64_t bgMediaObject::GetVideoDuration()
{
	return video_duration_;
}

int64_t bgMediaObject::GetVideoBitRate()
{
	return video_bit_rate_;
}

int bgMediaObject::GetVideoWidth()
{
	return video_resolution_w_;
}

int bgMediaObject::GetVideoHeight()
{
	return video_resolution_h_;
}

AVRational bgMediaObject::GetVideoFrameRate()
{
	return video_frame_rate_;
}

AVPixelFormat bgMediaObject::GetVideoPixelFormat()
{
	return video_pixel_format_;
}

AVColorSpace bgMediaObject::GetVideoColorSpace()
{
	return video_color_space_;
}

AVColorRange bgMediaObject::GetVideoColorRange()
{
	return video_color_range_;
}

AVCodecID bgMediaObject::GetAudioCodecID()
{
	return audio_codec_->id;
}

std::string bgMediaObject::GetAudioCodecName()
{
	return audio_codec_->name;
}

std::string bgMediaObject::GetAudioCodecLongName()
{
	return audio_codec_->long_name;
}

int64_t bgMediaObject::GetAudioDuration()
{
	return audio_duration_;
}

int64_t bgMediaObject::GetAudioBitRate()
{
	return audio_bit_rate_;
}

int bgMediaObject::GetAudioChannels()
{
	return audio_channel_count_;
}

int bgMediaObject::GetAudioSampleRate()
{
	return audio_sample_rate_;
}

AVRational bgMediaObject::GetAudioFrameRate()
{
	return audio_frame_rate_;
}

int bgMediaObject::DecodeMedia(AVCodec *decoder)
{
	int errCode = 0;
	AVPacket av_packet;
	AVFrame *av_frame = av_frame_alloc();

	while (true)
	{
		errCode = av_read_frame(media_format_context_, &av_packet);
		if (errCode < 0)
			break;

		if (decoder->type == AVMEDIA_TYPE_VIDEO)
		{
			int got_pic = 0;
			errCode = avcodec_decode_video2(video_codec_context_, av_frame, &got_pic, &av_packet);

			if (!got_pic)
				continue;

			if (!av_frame->key_frame)
				continue;

			// 在这里我们统一拿一下视频流数据
			// 1. 视频分辨率
			// 2. 视频波特率
			// 3. 视频帧率
			// 4. 视频像素格式
			// 5. 视频颜色范围
			// 6. 视频颜色空间
			// 7. 视频时长
			// 8. 
			// ...

			video_resolution_w_ = video_codec_context_->width;
			video_resolution_h_ = video_codec_context_->height;

			video_duration_ = video_stream_->duration * av_q2d(video_stream_->time_base);
			video_bit_rate_ = video_codec_context_->bit_rate;
			video_frame_rate_ = video_codec_context_->framerate;
			video_pixel_format_ = video_codec_context_->pix_fmt;
			video_color_range_ = av_frame->color_range;
			video_color_space_ = av_frame->colorspace;

			break;
		}

		if (decoder->type == AVMEDIA_TYPE_AUDIO)
		{
			int got_frame_ptr = 0;
			errCode = avcodec_decode_audio4(audio_codec_context_, av_frame, &got_frame_ptr, &av_packet);

			if (!got_frame_ptr)
				continue;

			if (!av_frame->key_frame)
				continue;

			// 在这里我们统一拿一下音频流的数据
			// 1. 采样率
			// 2. 声道
			// 3. 时长
			// 4. 波特率
			// 5. 
			// ...

			audio_sample_rate_ = av_frame->sample_rate;
			audio_channel_count_ = av_frame->channels;

			audio_duration_ = audio_stream_->duration * av_q2d(audio_stream_->time_base);
			audio_bit_rate_ = audio_codec_context_->bit_rate;
			audio_frame_rate_ = audio_codec_context_->framerate;

			break;
		}
	}

	return errCode;
}