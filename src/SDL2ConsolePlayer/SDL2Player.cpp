#include "stdafx.h"
#include "SDL2Player.h"


#include "base/bind.h"
#include "base/location.h"
#include "base/synchronization/waitable_event.h"


#include <iostream>

#define sub_screen_width	848
#define sub_screen_height	640

SDL2Player::SDL2Player()
	: screen_width_(848)
	, screen_height_(640)
	, decoder_v3_(new bgMediaDecoderV3(this))
	, frame_rate_(25)
	, main_working_thread_(new base::Thread("main_working_thread"))
{
}

SDL2Player::~SDL2Player()
{
}

void SDL2Player::StateNotify(std::string statestr, enum _Decoder_State_ stateindex)
{
	decoder_state_ = stateindex;
	std::cout<<"current decoder state is : "<<stateindex<<std::endl;
}

void SDL2Player::ErrorNotify(std::string errstr, int errcode)
{
	std::cout<<errstr<<" errcode : "<<errcode<<std::endl;
}

void SDL2Player::DecodeNotify(AVFrame *frame_data, int frame_type)
{
	// ����õ���������Ƶ����֡
	switch (frame_type)
	{
	case AVMEDIA_TYPE_VIDEO:
		{
			FrameNode *node = new FrameNode(frame_data);
			video_list_.Append(node);

			//video_list_2_.push(frame_data);
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

void SDL2Player::VideoInfoNotify(MediaVideoInfo video_info)
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

void SDL2Player::AudioInfoNotify(MediaAudioInfo audio_info)
{

}

int SDL2Player::Play(const char *url)
{
	int errCode = 0;

	// ��ʼ�����Ż���
	errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (errCode != 0)
		return errCode;

	// ������Ҫע��һ�£��ڴ�����Ⱦ��ʱ���������ڴ�С��ͬ����Ȼ�ᱯ��
	sdl_window_ = SDL_CreateWindow("123", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width_, screen_height_, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_POPUP_MENU);
	sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, 0);
	sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, screen_width_, screen_height_);

	// �������ڸ�ʵ�ʴ���չʾ�õľ�������
	sub_screen_rect_.x = 0;
	sub_screen_rect_.y = 0;
	sub_screen_rect_.w = sub_screen_width;
	sub_screen_rect_.h = sub_screen_height;

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

	// ע�⣬��������ʱ�򣬺����width��height�Ǳ�ʾ��������������ϵ�����ֵ
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

void SDL2Player::MainWorkingTask(SDL2Player *player)
{
	player->sdl_refresh_thread_ = SDL_CreateThread(SDL2Player::refresh_video, nullptr, player);
	SDL_Event event;

	bool is_stopped = false;
	while (true)
	{
		SDL_WaitEvent(&event);

		switch (event.type)
		{
		case REFRESH_EVENT:
			{
				// ������Ҫ���������Ĺ���״̬������....׼������....������....�������....
				// ��������Ϊ�գ�ֻҪ������״̬���Ǿ������߽����������ô���Ǿ�ֱ������������ǣ�����Ϊ����
				if ((player->video_list_.head() == player->video_list_.end()) && (player->video_list_.tail() == player->video_list_.end()))
				{
					if ((player->decoder_state_ == StandBy) || (player->decoder_state_ == DecodeFinished) || (player->decoder_state_ == DecodeError))
						player->thread_exit_ = 1;

					break;
				}

				// ����Ƶ֡������ȡ��һ֡��ת��ͼ�����Ⱦ��ʾ
				base::LinkNode<FrameNode> *node = player->video_list_.head();
				FrameNode *frame_node = node->value();
				
				AVFrame *current_frame = frame_node->frame_;
				node->RemoveFromList();

				//AVFrame *current_frame = player->video_list_2_.front();
				//player->video_list_2_.pop();
				
				// ת��ͼ��ͼ��ת��������������֡��
				sws_scale(player->img_convert_ctx_, (const unsigned char * const*)current_frame->data, current_frame->linesize, 
					0, current_frame->height, player->frame_yuv_->data, player->frame_yuv_->linesize);

				// SDL��Ⱦ����
				SDL_UpdateTexture(player->sdl_texture_, nullptr, player->frame_yuv_->data[0], player->frame_yuv_->linesize[0]);
				SDL_RenderClear(player->sdl_renderer_);
				SDL_RenderCopy(player->sdl_renderer_, player->sdl_texture_, nullptr, &player->sub_screen_rect_);
				SDL_RenderPresent(player->sdl_renderer_);
				// ��Ⱦ����

				// �ͷŵ���ǰʹ�ù���֡�ڵ�
				delete frame_node;

				break;
			}
		case BREAK_EVENT:
			{
				is_stopped = true;
				break;
			}
		case SDL_WINDOWEVENT:
			{
				SDL_GetWindowSize(player->sdl_window_, &player->screen_width_, &player->screen_height_);
				break;
			}
		case SDL_QUIT:
			{
				player->thread_exit_ = 1;
				break;
			}
		}

		if (is_stopped)
		{
			SDL_Quit();
			break;
		}
	}

	// ������ɣ�����Ӧ�ø�Щʲô����������Ҫ�ټӰ�
}

// ����һ�������̣߳����ڿ��Ʋ���������֡�ʽ��벥��
// ͬʱ���ڱ�Ǳ�Ϊ�������򲥷������ͽ�������
int SDL2Player::refresh_video(void *opaque)
{
	SDL2Player *player = (SDL2Player *)opaque;
	player->thread_exit_ = 0;

	// ����֡��
	int delay_count = 1000 / (player->media_video_info_.frame_rate_.num / player->media_video_info_.frame_rate_.den);

	while (!player->thread_exit_)
	{
		SDL_Event event;
		event.type = REFRESH_EVENT;
		SDL_PushEvent(&event);

		// ����Ӧ������֡�ʷ���
		// ����������Ĭ��ʹ��40ms�ȴ�����ʵ����1000ms / 40ms = 25֡
		// ����ǰ���ǽ�֡�ʹ̶���25
		SDL_Delay(40);
	}

	player->thread_exit_ = 0;

	SDL_Event event;
	event.type = BREAK_EVENT;
	SDL_PushEvent(&event);

	return 0;
}