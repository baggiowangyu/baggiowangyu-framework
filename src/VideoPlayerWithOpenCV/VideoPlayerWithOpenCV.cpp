// VideoPlayerWithOpenCV.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"

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

	// ��������ʼ��֡��ȡ
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
				img_convert
			}
		}
	}

	return 0;
}

