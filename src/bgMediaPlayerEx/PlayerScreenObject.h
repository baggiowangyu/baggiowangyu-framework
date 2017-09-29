#ifndef _PLAYER_SCREEN_OBJECT_H_
#define _PLAYER_SCREEN_OBJECT_H_

#include "base/location.h"
#include "base/bind.h"
#include "base/threading/thread.h"

#include <iostream>
#include "bgMediaPlayerBaseDef.h"

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Rect;
class bgMediaObject;
class bgMediaDecoderV2;
class base::Thread;

// 播放器分屏对象
// 每一个分屏都能独立的对一个视频进行操作
class PlayerScreenObject
{
public:
	PlayerScreenObject();
	~PlayerScreenObject();

public:
	/**
	 * 初始化分屏播放器
	 * 参数：
	 *	@rect			分屏所在矩形区域
	 *	@img_data		就绪状态下显示的图片
	 *	@img_data_len	图片大小
	 */
	int Initialize(SDL_Rect *rect, const unsigned char *img_data, int img_data_len, SDL_Renderer *renderer, SDL_Texture *texture);

	/**
	 * 销毁分屏播放器
	 */
	void Destroy();

public:
	/**
	 * 播放
	 * 参数：
	 *	@media_url		媒体路径
	 *	@play_mode		播放模式，实时流、录像流
	 */
	int Play(std::string media_url, enum PlayMode play_mode);

	/**
	 * 暂停
	 */
	int Pause();

	/**
	 * 停止
	 */
	int Stop();

public:
	bgMediaDecoderV2* GetDecoderV2Object();
	bgMediaObject* GetMediaObject();
	SDL_Renderer* GetPlayerRenderer();
	SDL_Texture* GetPlayerTexture();
	SDL_Rect* GetSubScreenRect();

private:
	/**
	 * 绘制图像
	 * 参数：
	 *	@img_data		就绪状态下显示的图片
	 *	@img_data_len	图片大小
	 */
	int DrawImage(unsigned char *img_data, int img_data_len);

private:
	/**
	 * 播放任务
	 * 参数：
	 *	@screen
	 */
	static void WorkingTask(PlayerScreenObject *screen);

	/**
	 * 同步控制任务
	 * 参数：
	 *	@screen
	 */
	static void SyncTask(PlayerScreenObject *screen);

private:
	base::Thread *working_thread_;
	base::Thread *sync_thread_;

private:
	// 
	SDL_Renderer *sdl_renderer_;

	// 
	SDL_Texture *sdl_texture_;

	// 分屏区域
	SDL_Rect *screen_rect;

	// 空闲状态显示的图片
	unsigned char *img_data_;

	// 图片数据大小
	int img_data_len_;

private:
	// 媒体文件对象
	bgMediaObject *media_object_;

	// 播放器播放时需要的参数
	int64_t total_duration_;

	// 视音频解码器
	bgMediaDecoderV2 *media_decoder_;
};

#endif
