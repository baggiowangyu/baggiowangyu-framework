#include "stdafx.h"

#include "base/bind.h"
#include "base/location.h"
#include "base/synchronization/waitable_event.h"

#include "SDL2PlayerEx.h"

#include <iostream>

// 分屏播放自定义事件
#define REFRESH_EVENT	(SDL_USEREVENT + 1)
#define BREAK_EVENT		(SDL_USEREVENT + 2)

SDL2PlayerSubScreen::SDL2PlayerSubScreen()
	: sdl_texture_(nullptr)
	, sdl_refresh_thread_(nullptr)
	, frame_(nullptr)
	, frame_yuv_(nullptr)
	, out_buffer_(nullptr)
	, img_convert_ctx_(nullptr)
	, decoder_v3_(new bgMediaDecoderV3(this))
	, main_working_thread_(nullptr)
	, decoder_state_(StandBy)
	, player_working_state_(SubScreen_Free)
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
	// 这里得到的是视音频解码帧
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

	media_video_info_ = video_info;
}

void SDL2PlayerSubScreen::AudioInfoNotify()
{

}

int SDL2PlayerSubScreen::Init(int player_screen_width, int player_screen_height, SDL_Renderer *renderer)
{
	int errCode = 0;

	av_register_all();
	avformat_network_init();
	avcodec_register_all();

	player_screen_width_ = player_screen_width;
	player_screen_height_ = player_screen_height;
	sdl_renderer_ = renderer;

	return errCode;
}

void SDL2PlayerSubScreen::Close()
{

}

int SDL2PlayerSubScreen::Play(const char *url)
{
	int errCode = 0;

	//// 初始化播放环境
	//errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	//if (errCode != 0)
	//	return errCode;

	sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, player_screen_width_, player_screen_height_);

	// 开始解码
	errCode = decoder_v3_->OpenMedia(url);
	if (errCode != 0)
		return errCode;

	// 等待1秒钟
	base::TimeDelta time_delta = base::TimeDelta::FromSeconds(1);
	base::WaitableEvent wait_event(false, false);
	wait_event.TimedWait(time_delta);

	frame_ = av_frame_alloc();
	frame_yuv_ = av_frame_alloc();
	out_buffer_ = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  player_screen_width_, player_screen_height_, 1));
	av_image_fill_arrays(frame_yuv_->data, frame_yuv_->linesize, out_buffer_, AV_PIX_FMT_YUV420P, player_screen_width_, player_screen_height_, 1);

	img_convert_ctx_ = sws_getContext(media_video_info_.codec_width_, media_video_info_.codec_height_,
		media_video_info_.pixel_format_, player_screen_width_, player_screen_height_, media_video_info_.pixel_format_,
		SWS_BICUBIC, nullptr, nullptr, nullptr);

	// 这里启动线程
	main_working_thread_->Start();
	main_working_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&SDL2PlayerSubScreen::MainWorkingTask, this));

	// 这里，将播放线程调整到主线程，这样就可以接受窗口消息，改变播放窗口的尺寸了
	//MainWorkingTask(this);

	return errCode;
}

int SDL2PlayerSubScreen::Pause()
{
	int errCode = 0;

	// 如果是实时视频，那么暂停就是停止解码，清空视音频帧队列，暂停渲染
	// 如果是录像视频，那么暂停就是停止解码，暂停渲染

	return errCode;
}

int SDL2PlayerSubScreen::Resume()
{
	int errCode = 0;

	// 如果是实时视频恢复，那么就是重新从最新的流数据进行解码播放
	// 如果是录像视频恢复，那么就是继续解码，继续渲染

	return errCode;
}

int SDL2PlayerSubScreen::Stop()
{
	int errCode = 0;

	// 停止，停止解码，停止渲染，清空帧队列

	return errCode;
}

void SDL2PlayerSubScreen::MainWorkingTask(SDL2PlayerSubScreen *sub_screen)
{
	sub_screen->sdl_refresh_thread_ = SDL_CreateThread(SDL2Player::refresh_video, nullptr, player);

	bool is_stopped = false;
	while (true)
	{
		SDL_WaitEvent(&sub_screen->sub_screen_event_);

		switch (sub_screen->sub_screen_event_.type)
		{
		case REFRESH_EVENT:
			{
				// 这里需要检查解码器的工作状态，就绪....准备解码....解码中....解码结束....
				// 如果真队列为空，只要解码器状态不是就绪或者解码结束，那么我们就直接跳出，如果是，则标记为结束
				if ((player->video_list_.head() == player->video_list_.end()) && (player->video_list_.tail() == player->video_list_.end()))
				{
					if ((player->decoder_state_ == StandBy) || (player->decoder_state_ == DecodeFinished) || (player->decoder_state_ == DecodeError))
						player->thread_exit_ = 1;

					break;
				}

				// 从视频帧链表中取出一帧，转换图像后渲染显示
				base::LinkNode<FrameNode> *node = player->video_list_.head();
				FrameNode *frame_node = node->value();

				AVFrame *current_frame = frame_node->frame_;
				node->RemoveFromList();

				//AVFrame *current_frame = player->video_list_2_.front();
				//player->video_list_2_.pop();

				// 转换图像，图像转换结果总是最后两帧？
				sws_scale(player->img_convert_ctx_, (const unsigned char * const*)current_frame->data, current_frame->linesize, 
					0, current_frame->height, player->frame_yuv_->data, player->frame_yuv_->linesize);

				// SDL渲染纹理
				SDL_UpdateTexture(player->sdl_texture_, nullptr, player->frame_yuv_->data[0], player->frame_yuv_->linesize[0]);
				SDL_RenderClear(player->sdl_renderer_);
				SDL_RenderCopy(player->sdl_renderer_, player->sdl_texture_, nullptr, &player->sub_screen_rect_);
				SDL_RenderPresent(player->sdl_renderer_);
				// 渲染结束

				// 释放掉当前使用过的帧节点
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

	// 播放完成，后续应该干些什么，后面有需要再加吧
}

void SDL2PlayerSubScreen::refresh_video(void *opaque)
{
	SDL2PlayerSubScreen *player = (SDL2PlayerSubScreen *)opaque;
	player->thread_exit_ = 0;

	// 计算帧率
	int delay_count = 1000 / (player->media_video_info_.frame_rate_.num / player->media_video_info_.frame_rate_.den);

	while (!player->thread_exit_)
	{
		// 由于这里采用了同一个消息通道，不同的线程向各自的子窗口发送事件
		SDL_Event event;
		event.type = REFRESH_EVENT;
		SDL_PushEvent(&event);

		// 这里应当按照帧率发送
		// 我们在这里默认使用40ms等待，其实就是1000ms / 40ms = 25帧
		// 即当前我们将帧率固定在25
		SDL_Delay(delay_count);
	}

	player->thread_exit_ = 0;

	SDL_Event event;
	event.type = BREAK_EVENT;
	SDL_PushEvent(&event);
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
	
	// 这里依次开始计算每一个分屏的渲染区域了
	if (sub_screen_count_ == 1)
	{
		// 这里是单屏幕，跟窗口一样大即可
		sub_screens_[0].sub_screen_rect_.x = 0;
		sub_screens_[0].sub_screen_rect_.y = 0;
		sub_screens_[0].sub_screen_rect_.w = screen_width_;
		sub_screens_[0].sub_screen_rect_.h = screen_height_;

		sub_screens_[0].Init(sdl_renderer_);
	}
	else if (sub_screen_count_ == 8)
	{
		// 8屏比较特殊，第一屏是一个相对大的屏，另外7屏围绕着它
	}
	else
	{
		// 剩余的都是矩阵屏，根据矩阵划分区域

		// 首先，将分屏数开方，得到水平列和垂直列的数量
		int count = sqrt((double)sub_screen_count_);

		int unit_width = screen_width_ / count;
		int unit_height = screen_height_ / count;

		for (int index = 0; index < sub_screen_count_; ++index)
		{
			int y_index = 0;
			for (int x_index = 0; x_index < count; ++x_index)
			{
				// 根据自己的索引计算矩形区域
				sub_screens_[index].sub_screen_rect_.x = (x_index + 1) * unit_width;
				sub_screens_[index].sub_screen_rect_.y = y_index * unit_height;
				sub_screens_[index].sub_screen_rect_.w = (x_index + 1) * unit_width;
				sub_screens_[index].sub_screen_rect_.h = (y_index + 1) * unit_height;

				sub_screens_[index].Init(sdl_renderer_);
			}
		}
	}

	return errCode;
}

void SDL2PlayerEx::Close()
{
	// 首先停止所有模块的播放，然后清理
	for (int index = 0; index < 64; ++index)
	{
		sub_screens_[index].Stop();
		sub_screens_[index].Close();
	}
}

int SDL2PlayerEx::GetFreeSubScreenIndex()
{
	for (int index = 0; index < 64; ++index)
	{
		enum SUBSCREEN_WORKING_STATE state = sub_screens_[index].GetWorkingState();

		if (state == SubScreen_Free)
			return index;
	}

	return -1;
}

int SDL2PlayerEx::PlayOnSubScreen(const char *url, int sub_screen_index /* = -1 */)
{
	int errCode = 0;

	int target_subscreen_index = -1;

	if (sub_screen_index == -1)
	{
		// 查找一个空闲的子窗口
		int free_subscreen_index = GetFreeSubScreenIndex();

		if ((free_subscreen_index + 1) > sub_screen_count_)
			return -1;

		target_subscreen_index = free_subscreen_index;
		
	}
	else
	{
		enum SUBSCREEN_WORKING_STATE state = sub_screens_[sub_screen_index].GetWorkingState();
		if (state != SubScreen_Free)
			return -2;

		target_subscreen_index = sub_screen_index;
	}

	// 找到子窗口，播放
	errCode = sub_screens_[target_subscreen_index].Play(url);

	return errCode;
}

int SDL2PlayerEx::PauseOnSubScreen(int sub_screen_index)
{
	int errCode = 0;

	if (sub_screen_index + 1 > sub_screen_count_)
		return -1;
	else
		errCode = sub_screens_[sub_screen_index].Pause();

	return errCode;
}

int SDL2PlayerEx::ResumeOnSubScreen(int sub_screen_index)
{
	int errCode = 0;

	if (sub_screen_index + 1 > sub_screen_count_)
		return -1;
	else
		errCode = sub_screens_[sub_screen_index].Resume();

	return errCode;
}

int SDL2PlayerEx::StopOnSubScreen(int sub_screen_index)
{
	int errCode = 0;

	if (sub_screen_index + 1 > sub_screen_count_)
		return -1;
	else
		errCode = sub_screens_[sub_screen_index].Stop();

	return errCode;
}

int SDL2PlayerEx::StopAll()
{
	int errCode = 0;

	for (int index = 0; index < sub_screen_count_; ++index)
	{
		errCode = sub_screens_[index].Stop();
		// 后期这里可以补上日志
	}

	return errCode;
}