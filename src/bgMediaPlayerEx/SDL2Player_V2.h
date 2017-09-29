#ifndef _SDL2PLAYER_V2_H_
#define _SDL2PLAYER_V2_H_

#include <iostream>
#include "bgMediaPlayerBaseDef.h"

class PlayerScreenObject;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;


// ����SDL2����Ƶ������
// ������ֻ��Ϊһ�����������������ڶ����еķ���������й���
// ��������Ĭ��֧�����¼��ַ�����1��4��8��9��16
class SDL2PlayerV2
{
public:
	SDL2PlayerV2();
	SDL2PlayerV2(std::string container_name);
	~SDL2PlayerV2();

public:
	/** ��ʼ����������
	 * ������
	 *	@player_container_width		�����������Ŀ��
	 *	@player_container_height	�����������ĸ߶�
	 *	@player_gui_hwnd			GUI�������������
	 *	@sub_screen_count			����������Ĭ��Ϊ1
	 */
	int Initialize(int player_container_width, int player_container_height, void *player_gui_hwnd = nullptr, int sub_screen_count = 1);

	/**
	 * ���ٲ���������
	 */
	void Destroy();

public:
	/**
	 * ���ķ�����
	 * ����
	 *	@sub_screen_count	��������
	 */
	int ChangeSubScreenCount(int sub_screen_count);

public:
	/**
	 * ����ý���ļ�
	 * ������
	 *	@media_url			ý��·��
	 *	@play_mode			����ģʽ����Ϊֱ��ģʽ��¼��ģʽ
	 *	@sub_screen_index	ָ�����ڲ��ŵķ���������Ϊ-1ʱ�Զ�Ѱ��һ�����еķ������в���
	 */
	int Play(std::string media_url, enum PlayMode play_mode = PlayMode_Record, int sub_screen_index = -1);

	/**
	 * ��ͣ����
	 * ������
	 *	@
	 */
	int Pause(int sub_screen_index);

	/**
	 * ֹͣ����
	 * ������
	 *	@
	 */
	int Stop(int sub_screen_index);

private:
	// ������������
	int sub_screen_num_;

	// ��������������
	PlayerScreenObject *sub_screen_objects_;

	int InitSubScreens();

private:
	// ������������
	std::string container_name_;

	// ���������ߴ�
	int player_container_width_;
	int player_container_height_;

	// SDL����
	SDL_Window *sdl_window_;

	// SDL��Ⱦ��
	SDL_Renderer *sdl_renderer_;

	// SDL����
	SDL_Texture *sdl_texture_;
};

#endif
