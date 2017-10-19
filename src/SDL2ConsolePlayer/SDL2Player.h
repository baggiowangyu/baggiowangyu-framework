#ifndef _SDL2PLAYER_H_
#define _SDL2PLAYER_H_

#include "bgMediaDecoder/bgMediaDecoder_V3.h"
#include "ext/SDL/include/SDL.h"

class SDL2Player : public bgMediaDecoderV3Notify
{
public:
	SDL2Player();
	~SDL2Player();

public:
	virtual void StateNotify(std::string statestr, int stateindex);
	virtual void ErrorNotify(std::string errstr, int errcode);
	virtual void DecodeNotify(AVFrame *frame_data, int frame_type);
	virtual void VideoInfoNotify(MediaVideoInfo video_info);
	virtual void AudioInfoNotify();

public:
	int Play(const char *url);

public:
	int screen_width_;
	int screen_height_;

	MediaVideoInfo media_video_info_;

	SDL_Rect sub_screen_rect_;

public:
	SDL_Window *sdl_window_;
	SDL_Renderer *sdl_renderer_;
	SDL_Texture *sdl_texture_;

public:
	AVFrame *frame_;
	AVFrame *frame_yuv_;
	unsigned char *out_buffer_;
	SwsContext *img_convert_ctx_;

public:
	// Ö¡Á´±í
	base::LinkedList<FrameNode> video_list_;
	base::LinkedList<FrameNode> audio_list_;

public:
	bgMediaDecoderV3 *decoder_v3_;
};

#endif