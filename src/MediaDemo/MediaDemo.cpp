// MediaDemo.cpp : 定义控制台应用程序的入口点。
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
	// 寻找第一个支持的输入格式
	std::cout<<"枚举所有支持的输入格式"<<std::endl;
	LOG(INFO)<<"枚举所有支持的输入格式";
	AVInputFormat *input_format = av_iformat_next(nullptr);

	while(input_format != nullptr)
	{
		// 输出相关信息
		std::cout<<"AVInputFormat::name : "<<input_format->name<<std::endl;
		LOG(INFO)<<"AVInputFormat::name : "<<input_format->name;
		std::cout<<"AVInputFormat::long_name : "<<input_format->long_name<<std::endl;
		LOG(INFO)<<"AVInputFormat::long_name : "<<input_format->long_name;
		std::cout<<"-----------------------------------"<<std::endl;
		LOG(INFO)<<"-----------------------------------";

		// 寻找下一个支持的输入格式
		input_format = av_iformat_next(input_format);
	}

	std::cout<<"枚举所有支持的输出格式"<<std::endl;
	LOG(INFO)<<"枚举所有支持的输出格式";
	AVOutputFormat *output_format = av_oformat_next(nullptr);

	while(output_format != nullptr)
	{
		// 输出相关信息
		std::cout<<"AVOutputFormat::name : "<<output_format->name<<std::endl;
		LOG(INFO)<<"AVOutputFormat::name : "<<output_format->name;
		std::cout<<"AVOutputFormat::long_name : "<<output_format->long_name<<std::endl;
		LOG(INFO)<<"AVOutputFormat::long_name : "<<output_format->long_name;
		std::cout<<"-----------------------------------"<<std::endl;
		LOG(INFO)<<"-----------------------------------";

		// 寻找下一个支持的输入格式
		output_format = av_oformat_next(output_format);
	}
}

void GetMediaInfo(std::string media_path_)
{
	// 分析文件
	MediaAnalyze media_analyze;
	int errCode = media_analyze.Open(media_path_.c_str());
	if (errCode != 0)
	{
		std::cout<<"分析媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"分析媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode;
		return ;
	}

	std::cout<<"媒体概要：";
	std::cout<<"\n\t"<<"格式名："<<media_analyze.GetFormatName();
	std::cout<<"\n\t"<<"格式全名："<<media_analyze.GetFormatLongName();
	std::cout<<"\n\t"<<"持续时间(秒)："<<media_analyze.GetKeepDuration();
	std::cout<<"\n\t"<<"平均混合码率："<<media_analyze.GetMixBitRate();

	// 分析完成，输出信息
	if (media_analyze.HasVideoInfo())
	{
		std::cout<<"\n  视频流数据 ：";
		std::cout<<"\n\t"<<"编码ID："<<media_analyze.GetVideoCodecId();
		std::cout<<"\n\t"<<"编码名称："<<media_analyze.GetVideoCodecName();
		std::cout<<"\n\t"<<"编码全名："<<media_analyze.GetVideoCodecLongName();
		std::cout<<"\n\t"<<"格式概况：";
		std::cout<<"\n\t"<<"格式设置，CABAC：";
		std::cout<<"\n\t"<<"格式设置，参考帧：";
		std::cout<<"\n\t"<<"格式设置，GOP：";
		std::cout<<"\n\t"<<"持续时间(秒)："<<media_analyze.GetVideoDuration();
		std::cout<<"\n\t"<<"码率：";
		std::cout<<"\n\t"<<"视频宽度："<<media_analyze.GetVideoWidth();
		std::cout<<"\n\t"<<"视频高度："<<media_analyze.GetVideoHeight();
		std::cout<<"\n\t"<<"画面比例：";
		std::cout<<"\n\t"<<"色彩空间："<<media_analyze.GetVideoPixFormatName();
		std::cout<<"\n\t"<<"色彩抽样：";
		std::cout<<"\n\t"<<"位深：";
		std::cout<<"\n\t"<<"扫描方式：";
	}

	if (media_analyze.HasAudioInfo())
	{
		std::cout<<"\n  音频流数据 ：";
		std::cout<<"\n\t"<<"编码ID："<<media_analyze.GetAudioCodecId();
		std::cout<<"\n\t"<<"格式："<<media_analyze.GetAudioCodecName();
		std::cout<<"\n\t"<<"格式版本：";
		std::cout<<"\n\t"<<"格式概况：";
		std::cout<<"\n\t"<<"模式：";
		std::cout<<"\n\t"<<"扩展模式：";
		std::cout<<"\n\t"<<"持续时间(秒)："<<media_analyze.GetAudioDuration();
		std::cout<<"\n\t"<<"码率模式：";
		std::cout<<"\n\t"<<"码率："<<media_analyze.GetAudioBitRate();
		std::cout<<"\n\t"<<"声道："<<media_analyze.GetAudioChannelCount();
		std::cout<<"\n\t"<<"采样率："<<media_analyze.GetAudioSanplingRate();
		std::cout<<"\n\t"<<"压缩模式：";
		std::cout<<"\n\t"<<"音频延迟：";
	}

	media_analyze.Close();
}

void ConvertMediaFormat(std::string media_path_, std::string target_format_)
{
	// 处理输入媒体文件
	AVFormatContext *input_context = nullptr;
	int errCode = avformat_open_input(&input_context, media_path_.c_str(), nullptr, nullptr);
	if (0 != errCode)
	{
		std::cout<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode;
		return;
	}

	errCode = avformat_find_stream_info(input_context, NULL);
	if (errCode < 0)
	{
		std::cout<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode;
		avformat_close_input(&input_context);
		return ;
	}

	// 处理输出媒体文件
	std::string media_path_output = media_path_.append(".").append(target_format_);
	AVFormatContext *output_context = nullptr;	
	avformat_alloc_output_context2(&output_context, nullptr, nullptr, media_path_output.c_str());
	if (!output_context)
	{
		std::cout<<"创建转换媒体文件 "<<media_path_output.c_str()<<" 失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"创建转换媒体文件 "<<media_path_output.c_str()<<" 失败！错误码："<<errCode;
		avformat_close_input(&input_context);
		return ;
	}

	std::cout<<"======= 媒体信息 ======="<<std::endl;
	int number_of_streams = input_context->nb_streams;
	for (int index = 0; index < number_of_streams; ++index)
	{
		AVStream *input_stream = input_context->streams[index];
		AVStream *output_stream = avformat_new_stream(output_context, input_stream->codec->codec);

		if (!output_stream)
		{
			std::cout<<"创建输出流对象失败！"<<std::endl;
			LOG(ERROR)<<"创建输出流对象失败！";

			avformat_close_input(&input_context);
			if (output_context && !(output_context->flags & AVFMT_NOFILE))
				avio_close(output_context->pb);
			avformat_free_context(output_context);
			return ;
		}

		errCode = avcodec_copy_context(output_stream->codec, input_stream->codec);
		if (errCode < 0)
		{
			std::cout<<"复制编码上下文失败！"<<std::endl;
			LOG(ERROR)<<"复制编码上下文失败！";
			
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

	// 打开输出文件
	if (!(output_context->flags & AVFMT_NOFILE))
	{
		errCode = avio_open(&output_context->pb, media_path_output.c_str(), AVIO_FLAG_WRITE);
		if (errCode < 0)
		{
			std::cout<<"打开输出文件 "<<media_path_output<<" 失败！"<<std::endl;
			LOG(ERROR)<<"打开输出文件 "<<media_path_output<<" 失败！";

			avformat_close_input(&input_context);
			if (output_context && !(output_context->flags & AVFMT_NOFILE))
				avio_close(output_context->pb);
			avformat_free_context(output_context);
			return ;
		}
	}

	// 写文件头
	errCode = avformat_write_header(output_context, nullptr);
	if (errCode < 0)
	{
		std::cout<<"输出文件，写文件头时出错！"<<std::endl;
		LOG(ERROR)<<"输出文件，写文件头时出错！";
		
		avformat_close_input(&input_context);
		if (output_context && !(output_context->flags & AVFMT_NOFILE))
			avio_close(output_context->pb);
		avformat_free_context(output_context);
		return ;
	}

	// 逐帧写入
	int frame_index = 0;
	while (true)
	{
		AVPacket av_packet;

		// 读取一个带编码的帧
		errCode = av_read_frame(input_context, &av_packet);
		if (errCode < 0)
			break;

		AVStream *input_stream = input_context->streams[av_packet.stream_index];
		AVStream *output_stream = output_context->streams[av_packet.stream_index];

		// 转换PTS/DTS
		// PTS: AVPacket中的显示时间戳
		// DTS: 解码时间戳
		av_packet.pts = av_rescale_q_rnd(av_packet.pts, input_stream->time_base, output_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		av_packet.dts = av_rescale_q_rnd(av_packet.dts, input_stream->time_base, output_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		av_packet.duration = av_rescale_q(av_packet.duration, input_stream->time_base, output_stream->time_base);
		av_packet.pos = -1;

		// 写入
		errCode = av_interleaved_write_frame(output_context, &av_packet);
		if (errCode < 0)
		{
			std::cout<<"包封装出错！"<<std::endl;
			LOG(ERROR)<<"包封装出错！";
			break;
		}

		av_free_packet(&av_packet);
		++frame_index;
	}

	// 写文件尾
	av_write_trailer(output_context);
	
	// 清理资源
	avformat_close_input(&input_context);
	if (output_context && !(output_context->flags & AVFMT_NOFILE))
		avio_close(output_context->pb);
	avformat_free_context(output_context);
	
	return ;
}

void ConvertMediaCodec(std::string media_path_, std::string codec_name)
{
	// 处理输入媒体文件
	AVFormatContext *input_context = nullptr;
	int errCode = avformat_open_input(&input_context, media_path_.c_str(), nullptr, nullptr);
	if (0 != errCode)
	{
		std::cout<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode;
		return;
	}

	errCode = avformat_find_stream_info(input_context, NULL);
	if (errCode < 0)
	{
		std::cout<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode;
		avformat_close_input(&input_context);
		return ;
	}

	// 处理输出媒体文件
	std::string media_path_output = media_path_.append(".").append("new");
	AVFormatContext *output_context = nullptr;	
	avformat_alloc_output_context2(&output_context, nullptr, nullptr, media_path_output.c_str());
	if (!output_context)
	{
		std::cout<<"创建转换媒体文件 "<<media_path_output.c_str()<<" 失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"创建转换媒体文件 "<<media_path_output.c_str()<<" 失败！错误码："<<errCode;
		avformat_close_input(&input_context);
		return ;
	}
}

// 将视频第一个关键帧保存下来，存成PNG或者JPG文件
void GetVideoThumbnail(std::string media_path_)
{
	// 处理输入媒体文件
	AVFormatContext *input_context = nullptr;
	int errCode = avformat_open_input(&input_context, media_path_.c_str(), nullptr, nullptr);
	if (0 != errCode)
	{
		std::cout<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode;

		return;
	}

	errCode = avformat_find_stream_info(input_context, NULL);
	if (errCode < 0)
	{
		std::cout<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode;

		avformat_close_input(&input_context);
		return ;
	}

	std::cout<<"======= 媒体信息 ======="<<std::endl;
	int video_stream_index = 0;
	int number_of_streams = input_context->nb_streams;
	for (int index = 0; index < number_of_streams; ++index)
	{
		AVStream *input_stream = input_context->streams[index];
		if (input_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			// 找到了视频流，标记视频流索引号
			video_stream_index = index;

			// 找出解码器
			AVCodec *av_codec = avcodec_find_decoder(input_stream->codec->codec_id);
			if (!av_codec)
			{
				// 没有找到解码器
				std::cout<<"没有找到视频解码器！"<<std::endl;
				LOG(ERROR)<<"没有找到视频解码器！";

				avformat_close_input(&input_context);
				return ;
			}

			// 尝试打开解码器
			errCode = avcodec_open2(input_stream->codec, av_codec, NULL);
			if (errCode < 0)
			{
				// 打开解码器失败
				std::cout<<"打开解码器失败！"<<std::endl;
				LOG(ERROR)<<"打开解码器失败！";

				avformat_close_input(&input_context);
				return ;
			}

			break;
		}
	}

	// 找到第一个关键帧进行解码
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
					// 这是关键帧，做下一步操作，准备搞图片了
					int number_bytes = avpicture_get_size(AV_PIX_FMT_BGR24, avcodec_context->width, avcodec_context->height);
					uint8_t *buffer = new(std::nothrow) uint8_t[number_bytes];

					AVFrame* av_frame_RGB = av_frame_alloc();
					avpicture_fill((AVPicture *)av_frame_RGB, buffer, AV_PIX_FMT_BGR24, avcodec_context->width, avcodec_context->height);

					SwsContext *sws_context = sws_getContext(avcodec_context->width, avcodec_context->height, avcodec_context->pix_fmt, avcodec_context->width, avcodec_context->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, nullptr, nullptr, nullptr);

					sws_scale(sws_context, pFrame->data, pFrame->linesize, 0, avcodec_context->height, av_frame_RGB->data, av_frame_RGB->linesize);

					// 最后将首帧存到磁盘
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

	// 不存在这个目录，则创建之
	if (!file_util::PathExists(logging_file))
		result = file_util::CreateDirectoryW(logging_file);

	// 得到今天的日期
	base::Time today = base::Time::Now();

	logging_file = logging_file.Append(_T("log.txt"));

	// 初始化日志
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
		std::cout<<"========FFmepg学习教程========"<<std::endl;
		std::cout<<"1. 枚举当前支持的输入输出格式"<<std::endl;
		std::cout<<"2. 查询视音频文件基本信息"<<std::endl;
		std::cout<<"3. 视音频文件格式转换(不改变编码)"<<std::endl;
		std::cout<<"4. 视音频文件编码转换（不改变格式）"<<std::endl;
		std::cout<<"5. 视音频文件格式转换(改变编码)"<<std::endl;
		std::cout<<"6. 视音频文件编码转换（改变格式）"<<std::endl;
		std::cout<<"7. 计算视频总时长"<<std::endl;
		std::cout<<"8. 提取视频缩略图"<<std::endl;
		std::cout<<"9. "<<std::endl;
		std::cout<<"0. 退出"<<std::endl;
		std::cout<<"请输入选项：";

		char input_data[512] = {0};
		std::cin.clear();
		std::cin>>input_data;

		if (_stricmp(input_data, "1") == 0){
			EnumInputOutputFormat();
		} else if (_stricmp(input_data, "2") == 0){
			std::cout<<"请输入视频文件路径：";
			char media_path[512] = {0};
			std::cin.clear();
			std::cin>>media_path;

			std::string path = media_path;
			GetMediaInfo(path);
		} else if (_stricmp(input_data, "3") == 0){
			std::cout<<"请输入视频文件路径：";
			char media_path[512] = {0};
			std::cin.clear();
			std::cin>>media_path;

			std::string path = media_path;
			GetMediaInfo(path);

			std::cout<<"请输入转换目标格式：";
			char format_name[512] = {0};
			std::cin.clear();
			std::cin>>format_name;

			std::string ext = format_name;
			ConvertMediaFormat(path, ext);
		} else if (_stricmp(input_data, "4") == 0){
			std::cout<<"请输入视频文件路径：";
			char media_path[512] = {0};
			std::cin.clear();
			std::cin>>media_path;

			std::string path = media_path;
			GetMediaInfo(path);

			std::cout<<"请输入转换目标编码：";
			char codec_name[512] = {0};
			std::cin.clear();
			std::cin>>codec_name;

			std::string codec = codec_name;
			ConvertMediaCodec(path, codec);
		} else if (_stricmp(input_data, "5") == 0){
		} else if (_stricmp(input_data, "6") == 0){
		} else if (_stricmp(input_data, "7") == 0){
		} else if (_stricmp(input_data, "8") == 0){
			std::cout<<"请输入视频文件路径：";
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

