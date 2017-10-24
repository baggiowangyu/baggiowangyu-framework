#ifndef _SDL2PLAYER_H_
#define _SDL2PLAYER_H_

#include "bgMediaDecoder/bgMediaDecoder_V3.h"
#include "ext/SDL/include/SDL.h"
#include "MediaFrameContainer.h"

//Refresh Event
#define REFRESH_EVENT	(SDL_USEREVENT + 1)
#define BREAK_EVENT		(SDL_USEREVENT + 2)

class SDL2Player : public bgMediaDecoderV3Notify
{
public:
	SDL2Player();
	~SDL2Player();

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
	// 播放视音频
	int Play(const char *url);

public:
	static void MainWorkingTask(SDL2Player *player);
	static int refresh_video(void *opaque);

public:
	MediaVideoInfo media_video_info_;

public:
	int screen_width_;				// 播放器窗口宽度
	int screen_height_;				// 播放器窗口高度

	SDL_Window *sdl_window_;		// 播放器窗口
	SDL_Renderer *sdl_renderer_;	// 播放器渲染器
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
	std::queue<AVFrame *> video_list_2_;

public:
	bgMediaDecoderV3 *decoder_v3_;	// 视音频解码器
	enum _Decoder_State_ decoder_state_;

public:
	base::Thread *main_working_thread_;
};

#endif