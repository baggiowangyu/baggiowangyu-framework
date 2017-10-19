// SDL2ConsolePlayer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"

#include "bgMediaDecoder/bgMediaDecoder_V3.h"
#include "ext/SDL/include/SDL.h"

#include <iostream>

class DecodeNotifer : public bgMediaDecoderV3Notify
{
public:
	DecodeNotifer() : player_width_(800), player_height_(600)
	, sdl_window_(nullptr), sdl_renderer_(nullptr), sdl_texture_(nullptr)
	{
	}

	~DecodeNotifer() {}

public:
	virtual void StateNotify(std::string statestr, int stateindex)
	{
		if (stateindex == 0)
		{
			std::cout<<statestr<<std::endl;
			//MessageLoop::current()->Quit();
		}
	}

	virtual void ErrorNotify(std::string errstr, int errcode)
	{
		std::cout<<errstr<<" errcode : "<<errcode<<std::endl;
	}

	virtual void DecodeNotify(AVFrame *frame_data, int frame_type)
	{
		// 这里得到的是视音频解码帧
		switch (frame_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			break;
		case AVMEDIA_TYPE_AUDIO:
			break;
		default:
			break;
		}

		std::cout<<"pkt_size : "<<frame_data->pkt_size<<std::endl;
	}

	virtual void VideoInfoNotify(MediaVideoInfo video_info)
	{
		// 这里拿到的是视音频基础信息
		std::cout<<"pix_fmt : "<<av_get_pix_fmt_name(video_info.pixel_format_)<<std::endl;
		std::cout<<"width : "<<video_info.codec_width_<<std::endl;
		std::cout<<"height : "<<video_info.codec_height_<<std::endl;
		std::cout<<"bit_rate : "<<video_info.bit_rate_<<std::endl;
		std::cout<<"gop_size : "<<video_info.gop_size_<<std::endl;
		std::cout<<"time_base_(den/num) : "<<video_info.time_base_.den<<"/"<<video_info.time_base_.num<<std::endl;
		std::cout<<"qmin_ : "<<video_info.qmin_<<std::endl;
		std::cout<<"qmax_ : "<<video_info.qmax_<<std::endl;
		std::cout<<"max_b_frames_ : "<<video_info.max_b_frames_<<std::endl;

		// 这里开始初始化SDL2
		int errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
		sdl_window_ = SDL_CreateWindow("123", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, player_width_, player_height_, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
		sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, 0);
		sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, player_width_, player_height_);
	}

	virtual void AudioInfoNotify() {}

public:
	void UpdatePlayRect(int w, int h) 
	{
		player_width_ = w;
		player_height_ = h;

		// 释放掉旧窗口，创建新窗口
	}

public:
	int player_width_;
	int player_height_;
	SDL_Window *sdl_window_;
	SDL_Renderer *sdl_renderer_;
	SDL_Texture *sdl_texture_;
};

int _tmain(int argc, _TCHAR* argv[])
{
	// 初始化程序环境
	base::AtExitManager at_exit_manager;	// 程序退出管理器，必须在入口函数声明
	base::MessageLoop message_loop;			// 程序消息循环，必须在入口函数声明

	// 托管命令行参数
	LPTSTR commandline = GetCommandLine();
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, nullptr);

	// 接下来可以开展核心业务

	if (!cmdline.HasSwitch("url"))
	{
		std::cout<<"not find param \"--url=...\""<<std::endl;
		system("pause");
		return 0;
	}
	std::string url = cmdline.GetSwitchValueASCII("url");

	// 首先解码视频
	DecodeNotifer notifer;
	bgMediaDecoderV3 decoder_v3(&notifer);
	decoder_v3.OpenMedia(url.c_str());

	message_loop.Run();

	return 0;
}

