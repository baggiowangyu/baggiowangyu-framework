#include "stdafx.h"

#include "base/synchronization/waitable_event.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#ifdef __cplusplus
};
#endif

#include "bgMediaDecoderV2.h"

bgMediaDecoderV2::bgMediaDecoderV2()
	: decode_thread_(new base::Thread("decode_thread_"))
	, decode_video_thread_(new base::Thread("decode_video_thread_"))
	, decode_audio_thread_(new base::Thread("decode_audio_thread_"))
{
	decod_video_state_ = Decoder_Ready;
	decod_audio_state_ = Decoder_Ready;
}

bgMediaDecoderV2::~bgMediaDecoderV2()
{

}

int bgMediaDecoderV2::OpenMedia(std::string media_url)
{
	int errCode = 0;

	media_url_ = media_url;

	decode_thread_->Start();
	decode_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&bgMediaDecoderV2::DecodeTask, this, media_url));

	return errCode;
}

void bgMediaDecoderV2::Close()
{

}

AVFrame* bgMediaDecoderV2::GetNextVideoFrame()
{
	// 检查解码器是否已经全部解码完成
	// 检查链表是否为空，如果为空，并且解码器还在解码过程中，那么这里就等待1毫秒
	while (video_frame_list_.head() == video_frame_list_.tail())
	{
		if (decod_video_state_ == Decoder_Working)
		{
			// 链表为空了，等待1微秒后再检查
			base::WaitableEvent wait_event(false, false);
			base::TimeDelta time_delta = base::TimeDelta::FromMicroseconds(1);
		}
		else
			return nullptr;
	}

	base::LinkNode<MediaFrameNode> *node = video_frame_list_.head();
	MediaFrameNode *frame_node = node->value();
	AVFrame *frame = frame_node->GetFrame();

	// 从缓存队列中移除
	frame_node->RemoveFromList();

	// 添加到收集队列，收集队列用于收集已经播放过的帧，后期方便统一清理内存
	video_frame_collection_list_.Append(frame_node);

	return frame;
}

AVFrame* bgMediaDecoderV2::GetNextAudioFrame()
{
	while (audio_frame_list_.head() == audio_frame_list_.tail())
	{
		if (decod_audio_state_ == Decoder_Working)
		{
			// 链表为空了，等待1微秒后再检查
			base::WaitableEvent wait_event(false, false);
			base::TimeDelta time_delta = base::TimeDelta::FromMicroseconds(1);
		}
		else
			return nullptr;
	}

	base::LinkNode<MediaFrameNode> *node = audio_frame_list_.head();
	MediaFrameNode *frame_node = node->value();
	AVFrame *frame = frame_node->GetFrame();

	// 从缓存队列中移除
	frame_node->RemoveFromList();

	// 添加到收集队列，收集队列用于收集已经播放过的帧，后期方便统一清理内存
	audio_frame_collection_list_.Append(frame_node);

	return frame;
}

void bgMediaDecoderV2::ReleaseFrame(AVFrame *frame)
{
	av_frame_free(&frame);
	frame = nullptr;
}

void bgMediaDecoderV2::DecodeTask(bgMediaDecoderV2 *decoder_v2, std::string media_url)
{
	av_register_all();
	avformat_network_init();
	avcodec_register_all();

	AVFormatContext *media_format_context = nullptr;
	int errCode = avformat_open_input(&media_format_context, media_url.c_str(), nullptr, nullptr);
	if (errCode != 0)
	{
		LOG(ERROR)<<"Open media failed. errCode : "<<errCode;
		return ;
	}

	errCode = avformat_find_stream_info(media_format_context, nullptr);
	if (errCode < 0)
	{
		avformat_close_input(&media_format_context);
		LOG(ERROR)<<"Find stream info failed. errCode : "<<errCode;
		return ;
	}

	int video_stream_index = -1;
	int audio_stream_index = -1;

	int stream_count = media_format_context->nb_streams;
	for (int index = 0; index < stream_count; ++index)
	{
		if (media_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			video_stream_index = index;

		if (media_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
			audio_stream_index = index;
	}

	if (video_stream_index > -1)
	{
		decoder_v2->decode_video_thread_->Start();
		decoder_v2->decode_video_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&bgMediaDecoderV2::DecodeVideoTask, decoder_v2, media_format_context, video_stream_index));
	}

	if (audio_stream_index > -1)
	{
		decoder_v2->decode_audio_thread_->Start();
		decoder_v2->decode_audio_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&bgMediaDecoderV2::DecodeAudioTask, decoder_v2, media_format_context, audio_stream_index));
	}
}

void bgMediaDecoderV2::DecodeVideoTask(bgMediaDecoderV2 *decoder_v2, AVFormatContext *media_format_context, int stream_index)
{
	int errCode = 0;
	AVStream *video_stream_ = media_format_context->streams[stream_index];
	AVCodecContext *video_codec_context_ = video_stream_->codec;

	AVCodec *video_codec_ = avcodec_find_decoder(video_codec_context_->codec_id);
	if (video_codec_ != nullptr)
	{
		errCode = avcodec_open2(video_codec_context_, video_codec_, nullptr);
		if (errCode == 0)
		{
			while (true)
			{
				AVPacket av_packet;
				errCode = av_read_frame(media_format_context, &av_packet);
				if (errCode != 0)
					break;

				decoder_v2->decod_video_state_ = Decoder_Working;

				int got_pic = 0;
				AVFrame *video_frame = av_frame_alloc();
				avcodec_decode_video2(video_codec_context_, video_frame, &got_pic, &av_packet);

				//if (!got_pic)
				//	continue;

				//if (video_frame->key_frame != 1)
				//	continue;
				
				// 将视频帧插入视频链表
				MediaFrameNode *video_frame_node =new MediaFrameNode(video_frame);
				decoder_v2->video_frame_list_.Append(video_frame_node);
			}
		}
		else
			LOG(ERROR)<<"Open video decoder failed. errCode : "<<errCode;
	}
	else
		LOG(ERROR)<<"Not found video decoder...";

	decoder_v2->decod_video_state_ = Decoder_Ready;
}

void bgMediaDecoderV2::DecodeAudioTask(bgMediaDecoderV2 *decoder_v2, AVFormatContext *media_format_context, int stream_index)
{
	int errCode = 0;
	AVStream *audio_stream_ = media_format_context->streams[stream_index];
	AVCodecContext *audio_codec_context_ = audio_stream_->codec;

	AVCodec *audio_codec_ = avcodec_find_decoder(audio_codec_context_->codec_id);
	if (audio_codec_ != nullptr)
	{
		errCode = avcodec_open2(audio_codec_context_, audio_codec_, nullptr);
		if (errCode == 0)
		{
			//
			decoder_v2->decod_audio_state_ = Decoder_Working;
		}
		else
			LOG(ERROR)<<"Open video decoder failed. errCode : "<<errCode;
	}
	else
	{
		LOG(ERROR)<<"Not found audio decoder...";
		errCode = -3;
	}

	decoder_v2->decod_audio_state_ = Decoder_Ready;
}