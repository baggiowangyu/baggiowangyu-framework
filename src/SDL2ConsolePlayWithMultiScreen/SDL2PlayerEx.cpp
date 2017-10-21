#include "stdafx.h"

#include "SDL2PlayerEx.h"

#include <iostream>

SDL2PlayerSubScreen::SDL2PlayerSubScreen()
	: sdl_texture_(nullptr)
	, sdl_refresh_thread_(nullptr)
	, frame_(nullptr)
	, frame_yuv_(nullptr)
	, out_buffer_(nullptr)
	, img_convert_ctx_(nullptr)
	, decoder_v3_(new bgMediaDecoderV3(this))
{

}

SDL2PlayerSubScreen::~SDL2PlayerSubScreen()
{

}

void SDL2PlayerSubScreen::StateNotify(std::string statestr, enum _Decoder_State_ stateindex)
{
	decoder_state_ = stateindex;
	std::cout<<"current decoder state is : "<<stateindex<<std::endl;
}

void SDL2PlayerSubScreen::ErrorNotify(std::string errstr, int errcode)
{
	std::cout<<errstr<<" errcode : "<<errcode<<std::endl;
}

void SDL2PlayerSubScreen::DecodeNotify(AVFrame *frame_data, int frame_type)
{
	// ����õ���������Ƶ����֡
	switch (frame_type)
	{
	case AVMEDIA_TYPE_VIDEO:
		{
			FrameNode *node = new FrameNode(frame_data);
			video_list_.Append(node);
			break;
		}
	case AVMEDIA_TYPE_AUDIO:
		{
			FrameNode *node = new FrameNode(frame_data);
			audio_list_.Append(node);
			break;
		}
	default:
		break;
	}
}

void SDL2PlayerSubScreen::VideoInfoNotify(MediaVideoInfo video_info)
{
	// �����õ���������Ƶ������Ϣ
	std::cout<<"pix_fmt : "<<av_get_pix_fmt_name(video_info.pixel_format_)<<std::endl;
	std::cout<<"width : "<<video_info.codec_width_<<std::endl;
	std::cout<<"height : "<<video_info.codec_height_<<std::endl;
	std::cout<<"bit_rate : "<<video_info.bit_rate_<<std::endl;
	std::cout<<"gop_size : "<<video_info.gop_size_<<std::endl;
	std::cout<<"time_base_(den/num) : "<<video_info.time_base_.den<<"/"<<video_info.time_base_.num<<std::endl;
	std::cout<<"qmin_ : "<<video_info.qmin_<<std::endl;
	std::cout<<"qmax_ : "<<video_info.qmax_<<std::endl;
	std::cout<<"max_b_frames_ : "<<video_info.max_b_frames_<<std::endl;

	media_video_info_ = video_info;
}

void SDL2PlayerSubScreen::AudioInfoNotify()
{

}

int SDL2PlayerSubScreen::Init(SDL_Renderer *renderer)
{
	int errCode = 0;

	sdl_renderer_ = renderer;

	return errCode;
}

int SDL2PlayerSubScreen::Play(const char *url)
{
	int errCode = 0;

	// ��ʼ�����Ż���
	errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (errCode != 0)
		return errCode;

	sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, screen_width_, screen_height_);

	// ���������ʾ��
	sub_screen_rect_.x = 0;
	sub_screen_rect_.y = 0;
	sub_screen_rect_.w = screen_width_;
	sub_screen_rect_.h = screen_height_;

	// ��ʼ����
	errCode = decoder_v3_->OpenMedia(url);
	if (errCode != 0)
		return errCode;

	// �ȴ�1����
	base::TimeDelta time_delta = base::TimeDelta::FromSeconds(1);
	base::WaitableEvent wait_event(false, false);
	wait_event.TimedWait(time_delta);

	frame_ = av_frame_alloc();
	frame_yuv_ = av_frame_alloc();
	out_buffer_ = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  screen_width_, screen_height_, 1));
	av_image_fill_arrays(frame_yuv_->data, frame_yuv_->linesize, out_buffer_, AV_PIX_FMT_YUV420P, screen_width_, screen_height_, 1);

	img_convert_ctx_ = sws_getContext(media_video_info_.codec_width_, media_video_info_.codec_height_,
		media_video_info_.pixel_format_, screen_width_, screen_height_, media_video_info_.pixel_format_,
		SWS_BICUBIC, nullptr, nullptr, nullptr);

	// ���������߳�
	//main_working_thread_->Start();
	//main_working_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&SDL2Player::MainWorkingTask, this));

	// ����������̵߳��������̣߳������Ϳ��Խ��ܴ�����Ϣ���ı䲥�Ŵ��ڵĳߴ���
	MainWorkingTask(this);

	return errCode;
}

void SDL2PlayerSubScreen::MainWorkingTask(SDL2PlayerSubScreen *sub_screen)
{

}

void SDL2PlayerSubScreen::refresh_video(void *opaque)
{
	
}

// -----------------------------------------------------------------------------------------

SDL2PlayerEx::SDL2PlayerEx()
	: screen_width_(800)
	, screen_height_(600)
	, sdl_window_(nullptr)
	, sdl_renderer_(nullptr)
	, sub_screen_count_(1)
{

}

SDL2PlayerEx::~SDL2PlayerEx()
{

}

int SDL2PlayerEx::Init(int width /* = 800 */, int height /* = 600 */, enum SUBSCREEN_TYPE type /* = SubScreen_One */)
{
	int errCode = 0;

	screen_width_ = width;
	screen_height_ = height;
	
	switch (type)
	{
	case SubScreen_One:
		sub_screen_count_ = 1;
		break;
	case SubScreen_Four:
		sub_screen_count_ = 4;
		break;
	case SubScreen_Eight:
		sub_screen_count_ = 8;
		break;
	case SubScreen_Nine:
		sub_screen_count_ = 9;
		break;
	case SubScreen_Sixteen:
		sub_screen_count_ = 16;
		break;
	default:
		sub_screen_count_ = 1;
		break;
	}
	
	// �������ο�ʼ����ÿһ����������Ⱦ������
	if (sub_screen_count_ == 1)
	{
		// �����ǵ���Ļ��������һ���󼴿�
		sub_screens_[0].sub_screen_rect_.x = 0;
		sub_screens_[0].sub_screen_rect_.y = 0;
		sub_screens_[0].sub_screen_rect_.w = screen_width_;
		sub_screens_[0].sub_screen_rect_.h = screen_height_;

		sub_screens_[0].Init(sdl_renderer_);
	}
	else if (sub_screen_count_ == 8)
	{
		// 8���Ƚ����⣬��һ����һ����Դ����������7��Χ������
	}
	else
	{
		// ʣ��Ķ��Ǿ����������ݾ��󻮷�����

		// ���ȣ����������������õ��ֿ�����
		int count = sqrt((double)sub_screen_count_);

		int unit_width = screen_width_ / count;
		int unit_height = screen_height_ / count;

		for (int index = 0; index < sub_screen_count_; ++index)
		{
			// �����Լ������������������
			sub_screens_[index].sub_screen_rect_.x = index * unit_width;
			sub_screens_[index].sub_screen_rect_.y = index * unit_height;
			sub_screens_[index].sub_screen_rect_.w = (index + 1) * unit_width;
			sub_screens_[index].sub_screen_rect_.h = (index + 1) * unit_height;
		}
	}

	return errCode;
};