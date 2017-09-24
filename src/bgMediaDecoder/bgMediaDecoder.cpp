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
#ifdef __cplusplus
};
#endif

#include "bgMediaDecoder.h"

bgMediaDecoder::bgMediaDecoder(bgMediaDecoderCallback *callback)
	: decoder_callback_(callback)
	, decode_thread_(new base::Thread("bgDecodeThread"))
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

	std::string media_url_a = base::SysWideToUTF8(media_url.value());

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

void bgMediaDecoder::Working(bgMediaDecoder *decoder)
{
	bgMediaDecoder *bg_decoder = (bgMediaDecoder*)decoder;

	// 打开输入文件（流）
	AVFormatContext *avformat_input_context = nullptr;
	int errCode = avformat_open_input(&avformat_input_context, bg_decoder->media_url_a.c_str(), nullptr, nullptr);
	if (errCode != 0)
	{
		LOG(ERROR)<<"Open media url : "<<bg_decoder->media_url_a.c_str()<<" failed. errCode : "<<errCode;
		return ;
	}

	// 查找文件（流）信息
	errCode = avformat_find_stream_info(avformat_input_context, nullptr);
	if (errCode < 0)
	{
		LOG(ERROR)<<"Find "<<bg_decoder->media_url_a.c_str()<<" stream info failed. errCode : "<<errCode;
		avformat_close_input(&avformat_input_context);
		return ;
	}

	// 找到视频流
	int video_stream_index = -1;
	int audio_stream_index = -1;
	int streams_count = avformat_input_context->nb_streams;
	for (int index = 0; index < streams_count; ++index)
	{
		AVStream *av_stream = avformat_input_context->streams[index];
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

	AVCodecContext *video_codec_context = avformat_input_context->streams[video_stream_index]->codec;
	AVCodecContext *audio_codec_context = avformat_input_context->streams[audio_stream_index]->codec;

	// 通知上层播放器组件
	if (bg_decoder->decoder_callback_)
		bg_decoder->decoder_callback_->VideoSizeCallback(video_codec_context->width, video_codec_context->height);

	// 查找对应的解码器
	AVCodec *video_codec = avcodec_find_decoder(video_codec_context->codec_id);
	AVCodec *audio_codec = avcodec_find_decoder(audio_codec_context->codec_id);

	// 打开对应的解码器
	errCode = avcodec_open2(video_codec_context, video_codec, nullptr);
	if (errCode < 0)
	{
		LOG(ERROR)<<"Open video decoder failed. errCode : "<<errCode;
		return ;
	}

	errCode = avcodec_open2(audio_codec_context, audio_codec, nullptr);
	if (errCode < 0)
	{
		LOG(ERROR)<<"Open audio decoder failed. errCode : "<<errCode;
		return ;
	}

	// 图像转换
	struct SwsContext *img_convert_ctx = sws_getContext(video_codec_context->width, video_codec_context->height, video_codec_context->pix_fmt, video_codec_context->width, video_codec_context->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);

	// 开始读取每一帧并解码了
	AVPacket av_packet;
	while (av_read_frame(avformat_input_context, &av_packet) >= 0)
	{
		// 如果编码包是视频流，则进行解码
		if (av_packet.stream_index == video_stream_index)
		{
			int got_picture = 0;
			AVFrame *video_frame = av_frame_alloc();
			errCode = avcodec_decode_video2(video_codec_context, video_frame, &got_picture, &av_packet);
			if (errCode < 0)
			{
				LOG(ERROR)<<"Decode failed. errCode : "<<errCode;
				return ;
			}

			if (got_picture)
			{
				bg_decoder->decoder_callback_->MediaDecoderCallback(video_frame, sizeof(AVFrame));
			}
		}
	}

	while (true)
	{
		int got_picture = 0;
		AVFrame *video_frame = av_frame_alloc();
		errCode = avcodec_decode_video2(video_codec_context, video_frame, &got_picture, &av_packet);
		if (errCode < 0)
			break;

		if (!got_picture)
			break;

		bg_decoder->decoder_callback_->MediaDecoderCallback(video_frame, sizeof(AVFrame));
	}

	return ;
}