#include "stdafx.h"

#include "base/bind.h"
#include "base/location.h"
#include "base/synchronization/waitable_event.h"
#include "base/rand_util.h"

#include "SDL2PlayerEx.h"

#include <iostream>

// 分屏播放自定义事件
#define REFRESH_EVENT	(SDL_USEREVENT + 1)
#define BREAK_EVENT		(SDL_USEREVENT + 2)

SDL2PlayerSubScreen::SDL2PlayerSubScreen()
	: player_screen_width_(800)
	, player_screen_height_(600)
	, sdl_window_(nullptr)
	, sdl_renderer_(nullptr)
	, sdl_texture_(nullptr)
	, sdl_refresh_thread_(nullptr)
	, sdl_refresh_control_thread_(nullptr)
	, sdl_control_thread_(nullptr)
	, play_event_(new base::WaitableEvent(false, false))
	, play_event_type_(SubScreen_Refresh)
	, thread_exit_(0)
	, frame_(nullptr)
	, frame_yuv_(nullptr)
	, out_buffer_(nullptr)
	, img_convert_ctx_(nullptr)
	, frame_rate_(25)
	, video_list_max_len_(150)
	, audio_list_max_len_(150)
	, video_list_current_len_(0)
	, audio_list_current_len_(0)
	, decoder_v3_(new bgMediaDecoderV3(this))
	, decoder_state_(StandBy)
	, sub_screen_working_state_(SubScreen_Free)
	, current_index_(-1)
	, video_info_notify_event_(new base::WaitableEvent(false, false))
	, audio_info_notify_event_(new base::WaitableEvent(false, false))
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
			int errCode = PushVideoFrame(frame_data);
			if (errCode != 0)
			{
			}
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

	video_info_notify_event_->Signal();
}

void SDL2PlayerSubScreen::AudioInfoNotify(MediaAudioInfo audio_info)
{
	std::cout<<"sample_rate : "<<audio_info.sample_rate_<<std::endl;
	std::cout<<"bit_rate : "<<audio_info.bit_rate_<<std::endl;
	std::cout<<"channels : "<<audio_info.channels_<<std::endl;
	std::cout<<"frame_size : "<<audio_info.frame_size_<<std::endl;
	std::cout<<"out_channels : "<<audio_info.out_channels_<<std::endl;

	media_audio_info_ = audio_info;

	audio_info_notify_event_->Signal();
}

int SDL2PlayerSubScreen::Init(int player_screen_width, int player_screen_height, SDL_Window *window, SDL_Renderer *renderer, int current_index)
{
	int errCode = 0;

	av_register_all();
	avformat_network_init();
	avcodec_register_all();

	player_screen_width_ = player_screen_width;
	player_screen_height_ = player_screen_height;

	sdl_window_ = window;
	sdl_renderer_ = renderer;
	
	current_index_ = current_index;

	// 根据索引，构建线程名，创建各个线程
	char thread_name[4096] = {0};
	base::snprintf(thread_name, 4096, "sdl_refresh_thread_%d", current_index_);
	sdl_refresh_thread_ = new base::Thread(thread_name);
	sdl_refresh_thread_->Start();

	base::snprintf(thread_name, 4096, "sdl_refresh_control_thread_%d", current_index_);
	sdl_refresh_control_thread_ = new base::Thread(thread_name);
	sdl_refresh_control_thread_->Start();

	base::snprintf(thread_name, 4096, "sdl_control_thread_%d", current_index_);
	sdl_control_thread_ = new base::Thread(thread_name);
	sdl_control_thread_->Start();

	return errCode;
}

void SDL2PlayerSubScreen::Close()
{

}

int SDL2PlayerSubScreen::Play(const char *url)
{
	int errCode = 0;

	sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, player_screen_width_, player_screen_height_);

	// 开始解码
	errCode = decoder_v3_->OpenMedia(url);
	if (errCode != 0)
		return errCode;

	// 这里可以判断，是否含有视音频流
	if (decoder_v3_->input_video_stream_index_ >= 0)
	{
		// 存在视频流, 做视频处理的一些初始化工作
		frame_ = av_frame_alloc();
		frame_yuv_ = av_frame_alloc();
		out_buffer_ = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  player_screen_width_, player_screen_height_, 1));
		av_image_fill_arrays(frame_yuv_->data, frame_yuv_->linesize, out_buffer_, AV_PIX_FMT_YUV420P, player_screen_width_, player_screen_height_, 1);


		// 在此之前要等第一波解码完成，否则会出问题
		video_info_notify_event_->Wait();
		img_convert_ctx_ = sws_getContext(media_video_info_.codec_width_, media_video_info_.codec_height_,
			media_video_info_.pixel_format_, player_screen_width_, player_screen_height_, media_video_info_.pixel_format_,
			SWS_BICUBIC, nullptr, nullptr, nullptr);

		// 这里启动线程，分别用于处理SDL的内部消息和我们用于控制播放的自定义消息
		sdl_refresh_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&SDL2PlayerSubScreen::SubScreenRefreshTask, this));

		base::TimeDelta time_delta = base::TimeDelta::FromMicroseconds(100);
		base::WaitableEvent wait_event(false, false);
		wait_event.TimedWait(time_delta);

		sdl_control_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&SDL2PlayerSubScreen::SubScreenControlTask, this));
		wait_event.TimedWait(time_delta);
	}

	if (decoder_v3_->input_audio_stream_index_ >= 0)
	{
		// 存在音频流，做音频处理的一些初始化工作
	}

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

enum SUBSCREEN_WORKING_STATE SDL2PlayerSubScreen::GetWorkingState()
{
	return sub_screen_working_state_;
}

void SDL2PlayerSubScreen::SubScreenRefreshTask(SDL2PlayerSubScreen *sub_screen)
{
	// 启动按帧率刷新控制线程
	sub_screen->sdl_refresh_control_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&SDL2PlayerSubScreen::SubScreenRefreshControlTask, sub_screen));

	base::TimeDelta time_delta = base::TimeDelta::FromMicroseconds(100);
	base::WaitableEvent wait_event(false, false);
	wait_event.TimedWait(time_delta);

	bool is_stopped = false;
	while (true)
	{
		sub_screen->play_event_->Wait();

		switch (sub_screen->play_event_type_)
		{
		case SubScreen_Refresh:
			{
				sub_screen->sub_screen_working_state_ = SubScreen_Playing;

				AVFrame *current_frame = sub_screen->PopVideoFrame();

				if (!current_frame)
					break;

				AVStream *video_stream = sub_screen->decoder_v3_->input_format_context_->streams[sub_screen->decoder_v3_->input_video_stream_index_];
				double current_play_time = current_frame->pts * av_q2d(video_stream->time_base);
				std::cout<<"current pts : "<<current_play_time<<std::endl;

				//AVFrame *current_frame = player->video_list_2_.front();
				//player->video_list_2_.pop();

				// 转换图像，图像转换结果总是最后两帧？
				sws_scale(sub_screen->img_convert_ctx_, (const unsigned char * const*)current_frame->data, current_frame->linesize, 
					0, current_frame->height, sub_screen->frame_yuv_->data, sub_screen->frame_yuv_->linesize);

				// SDL渲染纹理
				SDL_UpdateTexture(sub_screen->sdl_texture_, nullptr, sub_screen->frame_yuv_->data[0], sub_screen->frame_yuv_->linesize[0]);
				//SDL_RenderClear(sub_screen->sdl_renderer_);	// 这一句话会导致多屏渲染的时候相互干扰
				
				SDL_RenderCopy(sub_screen->sdl_renderer_, sub_screen->sdl_texture_, nullptr, &sub_screen->sub_screen_rect_);
				SDL_RenderPresent(sub_screen->sdl_renderer_);
				// 渲染结束

				// 释放掉当前使用过的帧节点
				delete frame_node;

				break;
			}
		case SubScreen_Break:
			{
				is_stopped = true;
				break;
			}
		}

		if (is_stopped)
		{
			sub_screen->sub_screen_working_state_ = SubScreen_Free;
			//SDL_Quit();
			break;
		}
	}

	// 播放完成，后续应该干些什么，后面有需要再加吧
}

void SDL2PlayerSubScreen::SubScreenControlTask(SDL2PlayerSubScreen *sub_screen)
{
	SDL_Event event;
	bool is_stopped = false;
	while (true)
	{
		SDL_WaitEvent(&event);

		//switch (event.type)
		//{
		//case SDL_WINDOWEVENT:
		//	{
		//		SDL_GetWindowSize(sub_screen->sdl_window_, &sub_screen->player_screen_width_, &sub_screen->player_screen_height_);
		//		break;
		//	}
		//case SDL_QUIT:
		//	{
		//		sub_screen->thread_exit_ = 1;
		//		break;
		//	}
		//}
	}
}

void SDL2PlayerSubScreen::SubScreenRefreshControlTask(SDL2PlayerSubScreen *sub_screen)
{
	sub_screen->thread_exit_ = 0;

	// 计算帧，帧率的计算方法跟time_base有关，这里一定要注意，不然会出大问题
	int delay_count = 1000 / (sub_screen->media_video_info_.frame_rate_.num / sub_screen->media_video_info_.frame_rate_.den);

	while (!sub_screen->thread_exit_)
	{
		// 由于这里采用了同一个消息通道，不同的线程向各自的子窗口发送事件
		sub_screen->play_event_type_ = SubScreen_Refresh;
		sub_screen->play_event_->Signal();

		// 这里应当按照帧率发送
		// 我们在这里默认使用40ms等待，其实就是1000ms / 40ms = 25帧
		// 即当前我们将帧率固定在25
		base::TimeDelta wait_time = base::TimeDelta::FromMilliseconds(40);
		base::WaitableEvent delay_event(false, false);
		delay_event.TimedWait(wait_time);

		//SDL_Delay(40);
	}

	sub_screen->thread_exit_ = 0;

	sub_screen->play_event_type_ = SubScreen_Break;
	sub_screen->play_event_->Signal();
}

int SDL2PlayerSubScreen::SetVideoFrameQueueLength(int len /* = 150 */)
{
	video_list_max_len_ = len;
	return 0;
}

int SDL2PlayerSubScreen::PushVideoFrame(AVFrame *frame)
{
	video_list_lock_.Acquire();

	// 首先检查当前队列中的帧数
	if (video_list_current_len_ == video_list_max_len_)
		return -1;
	
	FrameNode *node = new FrameNode(frame);
	video_list_.Append(node);
	++video_list_current_len_;

	video_list_lock_.Release();

	return 0;
}

AVFrame* SDL2PlayerSubScreen::PopVideoFrame()
{
	video_list_lock_.Acquire();

	// 这里需要检查解码器的工作状态，就绪....准备解码....解码中....解码结束....
	// 如果真队列为空，只要解码器状态不是就绪或者解码结束，那么我们就直接跳出，如果是，则标记为结束
	if ((video_list_.head() == video_list_.end()) && (video_list_.tail() == video_list_.end()))
	{
		if ((decoder_state_ == StandBy) || (decoder_state_ == DecodeFinished) || (decoder_state_ == DecodeError))
			thread_exit_ = 1;

		return nullptr;
	}

	// 从视频帧链表中取出一帧，转换图像后渲染显示
	base::LinkNode<FrameNode> *node = video_list_.head();
	FrameNode *frame_node = node->value();

	AVFrame *current_frame = frame_node->frame_;
	node->RemoveFromList();

	video_list_lock_.Release();

	return current_frame;
}

int SDL2PlayerSubScreen::SetAudioFrameQueueLength(int len /* = 150 */)
{
	audio_list_max_len_ = len;
	return 0;
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

	// 初始化播放环境
	errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (errCode != 0)
		return errCode;

	sdl_window_ = SDL_CreateWindow("SDL2PlayerEx", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width_, screen_height_, SDL_WINDOW_OPENGL);
	if (!sdl_window_)
	{
		return -1;
	}

	sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, 0);
	if (!sdl_renderer_)
	{
		return -2;
	}
	
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

		sub_screens_[0].Init(screen_width_, screen_height_, sdl_window_, sdl_renderer_, 0);
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

		int sub_screen_index = 0;
		int y_index = 0;
		while (true)
		{
			if (sub_screen_index == sub_screen_count_)
				break;
			
			for (int x_index = 0; x_index < count; ++x_index)
			{
				// 根据自己的索引计算矩形区域，坐标计算出错
				// 这里有一点要特别注意的，w和h分别是宽度和高度，不是D点的坐标.....我给自己挖了个坑
				sub_screens_[sub_screen_index].sub_screen_rect_.x = x_index * unit_width;
				sub_screens_[sub_screen_index].sub_screen_rect_.y = y_index * unit_height;
				sub_screens_[sub_screen_index].sub_screen_rect_.w = unit_width;
				sub_screens_[sub_screen_index].sub_screen_rect_.h = unit_height;

				//std::cout<<"sub_screen_"<<sub_screen_index<<" position : ("<<
				//	sub_screens_[sub_screen_index].sub_screen_rect_.x<<", "<<sub_screens_[sub_screen_index].sub_screen_rect_.y<<", "<<
				//	sub_screens_[sub_screen_index].sub_screen_rect_.w<<", "<<sub_screens_[sub_screen_index].sub_screen_rect_.h<<")"<<std::endl;

				sub_screens_[sub_screen_index].Init(screen_width_, screen_height_, sdl_window_, sdl_renderer_, sub_screen_index);
				++sub_screen_index;
			}

			++y_index;
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