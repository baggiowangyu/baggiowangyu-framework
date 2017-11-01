// VideoCaptureFromCam.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"

#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
};
#endif

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

	if (!cmdline.HasSwitch("face_data"))
	{
		std::cout<<"û�з�����������������"<<std::endl;
		system("pause");
		return 0;
	}

	if (!cmdline.HasSwitch("eyes_data"))
	{
		std::cout<<"û�з�����������������"<<std::endl;
		system("pause");
		return 0;
	}

	std::string face_factor = cmdline.GetSwitchValueASCII("face_data");
	std::string eyes_factor = cmdline.GetSwitchValueASCII("eyes_data");

	// ��������ֵ
	cv::CascadeClassifier face_cascade;	// ����������
	bool result = face_cascade.load(face_factor.c_str());
	if (!result)
	{
		std::cout<<"����������������ʧ�ܣ�"<<std::endl;
		system("pause");
		return 0;
	}

	cv::CascadeClassifier eyes_cascade;	// ���۷�����
	result = eyes_cascade.load(eyes_factor.c_str());
	if (!result)
	{
		std::cout<<"����������������ʧ�ܣ�"<<std::endl;
		system("pause");
		return 0;
	}

	// ���ȼ�鵱ǰ�������û�н�������ͷ
	av_register_all();
	avformat_network_init();
	avdevice_register_all();
	avcodec_register_all();

	AVFormatContext *device_format_context = avformat_alloc_context();
	AVInputFormat *device_input_format = av_find_input_format("vfwcap");

	int errCode = avformat_open_input(&device_format_context, "0", device_input_format, NULL);
	if (errCode != 0)
	{
		std::cout<<"������ͷʧ�ܣ�"<<std::endl;
		return -1;
	}

	errCode = avformat_find_stream_info(device_format_context, nullptr);
	if (errCode < 0)
	{
		std::cout<<"û���ҵ�����Ϣ��"<<std::endl;
		return -1;
	}

	int video_stream_index = -1;
	int audio_stream_index = -1;

	for (int index = 0; index < device_format_context->nb_streams; ++index)
	{
		switch (device_format_context->streams[index]->codec->codec_type)
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

	AVCodecContext *video_codec_context = device_format_context->streams[video_stream_index]->codec;
	AVCodecContext *audio_codec_context = device_format_context->streams[audio_stream_index]->codec;

	AVCodec *video_codec = avcodec_find_decoder(video_codec_context->codec_id);
	if (video_codec == nullptr)
	{
		std::cout<<"û���ҵ���Ƶ��������"<<std::endl;
		return -2;
	}

	errCode = avcodec_open2(video_codec_context, video_codec, nullptr);
	if (errCode < 0)
	{
		std::cout<<"����Ƶ������ʧ�ܣ�"<<std::endl;
		return -3;
	}

	AVPacket *packet = av_packet_alloc();
	AVFrame *frame = av_frame_alloc();
	AVFrame *frame_bgr = av_frame_alloc();

	int video_width = video_codec_context->width;
	int video_height = video_codec_context->height;
	AVPixelFormat video_pixel_format = video_codec_context->pix_fmt;

	struct SwsContext *img_convert_ctx = nullptr;
	img_convert_ctx = sws_getContext(video_width, video_height, video_pixel_format,
		video_width, video_height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

	int size = avpicture_get_size(AV_PIX_FMT_BGR24, video_width, video_height);  
	uint8_t *out_buffer = (uint8_t *)av_malloc(size);  
	avpicture_fill((AVPicture *)frame_bgr, out_buffer, AV_PIX_FMT_BGR24, video_width, video_height);

	cv::Mat pCvMat;  
	pCvMat.create(cv::Size(video_width, video_height), CV_8UC3);

	cvNamedWindow("Camera", 1); 

	bool is_decode = false;
	while (av_read_frame(device_format_context, packet) >= 0)
	{
		int got_picture = 0;
		avcodec_decode_video2(video_codec_context, frame, &got_picture, packet);

		if (got_picture)
		{
			sws_scale(img_convert_ctx, (const uint8_t* const*)frame->data,
				frame->linesize, 0, video_height, frame_bgr->data, frame_bgr->linesize);

			memcpy(pCvMat.data, out_buffer, size);

			// �����������ʶ���������Ƶ��������Ӱ��Ƚϴ󣬶�֡������
			cv::Mat image_frame_gray;
			cv::cvtColor(pCvMat, image_frame_gray, cv::COLOR_BGR2GRAY);

			// ������ΪHAAR������LBP����
			std::vector<cv::Rect> faces;
			face_cascade.detectMultiScale(image_frame_gray, faces, 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH, cv::Size(10, 10), cv::Size(800, 800));

			for (size_t index = 0; index < faces.size(); ++index)
			{
				// �������ϻ���
				cv::rectangle(pCvMat, faces[index], cv::Scalar(255, 0, 0), 2, 8, 0);
			}

			cv::imshow("Camera", pCvMat);  
			cv::waitKey(1);
		}
	}

	return 0;
}

