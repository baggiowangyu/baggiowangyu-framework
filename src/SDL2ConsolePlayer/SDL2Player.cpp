#include "stdafx.h"

#include "SDL2PlayerStructure.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
};
#endif

#include <cmdutils.h>

static const OptionDef options[] = {
	{ "L"				, OPT_EXIT, { (void*) show_license},     "show license" },
	{ "h"				, OPT_EXIT, { (void*)show_help},         "show help", "topic" },
	{ "?"				, OPT_EXIT, { (void*)show_help},         "show help", "topic" },
	{ "help"			, OPT_EXIT, { (void*)show_help},         "show help", "topic" },
	{ "-help"			, OPT_EXIT, { (void*)show_help},         "show help", "topic" },
	{ "version"			, OPT_EXIT, { (void*)show_version},      "show version" },
	{ "formats"			, OPT_EXIT, { (void*)show_formats  },    "show available formats" },
	{ "codecs"			, OPT_EXIT, { (void*)show_codecs   },    "show available codecs" },
	{ "decoders"		, OPT_EXIT, { (void*)show_decoders },    "show available decoders" },
	{ "encoders"		, OPT_EXIT, { (void*)show_encoders },    "show available encoders" },
	{ "bsfs"			, OPT_EXIT, { (void*)show_bsfs     },    "show available bit stream filters" },
	{ "protocols"		, OPT_EXIT, { (void*)show_protocols},    "show available protocols" },
	{ "filters"			, OPT_EXIT, { (void*)show_filters  },    "show available filters" },
	{ "pix_fmts"		, OPT_EXIT, { (void*)show_pix_fmts },    "show available pixel formats" },
	{ "layouts"			, OPT_EXIT, { (void*)show_layouts  },    "show standard channel layouts" },
	{ "sample_fmts"		, OPT_EXIT, { (void*)show_sample_fmts }, "show available audio sample formats" },
	{ "loglevel"		, HAS_ARG,  { (void*)opt_loglevel},      "set libav* logging level", "loglevel" },
	{ "v"				, HAS_ARG,  { (void*)opt_loglevel},      "set libav* logging level", "loglevel" },
	{ "debug"			, HAS_ARG,  { (void*)opt_codec_debug},   "set debug flags", "flags" },
	{ "fdebug"			, HAS_ARG,  {(void*) opt_codec_debug},   "set debug flags", "flags" },
	{ "report"			, 0,        {(void*)opt_report}, "generate a report" },
	{ "max_alloc"		, HAS_ARG,  {(void*) opt_max_alloc},     "set maximum size of a single allocated block", "bytes" },
	{ "cpuflags"		, HAS_ARG | OPT_EXPERT,	{(void*) opt_cpuflags}, "force specific cpu flags", "flags" },
	{ "x"				, HAS_ARG, { (void*) opt_width }, "force displayed width", "width" },
	{ "y"				, HAS_ARG, { (void*) opt_height }, "force displayed height", "height" },
	{ "s"				, HAS_ARG | OPT_VIDEO, { (void*) opt_frame_size }, "set frame size (WxH or abbreviation)", "size" },
	{ "fs"				, OPT_BOOL, { &is_full_screen }, "force full screen" },
	{ "an"				, OPT_BOOL, { &audio_disable }, "disable audio" },
	{ "vn"				, OPT_BOOL, { &video_disable }, "disable video" },
	{ "ast"				, OPT_INT | HAS_ARG | OPT_EXPERT	, { &wanted_stream[AVMEDIA_TYPE_AUDIO] }, "select desired audio stream", "stream_number" },
	{ "vst"				, OPT_INT | HAS_ARG | OPT_EXPERT	, { &wanted_stream[AVMEDIA_TYPE_VIDEO] }, "select desired video stream", "stream_number" },
	{ "sst"				, OPT_INT | HAS_ARG | OPT_EXPERT	, { &wanted_stream[AVMEDIA_TYPE_SUBTITLE] }, "select desired subtitle stream", "stream_number" },
	{ "ss"				, HAS_ARG							, { (void*) opt_seek }, "seek to a given position in seconds", "pos" },
	{ "t"				, HAS_ARG							, { (void*) opt_duration }, "play  \"duration\" seconds of audio/video", "duration" },
	{ "bytes"			, OPT_INT | HAS_ARG					, { &seek_by_bytes }, "seek by bytes 0=off 1=on -1=auto", "val" },
	{ "nodisp"			, OPT_BOOL							, { &display_disable }, "disable graphical display" },
	{ "f"				, HAS_ARG							, { (void*) opt_format }, "force format", "fmt" },
	{ "pix_fmt"			, HAS_ARG | OPT_EXPERT | OPT_VIDEO	, { (void*) opt_frame_pix_fmt }, "set pixel format", "format" },
	{ "stats"			, OPT_BOOL | OPT_EXPERT				, { &show_status }, "show status", "" },
	{ "bug"				, OPT_INT | HAS_ARG | OPT_EXPERT	, { &workaround_bugs }, "workaround bugs", "" },
	{ "fast"			, OPT_BOOL | OPT_EXPERT				, { &fast }, "non spec compliant optimizations", "" },
	{ "genpts"			, OPT_BOOL | OPT_EXPERT				, { &genpts }, "generate pts", "" },
	{ "drp"				, OPT_INT | HAS_ARG | OPT_EXPERT	, { &decoder_reorder_pts }, "let decoder reorder pts 0=off 1=on -1=auto", ""},
	{ "lowres"			, OPT_INT | HAS_ARG | OPT_EXPERT	, { &lowres }, "", "" },
	{ "skiploop"		, OPT_INT | HAS_ARG | OPT_EXPERT	, { &skip_loop_filter }, "", "" },
	{ "skipframe"		, OPT_INT | HAS_ARG | OPT_EXPERT	, { &skip_frame }, "", "" },
	{ "skipidct"		, OPT_INT | HAS_ARG | OPT_EXPERT	, { &skip_idct }, "", "" },
	{ "idct"			, OPT_INT | HAS_ARG | OPT_EXPERT	, { &idct }, "set idct algo",  "algo" },
	{ "ec"				, OPT_INT | HAS_ARG | OPT_EXPERT	, { &error_concealment }, "set error concealment options",  "bit_mask" },
	{ "sync"			, HAS_ARG | OPT_EXPERT				, { (void*) opt_sync }, "set audio-video sync. type (type=audio/video/ext)", "type" },
	{ "autoexit"		, OPT_BOOL | OPT_EXPERT				, { &autoexit }, "exit at the end", "" },
	{ "exitonkeydown"	, OPT_BOOL | OPT_EXPERT				, { &exit_on_keydown }, "exit on key down", "" },
	{ "exitonmousedown"	, OPT_BOOL | OPT_EXPERT				, { &exit_on_mousedown }, "exit on mouse down", "" },
	{ "loop"			, OPT_INT | HAS_ARG | OPT_EXPERT	, { &loop }, "set number of times the playback shall be looped", "loop count" },
	{ "framedrop"		, OPT_BOOL | OPT_EXPERT				, { &framedrop }, "drop frames when cpu is too slow", "" },
	{ "infbuf"			, OPT_BOOL | OPT_EXPERT				, { &infinite_buffer }, "don't limit the input buffer size (useful with realtime streams)", "" },
	{ "window_title"	, OPT_STRING | HAS_ARG				, { &window_title }, "set window title", "window title" },
#if CONFIG_AVFILTER
	{ "vf"				, OPT_STRING | HAS_ARG				, { &vfilters }, "video filters", "filter list" },
#endif
	{ "rdftspeed"		, OPT_INT | HAS_ARG| OPT_AUDIO | OPT_EXPERT, { &rdftspeed }, "rdft speed", "msecs" },
	{ "showmode"		, HAS_ARG							, { (void*) opt_show_mode}, "select show mode (0 = video, 1 = waves, 2 = RDFT)", "mode" },
	{ "default"			, HAS_ARG | OPT_AUDIO | OPT_VIDEO | OPT_EXPERT, { (void*)opt_default }, "generic catch all option", "" },
	{ "i"				, OPT_BOOL							, { &dummy }, "read specified file", "input_file"},
	{ "codec"			, HAS_ARG							, { (void*)opt_codec}, "force decoder", "decoder" },
	{ NULL, },
};

static void sigterm_handler(int sig)
{
	exit(123);
}

SDL2Player::SDL2Player()
{

}

SDL2Player::~SDL2Player()
{

}

int SDL2Player::Init(const char *url)
{
	int errCode = 0;

	avcodec_register_all();
#if CONFIG_AVDEVICE
	avdevice_register_all();
#endif
#if CONFIG_AVFILTER
	avfilter_register_all();
#endif
	av_register_all();
	avformat_network_init();

	signal(SIGINT , sigterm_handler); /* Interrupt (ANSI).    */
	signal(SIGTERM, sigterm_handler); /* Termination (ANSI).  */

	input_filename = url;

	int opt_argc=0;
	char **opt_argv=NULL;

	parse_options(NULL, opt_argc, opt_argv, options, opt_input_file);

	return errCode;
}