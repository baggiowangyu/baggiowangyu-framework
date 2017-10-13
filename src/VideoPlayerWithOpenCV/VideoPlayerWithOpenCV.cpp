// VideoPlayerWithOpenCV.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#ifdef __cplusplus
};
#endif

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	// ��ʼ�����򻷾�
	base::AtExitManager at_exit_manager;	// �����˳�����������������ں�������
	base::MessageLoop message_loop;			// ������Ϣѭ������������ں�������

	// �й������в���
	LPTSTR commandline = GetCommandLine();
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, nullptr);

	// �Ӳ��������õ�ý��·��
	if (!cmdline.HasSwitch("url"))
	{
		std::cout<<"no url ..."<<std::endl;
		return 0;
	}

	std::string url = cmdline.GetSwitchValueASCII("url");

	av_register_all();
	avformat_network_init();
	avcodec_register_all();

	AVFormatContext *input_format_context = nullptr;
	int errCode = avformat_open_input(&input_format_context, url.c_str(), nullptr, nullptr);
	if (errCode != 0)
	{
		return errCode;
	}

	avformat_find_stream_info(input_format_context, nullptr);

	int video_stream_index = 0;
	AVCodecContext *codec_context = nullptr;
	AVCodec *codec = nullptr;
	for (int index = 0; index < input_format_context->nb_streams; ++index)
	{
		switch (input_format_context->streams[index]->codec->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			video_stream_index = index;

			// �ҵ���Ƶ�����ҵ����Ľ�����
			codec_context = input_format_context->streams[video_stream_index]->codec;
			codec = avcodec_find_decoder(codec_context->codec_id);
			if (codec == nullptr)
			{
				// û�ҵ��������������˳���
				std::cout<<"not found decoder ... "<<std::endl;
				return -2;
			}

			if (codec_context->time_base.num > 1000 && codec_context->time_base.den == 1)
				codec_context->time_base.den = 1000;

			break;
		default:
			break;
		}
	}

	errCode = avcodec_open2(codec_context, codec, nullptr);
	if (errCode != 0)
	{
		std::cout<<"open decoder failed ... "<<std::endl;
		return -2;
	}

	AVFrame *frame = av_frame_alloc();
	AVFrame *frame_rgb = av_frame_alloc();

	// ������ʾͼ��������Դ��С
	int show_img_mem_size = avpicture_get_size(AV_PIX_FMT_RGB24, codec_context->width, codec_context->height);
	uint8_t *show_img_mem_buffer = (uint8_t *)av_malloc(show_img_mem_size * sizeof(uint8_t));

	// ��frame_rgb֡���ʵ��Ĳ��ָ�ֵ��ͼ�����ݻ�����
	avpicture_fill((AVPicture *)frame_rgb, show_img_mem_buffer, AV_PIX_FMT_RGB24, codec_context->width, codec_context->height);

	cv::namedWindow("test", cv::WINDOW_NORMAL);
	// ��������ʼ��֡��ȡ
	long prepts = 0;
	AVPacket packet;
	while (av_read_frame(input_format_context, &packet) >= 0)
	{
		// ������ݰ��Ƿ�������Ƶ��
		if (packet.stream_index == video_stream_index)
		{
			// ����
			int got_picture = 0;
			avcodec_decode_video2(codec_context, frame, &got_picture, &packet);

			if (got_picture)
			{
				static struct SwsContext *img_convert_ctx = nullptr;

				// ����Ҫ����SwsGetContext
				if (img_convert_ctx == nullptr)
					img_convert_ctx = sws_getContext(codec_context->width, codec_context->height, codec_context->pix_fmt, codec_context->width, codec_context->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, nullptr, nullptr, nullptr);

				// ת��ͼ��
				errCode = sws_scale(img_convert_ctx, frame->data, frame->linesize, 0, codec_context->height, frame_rgb->data, frame_rgb->linesize);
				if (!errCode)
				{
					std::cout<<"ͼ��ת��ʧ��"<<std::endl;
					continue;
				}

				// ���ｫrgbͼ��תΪOpenCVʶ����ڴ��ʽ
				// ��Ҫ�Ĳ���frame_rgb, codecctx->width, codecctx->height, packet->pts-prepts

				int time = packet.pts-prepts;
				if (time <= 0)
					time = 1;

				cv::Mat frame_image(cv::Size(codec_context->width, codec_context->height), CV_8UC3, cv::Scalar(0));
				int step_width = frame_image.step;
				int channels = frame_image.channels();
				unsigned char *data = frame_image.data;

				// ����ɨ�裿
				for (int i = 0; i < codec_context->height; ++i)
				{
					for (int j = 0; j < codec_context->width; ++j)
					{
						data[i * step_width + j * channels + 0] = frame_rgb->data[0][i * frame_rgb->linesize[0] + j * channels + 2];
						data[i * step_width + j * channels + 1] = frame_rgb->data[0][i * frame_rgb->linesize[0] + j * channels + 1];
						data[i * step_width + j * channels + 2] = frame_rgb->data[0][i * frame_rgb->linesize[0] + j * channels + 0];
					}
				}

				
				cv::imshow("test", frame_image);
				cv::waitKey(time);

				prepts = packet.pts;
			}
		}
	}

	return 0;
}

