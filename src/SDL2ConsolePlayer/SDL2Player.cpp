#include "stdafx.h"
#include "SDL2Player.h"

#include <iostream>

SDL2Player::SDL2Player()
	: screen_width_(800)
	, screen_height_(600)
	, decoder_v3_(new bgMediaDecoderV3(this))
{
}

SDL2Player::~SDL2Player()
{
}

void SDL2Player::StateNotify(std::string statestr, int stateindex)
{

}

void SDL2Player::ErrorNotify(std::string errstr, int errcode)
{
	std::cout<<errstr<<" errcode : "<<errcode<<std::endl;
}

void SDL2Player::DecodeNotify(AVFrame *frame_data, int frame_type)
{
	// 这里得到的是视音频解码帧
	switch (frame_type)
	{
	case AVMEDIA_TYPE_VIDEO:
		FrameNode *node = new FrameNode(frame_data);
		video_list_.Append(node);
		break;
	case AVMEDIA_TYPE_AUDIO:
		FrameNode *node = new FrameNode(frame_data);
		audio_list_.Append(node);
		break;
	default:
		break;
	}
}

void SDL2Player::VideoInfoNotify(MediaVideoInfo video_info)
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

void SDL2Player::AudioInfoNotify()
{

}

int SDL2Player::Play(const char *url)
{
	int errCode = 0;

	errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (errCode <= 0)
		return errCode;

	errCode = decoder_v3_->OpenMedia(url);
	if (errCode != 0)
		return errCode;

	frame_ = av_frame_alloc();
	frame_yuv_ = av_frame_alloc();
	out_buffer_ = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  screen_width_, screen_height_, 1));
	av_image_fill_arrays(frame_yuv_->data, frame_yuv_->linesize, out_buffer_, AV_PIX_FMT_YUV420P, screen_width_, screen_height_, 1);

	img_convert_ctx_ = sws_getContext(media_video_info_.codec_width_, media_video_info_.codec_height_, media_video_info_.pixel_format_, screen_width_, screen_height_, media_video_info_.pixel_format_, SWS_BICUBIC, nullptr, nullptr, nullptr);

	sdl_window_ = SDL_CreateWindow("123", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width_, screen_height_, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, 0);
	sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, screen_width_, screen_height_);

	sub_screen_rect_.x = 0;
	sub_screen_rect_.y = 0;
	sub_screen_rect_.w = screen_width_;
	sub_screen_rect_.h = screen_height_;

	// 这里启动线程

	return errCode;
}