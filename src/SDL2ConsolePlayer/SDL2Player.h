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
	// ������״̬֪ͨ
	virtual void StateNotify(std::string statestr, enum _Decoder_State_ stateindex);

	// ����������֪ͨ
	virtual void ErrorNotify(std::string errstr, int errcode);

	// ����������֪ͨ
	virtual void DecodeNotify(AVFrame *frame_data, int frame_type);

	// ��������Ƶ��Ϣ֪ͨ
	virtual void VideoInfoNotify(MediaVideoInfo video_info);

	// ��������Ƶ��Ϣ֪ͨ
	virtual void AudioInfoNotify(MediaAudioInfo audio_info);

public:
	// ��������Ƶ
	int Play(const char *url);

public:
	static void MainWorkingTask(SDL2Player *player);
	static int refresh_video(void *opaque);

public:
	MediaVideoInfo media_video_info_;

public:
	int screen_width_;				// ���������ڿ��
	int screen_height_;				// ���������ڸ߶�

	SDL_Window *sdl_window_;		// ����������
	SDL_Renderer *sdl_renderer_;	// ��������Ⱦ��
	SDL_Texture *sdl_texture_;		// �����������������Ϊ������

	SDL_Rect sub_screen_rect_;		// ��������

	SDL_Thread *sdl_refresh_thread_;
	int thread_exit_;

public:
	AVFrame *frame_;				// ԭʼ��Ƶ֡
	AVFrame *frame_yuv_;			// YUV��Ƶ֡
	unsigned char *out_buffer_;		// ��Ƶͼ�񻺳���
	SwsContext *img_convert_ctx_;	// ͼ��ת��������

	int frame_rate_;

public:
	// ֡����
	base::LinkedList<FrameNode> video_list_;	// ��Ƶ֡����
	base::LinkedList<FrameNode> audio_list_;	// ��Ƶ֡����
	std::queue<AVFrame *> video_list_2_;

public:
	bgMediaDecoderV3 *decoder_v3_;	// ����Ƶ������
	enum _Decoder_State_ decoder_state_;

public:
	base::Thread *main_working_thread_;
};

#endif