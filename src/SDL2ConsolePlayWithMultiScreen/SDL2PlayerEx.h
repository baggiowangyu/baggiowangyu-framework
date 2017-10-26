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
	SubScreen_Free,				// ����״̬��������ɺ�Ҳ�������״̬
	SubScreen_Playing,			// ����״̬��������������Ⱦ������Ƶ
	SubScreen_Pausing,			// ��ͣ״̬����������ͣ��Ⱦ����
	SubScreen_PerFramePlaying	// ��֡���ţ���������������֡����״̬
};

typedef enum SUBSCREEN_EVENT_TYPE
{
	SubScreen_Refresh,
	SubScreen_Break
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
// �����Ӵ�������¼������ǿ������ó���һ���Ӵ���֮�⣬���Ӵ��ڻ����²������κ��ڲ���Ϣ
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
	// ������Ƶ֡���г���
	// ���������ɵ�Ŀ�ģ���ʵ��Ϊ������һ�½����������ٶȣ������ڴ汩��
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
	int player_screen_width_;					// ���������ڿ�ȣ��������ڴ�������
	int player_screen_height_;					// ���������ڸ߶ȣ��������ڴ�������

	SDL_Window *sdl_window_;
	SDL_Renderer *sdl_renderer_;				// ��������Ⱦ��
	SDL_Texture *sdl_texture_;					// �����������������Ϊ������
	SDL_Rect sub_screen_rect_;					// ��������
	
	base::Thread *sdl_refresh_thread_;			// ��������ˢ���߳�
	base::Thread *sdl_refresh_control_thread_;	// ��������ˢ���߳�
	base::Thread *sdl_control_thread_;			// ����SDL�¼������߳�
	base::WaitableEvent *play_event_;			// 
	enum SUBSCREEN_EVENT_TYPE play_event_type_;

	base::WaitableEvent *video_info_notify_event_;
	base::WaitableEvent *audio_info_notify_event_;

	int thread_exit_;

public:
	//AVFrame *video_frame_;			// ԭʼ��Ƶ֡
	AVFrame *video_frame_yuv_;			// YUV��Ƶ֡
	unsigned char *video_out_buffer_;			// ��Ƶͼ�񻺳���
	SwsContext *video_img_convert_ctx_;		// ͼ��ת��������
	int video_frame_rate_;				// ��Ƶ֡�ʣ�֡/��

public:
	AVFrame *audio_frame_;
	unsigned char *audio_out_buffer_;

public:
	// ֡����
	base::LinkedList<FrameNode> video_list_;	// ��Ƶ֡����
	base::LinkedList<FrameNode> audio_list_;	// ��Ƶ֡����

	int video_list_max_len_;
	int audio_list_max_len_;

	int video_list_current_len_;
	int audio_list_current_len_;

	base::Lock video_list_lock_;
	base::Lock audio_list_lock_;

public:
	//base::Thread *main_working_thread_;
	bgMediaDecoderV3 *decoder_v3_;	// ����Ƶ������
	enum _Decoder_State_ decoder_state_;
	enum SUBSCREEN_WORKING_STATE sub_screen_working_state_;

public:
	int current_index_;
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

	int sub_screen_count_;					// �������������ܴ���64
	SDL2PlayerSubScreen sub_screens_[64];	// �����������֧��64������
};

#endif