#include "stdafx.h"

#include "base/bind.h"
#include "base/location.h"

#include "bgMediaDecoder_V3.h"

bgMediaDecoderV3::bgMediaDecoderV3(bgMediaDecoderV3Notify *notifer)
	: notifer_(notifer)
	, decode_thread_(new base::Thread("bgMediaDecoderV3_decode_thread"))
	, state_(StandBy)
{

}

bgMediaDecoderV3::~bgMediaDecoderV3()
{
	delete decode_thread_;
	decode_thread_ = nullptr;

	notifer_ = nullptr;
}

int bgMediaDecoderV3::OpenMedia(const char *url)
{
	int errCode = 0;
	state_ = BeforeDecode;
	notifer_->StateNotify("", state_);

	av_register_all();
	avformat_network_init();
	avcodec_register_all();

	url_ = url;

	input_format_context_ = avformat_alloc_context();
	errCode = avformat_open_input(&input_format_context_, url, nullptr, nullptr);
	if (errCode != 0)
	{
		state_ = StandBy;
		notifer_->StateNotify("", state_);
		std::string errstr = "open input url failed ...";
		notifer_->ErrorNotify(errstr, errCode);
		return -1;
	}

	avformat_find_stream_info(input_format_context_, nullptr);

	for (int index = 0; index < input_format_context_->nb_streams; ++index)
	{
		switch (input_format_context_->streams[index]->codec->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			input_video_stream_index_ = index;
			input_video_codec_context_ = input_format_context_->streams[index]->codec;
			input_video_codec_ = avcodec_find_decoder(input_video_codec_context_->codec_id);
			if (!input_video_codec_)
			{
				std::string errstr = "not find video decoder...";
				errCode = -2;
				notifer_->ErrorNotify(errstr, errCode);
				break;
			}

			errCode = avcodec_open2(input_video_codec_context_, input_video_codec_, nullptr);
			if (errCode != 0)
			{
				LOG(ERROR)<<"open video decoder failed... errcode : "<<errCode;
				std::string errstr = "open video decoder failed...";
				notifer_->ErrorNotify(errstr, errCode);
				break;
			}

			break;
		case AVMEDIA_TYPE_AUDIO:
			input_audio_stream_index_ = index;
			input_audio_codec_context_ = input_format_context_->streams[index]->codec;
			input_audio_codec_ = avcodec_find_decoder(input_audio_codec_context_->codec_id);
			if (!input_audio_codec_)
			{
				LOG(ERROR)<<"not find audio decoder...";
				std::string errstr = "not find audio decoder...";
				errCode = -3;
				notifer_->ErrorNotify(errstr, errCode);
				break;
			}

			errCode = avcodec_open2(input_audio_codec_context_, input_audio_codec_, nullptr);
			if (errCode != 0)
			{
				LOG(ERROR)<<"open audio decoder failed... errcode : "<<errCode;
				std::string errstr = "open audio decoder failed...";
				notifer_->ErrorNotify(errstr, errCode);
				break;
			}

			break;
		case AVMEDIA_TYPE_SUBTITLE:
			input_subtitle_stream_index_ = index;
			input_subtitle_codec_context_ = input_format_context_->streams[index]->codec;
			input_subtitle_codec_ = avcodec_find_decoder(input_subtitle_codec_context_->codec_id);
			if (!input_subtitle_codec_)
			{
				LOG(ERROR)<<"not find subtitle decoder...";
				std::string errstr = "not find subtitle decoder...";
				errCode = -4;
				notifer_->ErrorNotify(errstr, errCode);
				break;
			}

			errCode = avcodec_open2(input_subtitle_codec_context_, input_subtitle_codec_, nullptr);
			if (errCode != 0)
			{
				LOG(ERROR)<<"open subtitle decoder failed... errcode : "<<errCode;
				std::string errstr = "open subtitle decoder failed...";
				notifer_->ErrorNotify(errstr, errCode);
				break;
			}

			break;
		default:
			break;
		}
	}

	decode_thread_->Start();
	decode_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&bgMediaDecoderV3::DecodeTask, this));

	return 0;
}

void bgMediaDecoderV3::Close()
{

}

void bgMediaDecoderV3::DecodeTask(bgMediaDecoderV3 *decoder)
{
	decoder->state_ = Decoding;
	decoder->notifer_->StateNotify("", decoder->state_);

	bool is_notify_video_info = false;
	bool is_notify_audio_info = false;
	AVPacket packet;
	while (av_read_frame(decoder->input_format_context_, &packet) == 0)
	{
		AVFrame *frame = av_frame_alloc();
		if (packet.stream_index == decoder->input_video_stream_index_)
		{
			// 解码视频
			int got_picture = 0;
			avcodec_decode_video2(decoder->input_video_codec_context_, frame, &got_picture, &packet);

			if (got_picture)
			{
				if (!is_notify_video_info)
				{
					// 还没有向上层通知视频流信息，那么在这里组织一下传递到上面去
					MediaVideoInfo video_info;
					video_info.pixel_format_ = decoder->input_video_codec_context_->pix_fmt;
					video_info.codec_width_ = decoder->input_video_codec_context_->width;
					video_info.codec_height_ = decoder->input_video_codec_context_->height;
					video_info.bit_rate_ = decoder->input_video_codec_context_->bit_rate;
					video_info.gop_size_ = decoder->input_video_codec_context_->gop_size;
					video_info.time_base_ = decoder->input_video_codec_context_->time_base;
					video_info.qmin_ = decoder->input_video_codec_context_->qmin;
					video_info.qmax_ = decoder->input_video_codec_context_->qmax;
					video_info.max_b_frames_ = decoder->input_video_codec_context_->max_b_frames;
					video_info.frame_rate_ = decoder->input_format_context_->streams[decoder->input_video_stream_index_]->r_frame_rate;

					decoder->notifer_->VideoInfoNotify(video_info);
					is_notify_video_info = true;
				}
				decoder->notifer_->DecodeNotify(frame, AVMEDIA_TYPE_VIDEO);
				av_frame_free(&frame);
			}
		}
		else if (packet.stream_index == decoder->input_audio_stream_index_)
		{
			// 解码音频
			int got_sound = 0;
			avcodec_decode_audio4(decoder->input_audio_codec_context_, frame, &got_sound, &packet);

			decoder->notifer_->DecodeNotify(frame, AVMEDIA_TYPE_AUDIO);
		}
		//else if (packet.stream_index == decoder->input_subtitle_stream_index_)
		//{
		//	// 解码字幕
		//	int got_subtitle = 0;
		//	avcodec_decode_subtitle2(decoder->input_subtitle_codec_context_, frame, &got_subtitle, &packet);
		//}
		else
		{
		}
	}

	decoder->state_ = DecodeFinished;
	decoder->notifer_->StateNotify("", decoder->state_);
}