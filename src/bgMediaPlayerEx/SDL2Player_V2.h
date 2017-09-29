#ifndef _SDL2PLAYER_V2_H_
#define _SDL2PLAYER_V2_H_

#include <iostream>
#include "bgMediaPlayerBaseDef.h"

class PlayerScreenObject;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;


// 基于SDL2的视频播放器
// 本对象只作为一个播放器容器，用于对所有的分屏对象进行管理
// 这里我们默认支持以下几种分屏：1、4、8、9、16
class SDL2PlayerV2
{
public:
	SDL2PlayerV2();
	SDL2PlayerV2(std::string container_name);
	~SDL2PlayerV2();

public:
	/** 初始化播放容器
	 * 参数：
	 *	@player_container_width		播放器容器的宽度
	 *	@player_container_height	播放器容器的高度
	 *	@player_gui_hwnd			GUI播放器容器句柄
	 *	@sub_screen_count			分屏数量，默认为1
	 */
	int Initialize(int player_container_width, int player_container_height, void *player_gui_hwnd = nullptr, int sub_screen_count = 1);

	/**
	 * 销毁播放器容器
	 */
	void Destroy();

public:
	/**
	 * 更改分屏数
	 * 参数
	 *	@sub_screen_count	分屏数量
	 */
	int ChangeSubScreenCount(int sub_screen_count);

public:
	/**
	 * 播放媒体文件
	 * 参数：
	 *	@media_url			媒体路径
	 *	@play_mode			播放模式，分为直播模式和录像模式
	 *	@sub_screen_index	指定用于播放的分屏索引，为-1时自动寻找一个空闲的分屏进行播放
	 */
	int Play(std::string media_url, enum PlayMode play_mode = PlayMode_Record, int sub_screen_index = -1);

	/**
	 * 暂停播放
	 * 参数：
	 *	@
	 */
	int Pause(int sub_screen_index);

	/**
	 * 停止播放
	 * 参数：
	 *	@
	 */
	int Stop(int sub_screen_index);

private:
	// 播放器分屏数
	int sub_screen_num_;

	// 播放器分屏数组
	PlayerScreenObject *sub_screen_objects_;

	int InitSubScreens();

private:
	// 播放容器名称
	std::string container_name_;

	// 播放容器尺寸
	int player_container_width_;
	int player_container_height_;

	// SDL窗口
	SDL_Window *sdl_window_;

	// SDL渲染器
	SDL_Renderer *sdl_renderer_;

	// SDL纹理
	SDL_Texture *sdl_texture_;
};

#endif
