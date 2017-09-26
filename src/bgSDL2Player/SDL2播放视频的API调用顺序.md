# SDL2 播放视频的API调用顺序 #

## 1.解码器的工作流程 ##
- 初始化FFmpeg工作环境
	- av_register_all();
	- avformat_network_init();
- 打开等待播放的媒体文件
	- avformat_open_input();
- 查找媒体文件的流信息
- 找到视频流索引
- 从视频流索引中的编码ID查找解码器
- 打开解码器
- 申请帧内存
- 申请输出缓冲区内存
- 填充图像数组
- 获取像素转换上下文
- 读取编码包
- 解码
- 转换像素
- 释放资源

## 1.一个控制台播放器的API调用顺序 ##
- int stdcall SDL_Init(Uint32 flags); 
	- 
- SDL_CreateWindow()
	- s
- SDL_CreateRenderer()
- SDL_CreateTexture()
- SDL_UpdateTexture() / SDL_UpdateYUVTexture()
- SDL_RenderClear()
- SDL_RenderCopy()
- SDL_RenderPresent()
- SDL_Delay()
- SDL_Quit()