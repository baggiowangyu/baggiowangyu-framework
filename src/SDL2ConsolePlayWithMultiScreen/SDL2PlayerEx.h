#ifndef _SDL2PLAYEREX_H_
#define _SDL2PLAYEREX_H_

#include "base/containers/linked_list.h"
#include "base/threading/thread.h"

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

typedef enum SUBSCREEN_WORKING_STATE
{
	SubScreen_Free,				// ����״̬��������ɺ�Ҳ�������״̬
	SubScreen_Playing,			// ����״̬��������������Ⱦ������Ƶ
	SubScreen_Pausing,			// ��ͣ״̬����������ͣ��Ⱦ����
	SubScreen_PerFramePlaying	// ��֡���ţ���������������֡����״̬
};

typedef enum SUBSCREEN_EVENT_TYPE
{
	SubScreen_Refresh,
	SubScreen_
};


// -----------------------------------------------------------------------------------
// ����Ƶ֡�ڵ㣬�����������Ƶ֡����
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
// �Ӵ����࣬�ǲ�������Ƶ��ʵ�壬��������Ƶ���롢֡ͼ��ѹ��ת���ȹ���
class SDL2PlayerSubScreen : public bgMediaDecoderV3Notify
{
public:
	SDL2PlayerSubScreen();
	~SDL2PlayerSubScreen();

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
	virtual void AudioInfoNotify();

public:
	int Init(int player_screen_width, int player_screen_height, SDL_Renderer *renderer);
	void Close();

	int Play(const char *url);
	int Pause();
	int Resume();
	int Stop();

public:
	enum SUBSCREEN_WORKING_STATE GetWorkingState();

public:
	static void MainWorkingTask(SDL2PlayerSubScreen *sub_screen);
	static void refresh_video(void *opaque);

public:
	MediaVideoInfo media_video_info_;

public:
	int player_screen_width_;		// ���������ڿ�ȣ��������ڴ�������
	int player_screen_height_;		// ���������ڸ߶ȣ��������ڴ�������
	SDL_Renderer *sdl_renderer_;	// ��������Ⱦ��
	SDL_Texture *sdl_texture_;		// �����������������Ϊ������
	SDL_Rect sub_screen_rect_;		// ��������
	SDL_Event sub_screen_event_;	// �����¼������ڿ��Ʒ���
	SDL_Thread *sdl_refresh_thread_;
	//base::WaitableEvent sub_screen_event_;

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

public:
	base::Thread *main_working_thread_;
	bgMediaDecoderV3 *decoder_v3_;	// ����Ƶ������
	enum _Decoder_State_ decoder_state_;
	enum SUBSCREEN_WORKING_STATE player_working_state_;
};


// -----------------------------------------------------------------------------------
// �������࣬��Ҫ�����Ӵ��ڵĵ��������
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

	// ���ƶ�������Ļ�ϲ��ţ�����Ļ����Ϊ-1ʱΪ�Զ�
	int PlayOnSubScreen(const char *url, int sub_screen_index = -1);
	int PauseOnSubScreen(int sub_screen_index);
	int ResumeOnSubScreen(int sub_screen_index);
	int StopOnSubScreen(int sub_screen_index);
	int StopAll();

public:
	int screen_width_;						// ���������ڿ��
	int screen_height_;						// ���������ڸ߶�

	SDL_Window *sdl_window_;				// ����������
	SDL_Renderer *sdl_renderer_;			// ��������Ⱦ��

	int sub_screen_count_;
	SDL2PlayerSubScreen sub_screens_[64];	// �����������֧��64������
};

#endif