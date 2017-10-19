# ffplay媒体播放器源码分析 #
   
   
本文主要从ffplay的各个结构出发，围绕ffplay的核心功能（解码、播放、播放控制等）进行分析，最终将ffplay项目精简为只包含核心业务的模块。

完成这项工作后，下一步就着手于多屏播放和浏览器插件等业务模块的转化输出。

## 结构体解析 ##
**媒体编码包链表：**
    
    typedef struct MyAVPacketList {   
        AVPacket pkt;   
        struct MyAVPacketList *next;   
        int serial;   
    } MyAVPacketList;

- pkt：编码数据包
- next：下一个元素
- serial：



**媒体包队列：**

    typedef struct PacketQueue {
        MyAVPacketList *first_pkt, *last_pkt;
        int nb_packets;
        int size;
        int64_t duration;
        int abort_request;
        int serial;
        SDL_mutex *mutex;
        SDL_cond *cond;
    } PacketQueue;

- first_pkt：队头
- last_pkt：队尾
- nb_packets：队列中包数量
- size：
- duration：
- abort_request：
- serial：
- mutex：互斥体，用于同步
- cond：条件变量，用于同步



**音频参数：**

    typedef struct AudioParams {
        int freq;
        int channels;
        int64_t channel_layout;
        enum AVSampleFormat fmt;
        int frame_size;
        int bytes_per_sec;
    } AudioParams;

- freq：
- channels：
- channel_layout：
- fmt：
- frame_size：
- bytes_per_sec：



**时钟：**

    typedef struct Clock {
        double pts;           /* clock base */
        double pts_drift;     /* clock base minus time at which we updated the clock */
        double last_updated;
        double speed;
        int serial;           /* clock is based on a packet with this serial */
        int paused;
        int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
    } Clock;

- pts：
- pts_drift：
- last_updated：
- speed：
- serial：
- paused：
- queue_serial：



**帧：**

    typedef struct Frame {
        AVFrame *frame;
        AVSubtitle sub;
        int serial;
        double pts;           /* presentation timestamp for the frame */
        double duration;      /* estimated duration of the frame */
        int64_t pos;          /* byte position of the frame in the input file */
        int width;
        int height;
        int format;
        AVRational sar;
        int uploaded;
        int flip_v;
    } Frame;

- frame：
- sub：
- serial：
- pts：
- duration：
- pos：
- width：
- height：
- format：
- sar：
- uploaded：
- flip_v：



**帧队列：**

    typedef struct FrameQueue {
        Frame queue[FRAME_QUEUE_SIZE];
        int rindex;
        int windex;
        int size;
        int max_size;
        int keep_last;
        int rindex_shown;
        SDL_mutex *mutex;
        SDL_cond *cond;
        PacketQueue *pktq;
    } FrameQueue;

- queue：
- rindex：
- windex：
- size：
- max_size：
- keep_last：
- rindex_shown：
- mutex：
- cond：
- pktq：



**解码器：**

    typedef struct Decoder {
        AVPacket pkt;
        AVPacket pkt_temp;
        PacketQueue *queue;
        AVCodecContext *avctx;
        int pkt_serial;
        int finished;
        int packet_pending;
        SDL_cond *empty_queue_cond;
        int64_t start_pts;
        AVRational start_pts_tb;
        int64_t next_pts;
        AVRational next_pts_tb;
        SDL_Thread *decoder_tid;
    } Decoder;

- pkt：
- pkt_temp：
- queue：
- avctx：
- pkt_serial：
- finished：
- packet_pending：
- empty_queue_cond：
- start_pts：
- start_pts_tb：
- next_pts：
- next_pts_tb：
- decoder_tid：



**视频状态：**

    typedef struct VideoState {
        SDL_Thread *read_tid;
        AVInputFormat *iformat;
        int abort_request;
        int force_refresh;
        int paused;
        int last_paused;
        int queue_attachments_req;
        int seek_req;
        int seek_flags;
        int64_t seek_pos;
        int64_t seek_rel;
        int read_pause_return;
        AVFormatContext *ic;
        int realtime;
    
        Clock audclk;
        Clock vidclk;
        Clock extclk;
    
        FrameQueue pictq;
        FrameQueue subpq;
        FrameQueue sampq;
    
        Decoder auddec;
        Decoder viddec;
        Decoder subdec;
    
        int audio_stream;
    
        int av_sync_type;
    
        double audio_clock;
        int audio_clock_serial;
        double audio_diff_cum; /* used for AV difference average computation */
        double audio_diff_avg_coef;
        double audio_diff_threshold;
        int audio_diff_avg_count;
        AVStream *audio_st;
        PacketQueue audioq;
        int audio_hw_buf_size;
        uint8_t *audio_buf;
        uint8_t *audio_buf1;
        unsigned int audio_buf_size; /* in bytes */
        unsigned int audio_buf1_size;
        int audio_buf_index; /* in bytes */
        int audio_write_buf_size;
        int audio_volume;
        int muted;
        struct AudioParams audio_src;
    #if CONFIG_AVFILTER
        struct AudioParams audio_filter_src;
    #endif
        struct AudioParams audio_tgt;
        struct SwrContext *swr_ctx;
        int frame_drops_early;
        int frame_drops_late;
    
        enum ShowMode {
            SHOW_MODE_NONE = -1,
            SHOW_MODE_VIDEO = 0, 
            SHOW_MODE_WAVES, 
            SHOW_MODE_RDFT, 
            SHOW_MODE_NB
        } show_mode;
        int16_t sample_array[SAMPLE_ARRAY_SIZE];
        int sample_array_index;
        int last_i_start;
        RDFTContext *rdft;
        int rdft_bits;
        FFTSample *rdft_data;
        int xpos;
        double last_vis_time;
        SDL_Texture *vis_texture;
        SDL_Texture *sub_texture;
        SDL_Texture *vid_texture;
    
        int subtitle_stream;
        AVStream *subtitle_st;
        PacketQueue subtitleq;
    
        double frame_timer;
        double frame_last_returned_time;
        double frame_last_filter_delay;
        int video_stream;
        AVStream *video_st;
        PacketQueue videoq;
        double max_frame_duration;      // maximum duration of a frame - above this, we consider the     jump a timestamp discontinuity
        struct SwsContext *img_convert_ctx;
        struct SwsContext *sub_convert_ctx;
        int eof;
    
        char *filename;
        int width, height, xleft, ytop;
        int step;
    
    #if CONFIG_AVFILTER
        int vfilter_idx;
        AVFilterContext *in_video_filter;   // the first filter in the video chain
        AVFilterContext *out_video_filter;  // the last filter in the video chain
        AVFilterContext *in_audio_filter;   // the first filter in the audio chain
        AVFilterContext *out_audio_filter;  // the last filter in the audio chain
        AVFilterGraph *agraph;              // audio filter graph
    #endif
    
        int last_video_stream, last_audio_stream, last_subtitle_stream;
    
        SDL_cond *continue_read_thread;
    } VideoState;

- 参数略，这一块实际上是可以进行大量拆分的；

## 用户选项 ##

    static AVInputFormat *file_iformat;
    static const char *input_filename;
    static const char *window_title;
    static int default_width  = 640;
    static int default_height = 480;
    static int screen_width  = 0;
    static int screen_height = 0;
    static int audio_disable;
    static int video_disable;
    static int subtitle_disable;
    static const char* wanted_stream_spec[AVMEDIA_TYPE_NB] = {0};
    static int seek_by_bytes = -1;
    static int display_disable;
    static int borderless;
    static int startup_volume = 100;
    static int show_status = 1;
    static int av_sync_type = AV_SYNC_AUDIO_MASTER;
    static int64_t start_time = AV_NOPTS_VALUE;
    static int64_t duration = AV_NOPTS_VALUE;
    static int fast = 0;
    static int genpts = 0;
    static int lowres = 0;
    static int decoder_reorder_pts = -1;
    static int autoexit;
    static int exit_on_keydown;
    static int exit_on_mousedown;
    static int loop = 1;
    static int framedrop = -1;
    static int infinite_buffer = -1;
    static enum ShowMode show_mode = SHOW_MODE_NONE;
    static const char *audio_codec_name;
    static const char *subtitle_codec_name;
    static const char *video_codec_name;
    double rdftspeed = 0.02;
    static int64_t cursor_last_shown;
    static int cursor_hidden = 0;
    #if CONFIG_AVFILTER
    static const char **vfilters_list = NULL;
    static int nb_vfilters = 0;
    static char *afilters = NULL;
    #endif
    static int autorotate = 1;

## 执行依赖模块 ##

    static int is_full_screen;
    static int64_t audio_callback_time;
    
    static AVPacket flush_pkt;
    
    #define FF_QUIT_EVENT    (SDL_USEREVENT + 2)

    static SDL_Window *window;
    static SDL_Renderer *renderer;

- is_full_screen：是否全屏
- audio_callback_time：
- flush_pkt：用于刷新的媒体编码包
- window：SDL窗口
- renderer：SDL渲染器

## 业务处理逻辑 ##
### 编码包队列处理 ###
#### 编码包入队 ####

    static int packet_queue_put_private(PacketQueue *q, AVPacket *pkt)
    {
        MyAVPacketList *pkt1;
    
        if (q->abort_request)
           return -1;
    
        pkt1 = av_malloc(sizeof(MyAVPacketList));
        if (!pkt1)
            return -1;
        pkt1->pkt = *pkt;
        pkt1->next = NULL;
        if (pkt == &flush_pkt)
            q->serial++;
        pkt1->serial = q->serial;
    
        if (!q->last_pkt)
            q->first_pkt = pkt1;
        else
            q->last_pkt->next = pkt1;
        q->last_pkt = pkt1;
        q->nb_packets++;
        q->size += pkt1->pkt.size + sizeof(*pkt1);
        q->duration += pkt1->pkt.duration;
        /* XXX: should duplicate packet data in DV case */
        SDL_CondSignal(q->cond);
        return 0;
    }

- 传入参数：要入队的队列指针、编码包
- 工作流程：
	- 申请一个包链表对象；
	- 将编码包设置到对应的成员中；
	- 将链表内指向下一个包的指针设置为空；
	- 如果当前传入的编码包与flush_pkt相同，则队列序列自增1；
	- 