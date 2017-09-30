// bgAudioPlayer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/message_loop.h"
#include "base/command_line.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
#ifdef __cplusplus
};
#endif

#include "SDL.h"

#include <iostream>
#include <windows.h>

// 48kHz/32位音频 1秒
#define MAX_AUDIO_FRAME_SIZE	192000

static uint8_t *audio_buffer = nullptr;
static int audio_len = 0;
static long long audio_buffer_index = 0;

void SDLCALL _SDL_AudioCallback(void *userdata, Uint8 * stream,	int len)
{
	SDL_memset(stream, 0, len);

	if (audio_len == 0)
		return ;

	len = len > audio_len ? audio_len : len;

	SDL_MixAudio(stream, (uint8_t *)audio_buffer + audio_buffer_index, len, SDL_MIX_MAXVOLUME);

	audio_buffer_index += len;
	audio_len -= len;
	stream += len;
}

int _tmain(int argc, _TCHAR* argv[])
{
	base::AtExitManager at_exit_manager;
	base::MessageLoop message_loop;

	LPTSTR cmdline = GetCommandLine();
	CommandLine commandline = CommandLine::FromString(cmdline);
	commandline.Init(argc, nullptr);

	if (!commandline.HasSwitch("name"))
	{
		std::cout<<"file not found..."<<std::endl;
		system("pause");
		return 0;
	}

	std::string media_name = commandline.GetSwitchValueASCII("name");

	//////////////////////////////////////////////////////////////////////////

	av_register_all();

	int errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (errCode != 0)
	{
		std::cout<<"init SDL2 failed... errCode"<<errCode<<std::endl;
		system("pause");
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////

	AVFormatContext *input_format_context = nullptr;
	errCode = avformat_open_input(&input_format_context, media_name.c_str(), nullptr, nullptr);
	if (0 != errCode)
	{
		std::cout<<"open input file failed... errCode"<<errCode<<std::endl;
		system("pause");
		return 0;
	}

	errCode = avformat_find_stream_info(input_format_context, nullptr);
	if (errCode < 0)
	{
		std::cout<<"find input file stream info failed... errCode"<<errCode<<std::endl;
		system("pause");
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////

	int audio_stream_index = -1;

	for (int index = 0; index < input_format_context->nb_streams; ++index)
	{
		if (input_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
			audio_stream_index = index;
	}

	if (audio_stream_index == -1)
	{
		std::cout<<"audio stream not found ..."<<std::endl;
		system("pause");
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////

	AVCodecContext *input_avcodec_context = input_format_context->streams[audio_stream_index]->codec;
	AVCodec *input_avcodec = avcodec_find_decoder(input_avcodec_context->codec_id);
	if (!input_avcodec)
	{
		std::cout<<"audio codec not found ..."<<std::endl;
		system("pause");
		return 0;
	}

	uint8_t *audio_out_buffer = (uint8_t*)av_malloc(MAX_AUDIO_FRAME_SIZE);
	audio_buffer = audio_out_buffer;

	// 
	SDL_AudioSpec audio_spec;
	audio_spec.freq = input_avcodec_context->sample_rate;	// 采样率
	audio_spec.format = AUDIO_S16SYS;						// 样本格式
	audio_spec.channels = input_avcodec_context->channels;	// 声道：1-单声道，2-立体声
	audio_spec.silence = 0;									// 静音值
	audio_spec.samples = 1024;								// 输出样本数
	audio_spec.callback = _SDL_AudioCallback;				// 回调函数
	audio_spec.userdata = input_avcodec_context;			// 传入回调函数的用户数据

	// 打开音频设备
	errCode = SDL_OpenAudio(&audio_spec, nullptr);
	if (errCode < 0)
	{
		std::cout<<"open audio failed... errCode"<<errCode<<std::endl;
		system("pause");
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////

	AVFrame av_frame;
	AVFrame *audio_frame = av_frame_alloc();
	av_frame.format = AV_SAMPLE_FMT_S16;
	av_frame.sample_rate = audio_spec.freq;
	av_frame.channels = audio_spec.channels;
	av_frame.channel_layout = av_get_default_channel_layout(audio_spec.channels);

	errCode = avcodec_open2(input_avcodec_context, input_avcodec, nullptr);
	if (errCode < 0)
	{
		std::cout<<"open audio decoder failed... errCode"<<errCode<<std::endl;
		system("pause");
		return 0;
	}

	AVPacket *av_packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	av_init_packet(av_packet);

	SDL_PauseAudio(0);

	SwrContext *swr_context = nullptr;
	while (av_read_frame(input_format_context, av_packet) >= 0)
	{
		// 包所属的流索引
		if (av_packet->stream_index != audio_stream_index)
			continue;

		int got_frame = 0;
		errCode = avcodec_decode_audio4(input_avcodec_context, audio_frame, &got_frame, av_packet);
		if (errCode < 0)
			continue;

		int convert_len = 0;
		if (got_frame)
		{
			if (!swr_context)
			{
				swr_free(&swr_context);
				swr_context = nullptr;
			}

			swr_context = swr_alloc_set_opts(nullptr, av_frame.channel_layout, (AVSampleFormat)av_frame.format, av_frame.sample_rate,
				audio_frame->channel_layout, (AVSampleFormat)audio_frame->format, audio_frame->sample_rate, 0, nullptr);

			if (swr_context == nullptr)
			{
				errCode = swr_init(swr_context);
				if (errCode < 0)
					break;
			}

			convert_len = swr_convert(swr_context, NULL, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)audio_frame->data, audio_frame->nb_samples);
			
		}

		// 回到缓冲器头，继续播放数据
		audio_buffer_index = 0;
		audio_buffer = audio_out_buffer;

		// 通道数 * 转换长度 * 每个样本的长度
		audio_len = audio_frame->channels * convert_len * av_get_bytes_per_sample((AVSampleFormat)audio_frame->format);

		while (audio_len > 0)
			SDL_Delay(1);

		av_init_packet(av_packet);
	}

	return 0;
}

