#ifndef _SDL2PLAYEREX_H_
#define _SDL2PLAYEREX_H_

#include "base/containers/linked_list.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread.h"

#include "bgMediaDecoder/bgMediaDecoder_V3.h"
#include "ext/SDL/include/SDL.h"

typedef enum SUBSCREEN_TYPE
{
	SubScreen_One		= 1,
	SubScreen_Four		= 4,
	SubScreen_Eight		= 8,
	SubScreen_Nine		= 9,
	SubScreen_Sixteen	= 16
};

typedef enum SUBSCREEN_WORKING_STATE
{
	SubScreen_Free,				// 空闲状态，播放完成后也进入空闲状态
	SubScreen_Playing,			// 播放状态，播放器正在渲染播放视频
	SubScreen_Pausing,			// 暂停状态，播放器暂停渲染播放
	SubScreen_PerFramePlaying	// 逐帧播放，播放器整处于逐帧播放状态
};

typedef enum SUBSCREEN_EVENT_TYPE
{
	SubScreen_Refresh,
	SubScreen_Break
};


// -----------------------------------------------------------------------------------
// 视音频帧节点，用于组件视音频帧队列
class FrameNode : public base::LinkNode<FrameNode>
{
public:
	explicit FrameNode(AVFrame *frame)
		: frame_(nullptr)
	{
		frame_ = av_frame_clone(frame);
	}

	~FrameNode()
	{
		av_frame_free(&frame_);
	}

public:
	AVFrame *frame_;
};


// -----------------------------------------------------------------------------------
// 子窗口类，是播放视音频的实体，负责视音频解码、帧图像压缩转换等工作
// 关于子窗口类的事件，我们可以设置除了一个子窗口之外，多子窗口环境下不处理任何内部消息
class SDL2PlayerSubScreen : public bgMediaDecoderV3Notify
{
public:
	SDL2PlayerSubScreen();
	~SDL2PlayerSubScreen();

public:
	// 解码器状态通知
	virtual void StateNotify(std::string statestr, enum _Decoder_State_ stateindex);

	// 解码器错误通知
	virtual void ErrorNotify(std::string errstr, int errcode);

	// 解码器解码通知
	virtual void DecodeNotify(AVFrame *frame_data, int frame_type);

	// 解码器视频信息通知
	virtual void VideoInfoNotify(MediaVideoInfo video_info);

	// 解码器音频信息通知
	virtual void AudioInfoNotify(MediaAudioInfo audio_info);

public:
	int Init(int player_screen_width, int player_screen_height, SDL_Window *window, SDL_Renderer *renderer, int current_index);
	void Close();

	int Play(const char *url);
	int Pause();
	int Resume();
	int Stop();

public:
	enum SUBSCREEN_WORKING_STATE GetWorkingState();

public:
	static void SubScreenRefreshTask(SDL2PlayerSubScreen *sub_screen);
	static void SubScreenControlTask(SDL2PlayerSubScreen *sub_screen);
	static void SubScreenRefreshControlTask(SDL2PlayerSubScreen *sub_screen);
	//static void refresh_video(void *opaque);

public:
	// 设置视频帧队列长度
	// 这里这样干的目的，其实是为了限制一下解码器解码速度，避免内存暴涨
	int SetVideoFrameQueueLength(int len = 1500);
	int PushVideoFrame(AVFrame *frame);
	AVFrame* PopVideoFrame();

	int SetAudioFrameQueueLength(int len = 1500);
	int PushAudioFrame(AVFrame *frame);
	AVFrame* PopAudioFrame();

public:
	static void AudioDataCallback(void *userdata, unsigned char *stream, int len);

public:
	MediaVideoInfo media_video_info_;
	MediaAudioInfo media_audio_info_;

public:
	int player_screen_width_;					// 播放器窗口宽度，从主窗口传递下来
	int player_screen_height_;					// 播放器窗口高度，从主窗口传递下来

	SDL_Window *sdl_window_;
	SDL_Renderer *sdl_renderer_;				// 播放器渲染器
	SDL_Texture *sdl_texture_;					// 播放器纹理（可以理解为分屏）
	SDL_Rect sub_screen_rect_;					// 分屏区域
	
	base::Thread *sdl_refresh_thread_;			// 分屏画面刷新线程
	base::Thread *sdl_refresh_control_thread_;	// 分屏画面刷新线程
	base::Thread *sdl_control_thread_;			// 分屏SDL事件处理线程
	base::WaitableEvent *play_event_;			// 
	enum SUBSCREEN_EVENT_TYPE play_event_type_;

	base::WaitableEvent *video_info_notify_event_;
	base::WaitableEvent *audio_info_notify_event_;

	int thread_exit_;

public:
	//AVFrame *video_frame_;			// 原始视频帧
	AVFrame *video_frame_yuv_;			// YUV视频帧
	unsigned char *video_out_buffer_;			// 视频图像缓冲区
	SwsContext *video_img_convert_ctx_;		// 图像转换上下文
	int video_frame_rate_;				// 视频帧率：帧/秒

public:
	AVFrame *audio_frame_;
	unsigned char *audio_out_buffer_;

public:
	// 帧链表
	base::LinkedList<FrameNode> video_list_;	// 视频帧链表
	base::LinkedList<FrameNode> audio_list_;	// 音频帧链表

	int video_list_max_len_;
	int audio_list_max_len_;

	int video_list_current_len_;
	int audio_list_current_len_;

	base::Lock video_list_lock_;
	base::Lock audio_list_lock_;

public:
	//base::Thread *main_working_thread_;
	bgMediaDecoderV3 *decoder_v3_;	// 视音频解码器
	enum _Decoder_State_ decoder_state_;
	enum SUBSCREEN_WORKING_STATE sub_screen_working_state_;

public:
	int current_index_;
};


// -----------------------------------------------------------------------------------
// 播放器类，主要负责子窗口的调度与控制
class SDL2PlayerEx
{
public:
	SDL2PlayerEx();
	~SDL2PlayerEx();

public:
	int Init(int width = 800, int height = 600, enum SUBSCREEN_TYPE type = SubScreen_One);
	void Close();

public:
	int GetFreeSubScreenIndex();

	// 在制定的子屏幕上播放，子屏幕索引为-1时为自动
	int PlayOnSubScreen(const char *url, int sub_screen_index = -1);
	int PauseOnSubScreen(int sub_screen_index);
	int ResumeOnSubScreen(int sub_screen_index);
	int StopOnSubScreen(int sub_screen_index);
	int StopAll();

public:
	int screen_width_;						// 播放器窗口宽度
	int screen_height_;						// 播放器窗口高度

	SDL_Window *sdl_window_;				// 播放器窗口
	SDL_Renderer *sdl_renderer_;			// 播放器渲染器

	int sub_screen_count_;					// 分屏数量，不能大于64
	SDL2PlayerSubScreen sub_screens_[64];	// 分屏对象，最大支持64个分屏
};

#endif