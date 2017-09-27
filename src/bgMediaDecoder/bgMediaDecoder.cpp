// bgMediaDecoder.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "base/strings/sys_string_conversions.h"
#include "base/logging.h"
#include "base/location.h"
#include "base/bind.h"

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

#include "bgMediaDecoder.h"

bgMediaDecoder::bgMediaDecoder(bgMediaDecoderCallback *callback)
	: decoder_callback_(callback)
	, decode_thread_(new base::Thread("bgDecodeThread"))
	, player_screen_width(0)
	, player_screen_height(0)
	, current_player_screen_width(0)
	, current_player_screen_height(0)
	, img_convert_ctx(nullptr)
	, avformat_input_context(nullptr)
	, video_codec_context(nullptr)
	, audio_codec_context(nullptr)
	, video_codec(nullptr)
	, audio_codec(nullptr)
{

}

bgMediaDecoder::~bgMediaDecoder()
{

}

int bgMediaDecoder::OpenMedia(base::FilePath media_url)
{
	int errCode = 0;

	av_register_all();
	avcodec_register_all();
	avformat_network_init();

	media_url_a = base::SysWideToUTF8(media_url.value());

	return errCode;
}

int bgMediaDecoder::StartDecode()
{
	decode_thread_->Start();
	decode_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&bgMediaDecoder::Working, this));

	return 0;
}

void bgMediaDecoder::StopDecode()
{
	decode_thread_->Stop();

	return ;
}

int bgMediaDecoder::UpdatePlayerScreenSize(int width, int height)
{
	int errCode = 0;

	// 在更新的过程中其实是需要上锁的，以免出现错误

	img_convert_ctx_lock.Acquire();
	player_screen_width = width;
	player_screen_height = height;
	img_convert_ctx_lock.Release();
	
	return errCode;
}

void bgMediaDecoder::Working(bgMediaDecoder *decoder)
{
	bgMediaDecoder *bg_decoder = (bgMediaDecoder*)decoder;

	// 打开输入文件（流）
	
	int errCode = avformat_open_input(&bg_decoder->avformat_input_context, bg_decoder->media_url_a.c_str(), nullptr, nullptr);
	if (errCode != 0)
	{
		LOG(ERROR)<<"Open media url : "<<bg_decoder->media_url_a.c_str()<<" failed. errCode : "<<errCode;
		return ;
	}

	// 查找文件（流）信息
	errCode = avformat_find_stream_info(bg_decoder->avformat_input_context, nullptr);
	if (errCode < 0)
	{
		LOG(ERROR)<<"Find "<<bg_decoder->media_url_a.c_str()<<" stream info failed. errCode : "<<errCode;
		avformat_close_input(&bg_decoder->avformat_input_context);
		return ;
	}

	// 找到视频流
	int video_stream_index = -1;
	int audio_stream_index = -1;
	int streams_count = bg_decoder->avformat_input_context->nb_streams;
	for (int index = 0; index < streams_count; ++index)
	{
		AVStream *av_stream = bg_decoder->avformat_input_context->streams[index];
		switch (av_stream->codec->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			video_stream_index = index;
			break;
		case AVMEDIA_TYPE_AUDIO:
			audio_stream_index = index;
			break;
		default:
			break;
		}
	}

	bg_decoder->video_codec_context = bg_decoder->avformat_input_context->streams[video_stream_index]->codec;
	bg_decoder->audio_codec_context = bg_decoder->avformat_input_context->streams[audio_stream_index]->codec;

	// 通知上层播放器组件，目标媒体视频宽高
	if (bg_decoder->decoder_callback_)
		bg_decoder->decoder_callback_->VideoSizeCallback(bg_decoder->video_codec_context->width, bg_decoder->video_codec_context->height);

	// 查找对应的解码器
	bg_decoder->video_codec = avcodec_find_decoder(bg_decoder->video_codec_context->codec_id);
	bg_decoder->audio_codec = avcodec_find_decoder(bg_decoder->audio_codec_context->codec_id);

	// 打开对应的解码器
	errCode = avcodec_open2(bg_decoder->video_codec_context, bg_decoder->video_codec, nullptr);
	if (errCode < 0)
	{
		LOG(ERROR)<<"Open video decoder failed. errCode : "<<errCode;
		return ;
	}

	errCode = avcodec_open2(bg_decoder->audio_codec_context, bg_decoder->audio_codec, nullptr);
	if (errCode < 0)
	{
		LOG(ERROR)<<"Open audio decoder failed. errCode : "<<errCode;
		return ;
	}

	// 申请帧内存
	//AVFrame *av_frame = av_frame_alloc();
	AVFrame *av_frame_yuv = av_frame_alloc();

	// 申请输出缓冲区内存
	int buffer_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, bg_decoder->video_codec_context->width, bg_decoder->video_codec_context->height, 1);
	unsigned char *out_buffer = (unsigned char *)av_malloc(buffer_size);

	// 填充图像数组
	av_image_fill_arrays(av_frame_yuv->data, av_frame_yuv->linesize, out_buffer, AV_PIX_FMT_YUV420P, bg_decoder->video_codec_context->width, bg_decoder->video_codec_context->height, 1);

	// 开始读取每一帧并解码了
	AVPacket av_packet;
	while (av_read_frame(bg_decoder->avformat_input_context, &av_packet) >= 0)
	{
		// 如果编码包是视频流，则进行解码
		if (av_packet.stream_index != video_stream_index)
			continue;

		int got_picture = 0;
		AVFrame *video_frame = av_frame_alloc();
		errCode = avcodec_decode_video2(bg_decoder->video_codec_context, video_frame, &got_picture, &av_packet);
		if (errCode < 0)
		{
			// 解码失败，为了能保证后面的视频能正常播放，我们就跳过这一个包，继续读取下一个进行解码
			LOG(ERROR)<<"Decode failed. errCode : "<<errCode;
			continue;
		}

		if (got_picture)
		{
			// 这里执行像素转换
			bg_decoder->img_convert_ctx_lock.Acquire();

			// 这个地方进行了重新分配上下文
			// 频繁执行这个操作可能会消耗大量资源，需要一个机制进行控制
			// 只有播放窗口规格发生变化后才重新获取一次上下文
			if ((bg_decoder->current_player_screen_width != bg_decoder->player_screen_width) ||
				(bg_decoder->current_player_screen_height != bg_decoder->player_screen_height))
			{
				bg_decoder->current_player_screen_width = bg_decoder->player_screen_width;
				bg_decoder->current_player_screen_height = bg_decoder->player_screen_height;

				if (bg_decoder->img_convert_ctx)
				{
					sws_freeContext(bg_decoder->img_convert_ctx);
					bg_decoder->img_convert_ctx = nullptr;
				}

				// 获取像素转换上下文，这个应该就是视频源转到播放目标上的像素转换了，应该移动到播放器里面进行适应性匹配
				// 或者，由播放器调用解码器的接口进行调整
				bg_decoder->img_convert_ctx = sws_getContext(bg_decoder->video_codec_context->width, bg_decoder->video_codec_context->height, bg_decoder->video_codec_context->pix_fmt, 
					bg_decoder->current_player_screen_width, bg_decoder->current_player_screen_height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);

				if (bg_decoder->img_convert_ctx)
				{
					bg_decoder->current_player_screen_width = bg_decoder->player_screen_width;
				}
			}

			sws_scale(bg_decoder->img_convert_ctx, (const unsigned char * const *)video_frame->data, video_frame->linesize, 0, bg_decoder->video_codec_context->height, av_frame_yuv->data, av_frame_yuv->linesize);
			bg_decoder->img_convert_ctx_lock.Release();

			bg_decoder->decoder_callback_->MediaDecoderCallback(av_frame_yuv);
		}
		
		//av_frame_free(&video_frame);
	}

	return ;
}