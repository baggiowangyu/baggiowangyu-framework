// MediaDemo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#define __STDC_CONSTANT_MACROS

#include "base/at_exit.h"
#include "base/message_loop.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/file_util.h"
#include "base/strings/sys_string_conversions.h"

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

#include <iostream>

#include "MediaAnalyze.h"


void EnumInputOutputFormat()
{
	// Ѱ�ҵ�һ��֧�ֵ������ʽ
	std::cout<<"ö������֧�ֵ������ʽ"<<std::endl;
	LOG(INFO)<<"ö������֧�ֵ������ʽ";
	AVInputFormat *input_format = av_iformat_next(nullptr);

	while(input_format != nullptr)
	{
		// ��������Ϣ
		std::cout<<"AVInputFormat::name : "<<input_format->name<<std::endl;
		LOG(INFO)<<"AVInputFormat::name : "<<input_format->name;
		std::cout<<"AVInputFormat::long_name : "<<input_format->long_name<<std::endl;
		LOG(INFO)<<"AVInputFormat::long_name : "<<input_format->long_name;
		std::cout<<"-----------------------------------"<<std::endl;
		LOG(INFO)<<"-----------------------------------";

		// Ѱ����һ��֧�ֵ������ʽ
		input_format = av_iformat_next(input_format);
	}

	std::cout<<"ö������֧�ֵ������ʽ"<<std::endl;
	LOG(INFO)<<"ö������֧�ֵ������ʽ";
	AVOutputFormat *output_format = av_oformat_next(nullptr);

	while(output_format != nullptr)
	{
		// ��������Ϣ
		std::cout<<"AVOutputFormat::name : "<<output_format->name<<std::endl;
		LOG(INFO)<<"AVOutputFormat::name : "<<output_format->name;
		std::cout<<"AVOutputFormat::long_name : "<<output_format->long_name<<std::endl;
		LOG(INFO)<<"AVOutputFormat::long_name : "<<output_format->long_name;
		std::cout<<"-----------------------------------"<<std::endl;
		LOG(INFO)<<"-----------------------------------";

		// Ѱ����һ��֧�ֵ������ʽ
		output_format = av_oformat_next(output_format);
	}
}

void GetMediaInfo(std::string media_path_)
{
	// �����ļ�
	MediaAnalyze media_analyze;
	int errCode = media_analyze.Open(media_path_.c_str());
	if (errCode != 0)
	{
		std::cout<<"����ý���ļ� "<<media_path_.c_str()<<" ʧ�ܣ������룺"<<errCode<<std::endl;
		LOG(ERROR)<<"����ý���ļ� "<<media_path_.c_str()<<" ʧ�ܣ������룺"<<errCode;
		return ;
	}

	std::cout<<"ý���Ҫ��";
	std::cout<<"\n\t"<<"��ʽ����"<<media_analyze.GetFormatName();
	std::cout<<"\n\t"<<"��ʽȫ����"<<media_analyze.GetFormatLongName();
	std::cout<<"\n\t"<<"����ʱ��(��)��"<<media_analyze.GetKeepDuration();
	std::cout<<"\n\t"<<"ƽ��������ʣ�"<<media_analyze.GetMixBitRate();

	// ������ɣ������Ϣ
	if (media_analyze.HasVideoInfo())
	{
		std::cout<<"\n  ��Ƶ������ ��";
		std::cout<<"\n\t"<<"����ID��"<<media_analyze.GetVideoCodecId();
		std::cout<<"\n\t"<<"�������ƣ�"<<media_analyze.GetVideoCodecName();
		std::cout<<"\n\t"<<"����ȫ����"<<media_analyze.GetVideoCodecLongName();
		std::cout<<"\n\t"<<"��ʽ�ſ���";
		std::cout<<"\n\t"<<"��ʽ���ã�CABAC��";
		std::cout<<"\n\t"<<"��ʽ���ã��ο�֡��";
		std::cout<<"\n\t"<<"��ʽ���ã�GOP��";
		std::cout<<"\n\t"<<"����ʱ��(��)��"<<media_analyze.GetVideoDuration();
		std::cout<<"\n\t"<<"���ʣ�";
		std::cout<<"\n\t"<<"��Ƶ��ȣ�"<<media_analyze.GetVideoWidth();
		std::cout<<"\n\t"<<"��Ƶ�߶ȣ�"<<media_analyze.GetVideoHeight();
		std::cout<<"\n\t"<<"���������";
		std::cout<<"\n\t"<<"ɫ�ʿռ䣺"<<media_analyze.GetVideoPixFormatName();
		std::cout<<"\n\t"<<"ɫ�ʳ�����";
		std::cout<<"\n\t"<<"λ�";
		std::cout<<"\n\t"<<"ɨ�跽ʽ��";
	}

	if (media_analyze.HasAudioInfo())
	{
		std::cout<<"\n  ��Ƶ������ ��";
		std::cout<<"\n\t"<<"����ID��"<<media_analyze.GetAudioCodecId();
		std::cout<<"\n\t"<<"��ʽ��"<<media_analyze.GetAudioCodecName();
		std::cout<<"\n\t"<<"��ʽ�汾��";
		std::cout<<"\n\t"<<"��ʽ�ſ���";
		std::cout<<"\n\t"<<"ģʽ��";
		std::cout<<"\n\t"<<"��չģʽ��";
		std::cout<<"\n\t"<<"����ʱ��(��)��"<<media_analyze.GetAudioDuration();
		std::cout<<"\n\t"<<"����ģʽ��";
		std::cout<<"\n\t"<<"���ʣ�"<<media_analyze.GetAudioBitRate();
		std::cout<<"\n\t"<<"������"<<media_analyze.GetAudioChannelCount();
		std::cout<<"\n\t"<<"�����ʣ�"<<media_analyze.GetAudioSanplingRate();
		std::cout<<"\n\t"<<"ѹ��ģʽ��";
		std::cout<<"\n\t"<<"��Ƶ�ӳ٣�";
	}

	media_analyze.Close();
}

void ConvertMediaFormat(std::string media_path_, std::string target_format_)
{
	// ��������ý���ļ�
	AVFormatContext *input_context = nullptr;
	int errCode = avformat_open_input(&input_context, media_path_.c_str(), nullptr, nullptr);
	if (0 != errCode)
	{
		std::cout<<"��ý���ļ� "<<media_path_.c_str()<<" ʧ�ܣ������룺"<<errCode<<std::endl;
		LOG(ERROR)<<"��ý���ļ� "<<media_path_.c_str()<<" ʧ�ܣ������룺"<<errCode;
		return;
	}

	errCode = avformat_find_stream_info(input_context, NULL);
	if (errCode < 0)
	{
		std::cout<<"��ѯý���ļ� "<<media_path_.c_str()<<" ý����Ϣʧ�ܣ������룺"<<errCode<<std::endl;
		LOG(ERROR)<<"��ѯý���ļ� "<<media_path_.c_str()<<" ý����Ϣʧ�ܣ������룺"<<errCode;
		avformat_close_input(&input_context);
		return ;
	}

	// �������ý���ļ�
	std::string media_path_output = media_path_.append(".").append(target_format_);
	AVFormatContext *output_context = nullptr;	
	avformat_alloc_output_context2(&output_context, nullptr, nullptr, media_path_output.c_str());
	if (!output_context)
	{
		std::cout<<"����ת��ý���ļ� "<<media_path_output.c_str()<<" ʧ�ܣ������룺"<<errCode<<std::endl;
		LOG(ERROR)<<"����ת��ý���ļ� "<<media_path_output.c_str()<<" ʧ�ܣ������룺"<<errCode;
		avformat_close_input(&input_context);
		return ;
	}

	std::cout<<"======= ý����Ϣ ======="<<std::endl;
	int number_of_streams = input_context->nb_streams;
	for (int index = 0; index < number_of_streams; ++index)
	{
		AVStream *input_stream = input_context->streams[index];
		AVStream *output_stream = avformat_new_stream(output_context, input_stream->codec->codec);

		if (!output_stream)
		{
			std::cout<<"�������������ʧ�ܣ�"<<std::endl;
			LOG(ERROR)<<"�������������ʧ�ܣ�";

			avformat_close_input(&input_context);
			if (output_context && !(output_context->flags & AVFMT_NOFILE))
				avio_close(output_context->pb);
			avformat_free_context(output_context);
			return ;
		}

		errCode = avcodec_copy_context(output_stream->codec, input_stream->codec);
		if (errCode < 0)
		{
			std::cout<<"���Ʊ���������ʧ�ܣ�"<<std::endl;
			LOG(ERROR)<<"���Ʊ���������ʧ�ܣ�";
			
			avformat_close_input(&input_context);
			if (output_context && !(output_context->flags & AVFMT_NOFILE))
				avio_close(output_context->pb);
			avformat_free_context(output_context);
			return ;
		}

		output_stream->codec->codec_tag = 0;
		if (output_context->oformat->flags & AVFMT_GLOBALHEADER) {
			output_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
		}
	}

	// ������ļ�
	if (!(output_context->flags & AVFMT_NOFILE))
	{
		errCode = avio_open(&output_context->pb, media_path_output.c_str(), AVIO_FLAG_WRITE);
		if (errCode < 0)
		{
			std::cout<<"������ļ� "<<media_path_output<<" ʧ�ܣ�"<<std::endl;
			LOG(ERROR)<<"������ļ� "<<media_path_output<<" ʧ�ܣ�";

			avformat_close_input(&input_context);
			if (output_context && !(output_context->flags & AVFMT_NOFILE))
				avio_close(output_context->pb);
			avformat_free_context(output_context);
			return ;
		}
	}

	// д�ļ�ͷ
	errCode = avformat_write_header(output_context, nullptr);
	if (errCode < 0)
	{
		std::cout<<"����ļ���д�ļ�ͷʱ����"<<std::endl;
		LOG(ERROR)<<"����ļ���д�ļ�ͷʱ����";
		
		avformat_close_input(&input_context);
		if (output_context && !(output_context->flags & AVFMT_NOFILE))
			avio_close(output_context->pb);
		avformat_free_context(output_context);
		return ;
	}

	// ��֡д��
	int frame_index = 0;
	while (true)
	{
		AVPacket av_packet;

		// ��ȡһ���������֡
		errCode = av_read_frame(input_context, &av_packet);
		if (errCode < 0)
			break;

		AVStream *input_stream = input_context->streams[av_packet.stream_index];
		AVStream *output_stream = output_context->streams[av_packet.stream_index];

		// ת��PTS/DTS
		// PTS: AVPacket�е���ʾʱ���
		// DTS: ����ʱ���
		av_packet.pts = av_rescale_q_rnd(av_packet.pts, input_stream->time_base, output_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		av_packet.dts = av_rescale_q_rnd(av_packet.dts, input_stream->time_base, output_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		av_packet.duration = av_rescale_q(av_packet.duration, input_stream->time_base, output_stream->time_base);
		av_packet.pos = -1;

		// д��
		errCode = av_interleaved_write_frame(output_context, &av_packet);
		if (errCode < 0)
		{
			std::cout<<"����װ����"<<std::endl;
			LOG(ERROR)<<"����װ����";
			break;
		}

		av_free_packet(&av_packet);
		++frame_index;
	}

	// д�ļ�β
	av_write_trailer(output_context);
	
	// ������Դ
	avformat_close_input(&input_context);
	if (output_context && !(output_context->flags & AVFMT_NOFILE))
		avio_close(output_context->pb);
	avformat_free_context(output_context);
	
	return ;
}

void ConvertMediaCodec(std::string media_path_, std::string codec_name)
{
	// ��������ý���ļ�
	AVFormatContext *input_context = nullptr;
	int errCode = avformat_open_input(&input_context, media_path_.c_str(), nullptr, nullptr);
	if (0 != errCode)
	{
		std::cout<<"��ý���ļ� "<<media_path_.c_str()<<" ʧ�ܣ������룺"<<errCode<<std::endl;
		LOG(ERROR)<<"��ý���ļ� "<<media_path_.c_str()<<" ʧ�ܣ������룺"<<errCode;
		return;
	}

	errCode = avformat_find_stream_info(input_context, NULL);
	if (errCode < 0)
	{
		std::cout<<"��ѯý���ļ� "<<media_path_.c_str()<<" ý����Ϣʧ�ܣ������룺"<<errCode<<std::endl;
		LOG(ERROR)<<"��ѯý���ļ� "<<media_path_.c_str()<<" ý����Ϣʧ�ܣ������룺"<<errCode;
		avformat_close_input(&input_context);
		return ;
	}

	// �������ý���ļ�
	std::string media_path_output = media_path_.append(".").append("new");
	AVFormatContext *output_context = nullptr;	
	avformat_alloc_output_context2(&output_context, nullptr, nullptr, media_path_output.c_str());
	if (!output_context)
	{
		std::cout<<"����ת��ý���ļ� "<<media_path_output.c_str()<<" ʧ�ܣ������룺"<<errCode<<std::endl;
		LOG(ERROR)<<"����ת��ý���ļ� "<<media_path_output.c_str()<<" ʧ�ܣ������룺"<<errCode;
		avformat_close_input(&input_context);
		return ;
	}
}

// ����Ƶ��һ���ؼ�֡�������������PNG����JPG�ļ�
void GetVideoThumbnail(std::string media_path_)
{
	// ��������ý���ļ�
	AVFormatContext *input_context = nullptr;
	int errCode = avformat_open_input(&input_context, media_path_.c_str(), nullptr, nullptr);
	if (0 != errCode)
	{
		std::cout<<"��ý���ļ� "<<media_path_.c_str()<<" ʧ�ܣ������룺"<<errCode<<std::endl;
		LOG(ERROR)<<"��ý���ļ� "<<media_path_.c_str()<<" ʧ�ܣ������룺"<<errCode;

		return;
	}

	errCode = avformat_find_stream_info(input_context, NULL);
	if (errCode < 0)
	{
		std::cout<<"��ѯý���ļ� "<<media_path_.c_str()<<" ý����Ϣʧ�ܣ������룺"<<errCode<<std::endl;
		LOG(ERROR)<<"��ѯý���ļ� "<<media_path_.c_str()<<" ý����Ϣʧ�ܣ������룺"<<errCode;

		avformat_close_input(&input_context);
		return ;
	}

	std::cout<<"======= ý����Ϣ ======="<<std::endl;
	int video_stream_index = 0;
	int number_of_streams = input_context->nb_streams;
	for (int index = 0; index < number_of_streams; ++index)
	{
		AVStream *input_stream = input_context->streams[index];
		if (input_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			// �ҵ�����Ƶ���������Ƶ��������
			video_stream_index = index;

			// �ҳ�������
			AVCodec *av_codec = avcodec_find_decoder(input_stream->codec->codec_id);
			if (!av_codec)
			{
				// û���ҵ�������
				std::cout<<"û���ҵ���Ƶ��������"<<std::endl;
				LOG(ERROR)<<"û���ҵ���Ƶ��������";

				avformat_close_input(&input_context);
				return ;
			}

			// ���Դ򿪽�����
			errCode = avcodec_open2(input_stream->codec, av_codec, NULL);
			if (errCode < 0)
			{
				// �򿪽�����ʧ��
				std::cout<<"�򿪽�����ʧ�ܣ�"<<std::endl;
				LOG(ERROR)<<"�򿪽�����ʧ�ܣ�";

				avformat_close_input(&input_context);
				return ;
			}

			break;
		}
	}

	// �ҵ���һ���ؼ�֡���н���
	AVStream *input_stream = input_context->streams[video_stream_index];
	AVFrame *pFrame = av_frame_alloc();
	AVPacket av_packet;
	while (av_read_frame(input_context, &av_packet) >= 0)
	{
		if (av_packet.stream_index == video_stream_index)
		{
			AVCodecContext *avcodec_context = input_stream->codec;
			int got_frame = 0;
			avcodec_decode_video2(avcodec_context, pFrame, &got_frame, &av_packet);

			if (got_frame)
			{
				if (pFrame->key_frame)
				{
					// ���ǹؼ�֡������һ��������׼����ͼƬ��
					int number_bytes = avpicture_get_size(AV_PIX_FMT_BGR24, avcodec_context->width, avcodec_context->height);
					uint8_t *buffer = new(std::nothrow) uint8_t[number_bytes];

					AVFrame* av_frame_RGB = av_frame_alloc();
					avpicture_fill((AVPicture *)av_frame_RGB, buffer, AV_PIX_FMT_BGR24, avcodec_context->width, avcodec_context->height);

					SwsContext *sws_context = sws_getContext(avcodec_context->width, avcodec_context->height, avcodec_context->pix_fmt, avcodec_context->width, avcodec_context->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, nullptr, nullptr, nullptr);

					sws_scale(sws_context, pFrame->data, pFrame->linesize, 0, avcodec_context->height, av_frame_RGB->data, av_frame_RGB->linesize);

					// �����֡�浽����
					std::string thumbnail_path = media_path_.append(".bmp");
					
					BITMAPFILEHEADER bmp_header;
					BITMAPINFO bmp_info;
				}
			}
		}
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	base::AtExitManager _at_exit_manager;
	base::MessageLoop _message_loop;

#ifdef WIN32
	LPCTSTR commandline = GetCommandLine();
#endif
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, NULL);

	base::FilePath current_dir;
	bool result = file_util::GetCurrentDirectory(&current_dir);
	base::FilePath logging_file = current_dir.Append(_T("log"));

	// ���������Ŀ¼���򴴽�֮
	if (!file_util::PathExists(logging_file))
		result = file_util::CreateDirectoryW(logging_file);

	// �õ����������
	base::Time today = base::Time::Now();

	logging_file = logging_file.Append(_T("log.txt"));

	// ��ʼ����־
	result = logging::InitLogging(logging_file.LossyDisplayName().c_str(), 
		logging::LOG_ONLY_TO_FILE, 
		logging::DONT_LOCK_LOG_FILE, 
		logging::APPEND_TO_OLD_LOG_FILE, 
		logging::DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS);

	logging::SetMinLogLevel(logging::LOG_INFO);

	av_register_all();
	avformat_network_init();

	while (true)
	{
		std::cout<<"========FFmepgѧϰ�̳�========"<<std::endl;
		std::cout<<"1. ö�ٵ�ǰ֧�ֵ����������ʽ"<<std::endl;
		std::cout<<"2. ��ѯ����Ƶ�ļ�������Ϣ"<<std::endl;
		std::cout<<"3. ����Ƶ�ļ���ʽת��(���ı����)"<<std::endl;
		std::cout<<"4. ����Ƶ�ļ�����ת�������ı��ʽ��"<<std::endl;
		std::cout<<"5. ����Ƶ�ļ���ʽת��(�ı����)"<<std::endl;
		std::cout<<"6. ����Ƶ�ļ�����ת�����ı��ʽ��"<<std::endl;
		std::cout<<"7. ������Ƶ��ʱ��"<<std::endl;
		std::cout<<"8. ��ȡ��Ƶ����ͼ"<<std::endl;
		std::cout<<"9. "<<std::endl;
		std::cout<<"0. �˳�"<<std::endl;
		std::cout<<"������ѡ�";

		char input_data[512] = {0};
		std::cin.clear();
		std::cin>>input_data;

		if (_stricmp(input_data, "1") == 0){
			EnumInputOutputFormat();
		} else if (_stricmp(input_data, "2") == 0){
			std::cout<<"��������Ƶ�ļ�·����";
			char media_path[512] = {0};
			std::cin.clear();
			std::cin>>media_path;

			std::string path = media_path;
			GetMediaInfo(path);
		} else if (_stricmp(input_data, "3") == 0){
			std::cout<<"��������Ƶ�ļ�·����";
			char media_path[512] = {0};
			std::cin.clear();
			std::cin>>media_path;

			std::string path = media_path;
			GetMediaInfo(path);

			std::cout<<"������ת��Ŀ���ʽ��";
			char format_name[512] = {0};
			std::cin.clear();
			std::cin>>format_name;

			std::string ext = format_name;
			ConvertMediaFormat(path, ext);
		} else if (_stricmp(input_data, "4") == 0){
			std::cout<<"��������Ƶ�ļ�·����";
			char media_path[512] = {0};
			std::cin.clear();
			std::cin>>media_path;

			std::string path = media_path;
			GetMediaInfo(path);

			std::cout<<"������ת��Ŀ����룺";
			char codec_name[512] = {0};
			std::cin.clear();
			std::cin>>codec_name;

			std::string codec = codec_name;
			ConvertMediaCodec(path, codec);
		} else if (_stricmp(input_data, "5") == 0){
		} else if (_stricmp(input_data, "6") == 0){
		} else if (_stricmp(input_data, "7") == 0){
		} else if (_stricmp(input_data, "8") == 0){
			std::cout<<"��������Ƶ�ļ�·����";
			char media_path[512] = {0};
			std::cin.clear();
			std::cin>>media_path;

			std::string path = media_path;
			GetMediaInfo(path);
		} else if (_stricmp(input_data, "9") == 0){
		} else if (_stricmp(input_data, "0") == 0){
			break;
		}

		std::cout<<"\n\n"<<std::endl;
	}
	

	return 0;
}

