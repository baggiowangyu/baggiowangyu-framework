#ifndef _SDL2PLAYEREX_H_
#define _SDL2PLAYEREX_H_

#include "base/containers/linked_list.h"

#include "bgMediaDecoder/bgMediaDecoder_V3.h"
#include "ext/SDL/include/SDL.h"

typedef enum SUBSCREEN_TYPE
{
	SubScreen_One,
	SubScreen_Four,
	SubScreen_Eight,
	SubScreen_Nine,
	SubScreen_Sixteen
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
	virtual void AudioInfoNotify();

public:
	int Init(SDL_Renderer *renderer);
	int Play(const char *url);

public:
	static void MainWorkingTask(SDL2PlayerSubScreen *sub_screen);
	static void refresh_video(void *opaque);

public:
	MediaVideoInfo media_video_info_;

public:
	SDL_Renderer *sdl_renderer_;			// 播放器渲染器
	SDL_Texture *sdl_texture_;		// 播放器纹理（可以理解为分屏）
	SDL_Rect sub_screen_rect_;		// 分屏区域
	SDL_Thread *sdl_refresh_thread_;
	int thread_exit_;

public:
	AVFrame *frame_;				// 原始视频帧
	AVFrame *frame_yuv_;			// YUV视频帧
	unsigned char *out_buffer_;		// 视频图像缓冲区
	SwsContext *img_convert_ctx_;	// 图像转换上下文

	int frame_rate_;

public:
	// 帧链表
	base::LinkedList<FrameNode> video_list_;	// 视频帧链表
	base::LinkedList<FrameNode> audio_list_;	// 音频帧链表

public:
	bgMediaDecoderV3 *decoder_v3_;	// 视音频解码器
	enum _Decoder_State_ decoder_state_;
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

	int sub_screen_count_;
	SDL2PlayerSubScreen sub_screens_[64];	// 分屏对象，最大支持64个分屏
};

#endif