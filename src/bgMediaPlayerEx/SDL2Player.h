#ifndef _SDL2_PLAYER_H_
#define _SDL2_PLAYER_H_


#include "base/containers/linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

#include "SDL.h"
#include "bgMediaDecoder/bgMediaDecoder.h"

class SDL2PlayerFrameNode : public base::LinkNode<SDL2PlayerFrameNode>
{
public:
	explicit SDL2PlayerFrameNode(AVFrame *src_frame)
		: frame(src_frame)
	{
		//frame = av_frame_alloc();
		//av_frame_copy(frame, src_frame);
	}

	~SDL2PlayerFrameNode()
	{
		//av_frame_free(&frame);
		//frame = nullptr;
	}

public:
	AVFrame* GetFrameObject() {return frame;}

private:
	AVFrame *frame;
};

class SDL2Player : public bgMediaDecoderCallback
{
public:
	typedef enum _SDL2_PLAYER_STATE_
	{
		SDL2Player_None,
		SDL2Player_Initializing,
		SDL2Player_Ready,
		SDL2Player_GoingToPlay,
		SDL2Player_Playing,
		SDL2Player_Pausing,
		SDL2Player_Stopped
	} SDL2_PLAYER_STATE ;

public:
	SDL2Player();
	~SDL2Player();

public:
	int Initialize(std::string window_name, int window_width, int window_height);
	void Destroy();

public:
	int Play(std::string media_url);
	int Pause();
	int Stop();

public:
	int InputFrameData(AVFrame* frame);

public:
	static void PlayThread(SDL2Player *player);

	int player_state;
	base::LinkedList<SDL2PlayerFrameNode> frame_linked_list;

public:
	virtual void VideoSizeCallback(int width, int height);
	virtual void MediaDecoderCallback(AVFrame* frame_data);

public:
	bgMediaDecoder *decoder;

public:
	base::Thread *player_thread;

public:
	// 视频播放器的宽高度
	int player_width;
	int player_height;

	// 视频源的宽高度
	int video_width;
	int video_height;

	std::string sdl_window_name;

	// SDL窗口
	SDL_Window *sdl_window;

	// SDL渲染器
	SDL_Renderer *sdl_renderer;

	// SDL纹理
	SDL_Texture *sdl_texture;

	// 
	SDL_Rect sdl_rect;
};

#endif
