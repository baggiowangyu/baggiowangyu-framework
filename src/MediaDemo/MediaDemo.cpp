// MediaDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#define __STDC_CONSTANT_MACROS

#include "base/at_exit.h"
#include "base/message_loop.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/file_util.h"
#include "base/strings/sys_string_conversions.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

#include <iostream>

std::string GetCodecNameFromCodecId(AVCodecID id)
{
	std::string codec_name;
	switch (id)
	{
	case AV_CODEC_ID_MPEG1VIDEO:
		codec_name = "MPEG1VIDEO";
		break;
	case AV_CODEC_ID_MPEG2VIDEO:
		codec_name = "MPEG2VIDEO";
		break;
	case AV_CODEC_ID_MPEG2VIDEO_XVMC:
		codec_name = "MPEG2VIDEO_XVMC";
		break;
	case AV_CODEC_ID_H261:
		codec_name = "H261";
		break;
	case AV_CODEC_ID_H263:
		codec_name = "H263";
		break;
	case AV_CODEC_ID_RV10:
		codec_name = "RV10";
		break;
	case AV_CODEC_ID_RV20:
		codec_name = "RV20";
		break;
	case AV_CODEC_ID_MJPEG:
		codec_name = "MJPEG";
		break;
	case AV_CODEC_ID_MJPEGB:
		codec_name = "MJPEGB";
		break;
	case AV_CODEC_ID_LJPEG:
		codec_name = "LJPEG";
		break;
	case AV_CODEC_ID_SP5X:
		codec_name = "SP5X";
		break;
	case AV_CODEC_ID_JPEGLS:
		codec_name = "JPEGLS";
		break;
	case AV_CODEC_ID_MPEG4:
		codec_name = "MPEG4";
		break;
	case AV_CODEC_ID_RAWVIDEO:
		codec_name = "RAWVIDEO";
		break;
	case AV_CODEC_ID_MSMPEG4V1:
		codec_name = "MSMPEG4V1";
		break;
	case AV_CODEC_ID_MSMPEG4V2:
		codec_name = "MSMPEG4V2";
		break;
	case AV_CODEC_ID_MSMPEG4V3:
		codec_name = "MSMPEG4V3";
		break;
	case AV_CODEC_ID_WMV1:
		codec_name = "WMV1";
		break;
	case AV_CODEC_ID_WMV2:
		codec_name = "WMV2";
		break;
	case AV_CODEC_ID_H263P:
		codec_name = "H263P";
		break;
	case AV_CODEC_ID_H263I:
		codec_name = "H263I";
		break;
	case AV_CODEC_ID_FLV1:
		codec_name = "FLV1";
		break;
	case AV_CODEC_ID_SVQ1:
		codec_name = "SVQ1";
		break;
	case AV_CODEC_ID_SVQ3:
		codec_name = "SVQ3";
		break;
	case AV_CODEC_ID_DVVIDEO:
		codec_name = "DVVIDEO";
		break;
	case AV_CODEC_ID_HUFFYUV:
		codec_name = "HUFFYUV";
		break;
	case AV_CODEC_ID_CYUV:
		codec_name = "CYUV";
		break;
	case AV_CODEC_ID_H264:
		codec_name = "H264";
		break;
	case AV_CODEC_ID_INDEO3:
		codec_name = "INDEO3";
		break;
	case AV_CODEC_ID_VP3:
		codec_name = "VP3";
		break;
	case AV_CODEC_ID_THEORA:
		codec_name = "THEORA";
		break;
	case AV_CODEC_ID_ASV1:
		codec_name = "ASV1";
		break;
	case AV_CODEC_ID_ASV2:
		codec_name = "ASV2";
		break;
	case AV_CODEC_ID_FFV1:
		codec_name = "FFV1";
		break;
	case AV_CODEC_ID_4XM:
		codec_name = "4XM";
		break;
	case AV_CODEC_ID_VCR1:
		codec_name = "VCR1";
		break;
	case AV_CODEC_ID_CLJR:
		codec_name = "CLJR";
		break;
	case AV_CODEC_ID_MDEC:
		codec_name = "MDEC";
		break;
	case AV_CODEC_ID_ROQ:
		codec_name = "ROQ";
		break;
	case AV_CODEC_ID_INTERPLAY_VIDEO:
		codec_name = "INTERPLAY_VIDEO";
		break;
	case AV_CODEC_ID_XAN_WC3:
		codec_name = "XAN_WC3";
		break;
	case AV_CODEC_ID_XAN_WC4:
		codec_name = "XAN_WC4";
		break;
	case AV_CODEC_ID_RPZA:
		codec_name = "RPZA";
		break;
	case AV_CODEC_ID_CINEPAK:
		codec_name = "CINEPAK";
		break;
	case AV_CODEC_ID_WS_VQA:
		codec_name = "WS_VQA";
		break;
	case AV_CODEC_ID_MSRLE:
		codec_name = "MSRLE";
		break;
	case AV_CODEC_ID_MSVIDEO1:
		codec_name = "MSVIDEO1";
		break;
	case AV_CODEC_ID_IDCIN:
		codec_name = "IDCIN";
		break;
	case AV_CODEC_ID_8BPS:
		codec_name = "8BPS";
		break;
	case AV_CODEC_ID_SMC:
		codec_name = "SMC";
		break;
	case AV_CODEC_ID_FLIC:
		codec_name = "FLIC";
		break;
	case AV_CODEC_ID_TRUEMOTION1:
		codec_name = "TRUEMOTION1";
		break;
	case AV_CODEC_ID_VMDVIDEO:
		codec_name = "VMDVIDEO";
		break;
	case AV_CODEC_ID_MSZH:
		codec_name = "MSZH";
		break;
	case AV_CODEC_ID_ZLIB:
		codec_name = "ZLIB";
		break;
	case AV_CODEC_ID_QTRLE:
		codec_name = "QTRLE";
		break;
	case AV_CODEC_ID_TSCC:
		codec_name = "TSCC";
		break;
	case AV_CODEC_ID_ULTI:
		codec_name = "ULTI";
		break;
	case AV_CODEC_ID_QDRAW:
		codec_name = "QDRAW";
		break;
	case AV_CODEC_ID_VIXL:
		codec_name = "VIXL";
		break;
	case AV_CODEC_ID_QPEG:
		codec_name = "QPEG";
		break;
	case AV_CODEC_ID_PNG:
		codec_name = "PNG";
		break;
	case AV_CODEC_ID_PPM:
		codec_name = "PPM";
		break;
	case AV_CODEC_ID_PBM:
		codec_name = "PBM";
		break;
	case AV_CODEC_ID_PGM:
		codec_name = "PGM";
		break;
	case AV_CODEC_ID_PGMYUV:
		codec_name = "PGMYUV";
		break;
	case AV_CODEC_ID_PAM:
		codec_name = "PAM";
		break;
	case AV_CODEC_ID_FFVHUFF:
		codec_name = "FFVHUFF";
		break;
	case AV_CODEC_ID_RV30:
		codec_name = "RV30";
		break;
	case AV_CODEC_ID_RV40:
		codec_name = "RV40";
		break;
	case AV_CODEC_ID_VC1:
		codec_name = "VC1";
		break;
	case AV_CODEC_ID_WMV3:
		codec_name = "WMV3";
		break;
	case AV_CODEC_ID_LOCO:
		codec_name = "LOCO";
		break;
	case AV_CODEC_ID_WNV1:
		codec_name = "WNV1";
		break;
	case AV_CODEC_ID_AASC:
		codec_name = "AASC";
		break;
	case AV_CODEC_ID_INDEO2:
		codec_name = "INDEO2";
		break;
	case AV_CODEC_ID_FRAPS:
		codec_name = "FRAPS";
		break;
	case AV_CODEC_ID_TRUEMOTION2:
		codec_name = "TRUEMOTION2";
		break;
	case AV_CODEC_ID_BMP:
		codec_name = "BMP";
		break;
	case AV_CODEC_ID_CSCD:
		codec_name = "CSCD";
		break;
	case AV_CODEC_ID_MMVIDEO:
		codec_name = "MMVIDEO";
		break;
	case AV_CODEC_ID_ZMBV:
		codec_name = "ZMBV";
		break;
	case AV_CODEC_ID_AVS:
		codec_name = "AVS";
		break;
	case AV_CODEC_ID_SMACKVIDEO:
		codec_name = "SMACKVIDEO";
		break;
	case AV_CODEC_ID_NUV:
		codec_name = "NUV";
		break;
	case AV_CODEC_ID_KMVC:
		codec_name = "KMVC";
		break;
	case AV_CODEC_ID_FLASHSV:
		codec_name = "FLASHSV";
		break;
	case AV_CODEC_ID_CAVS:
		codec_name = "CAVS";
		break;
	case AV_CODEC_ID_JPEG2000:
		codec_name = "JPEG2000";
		break;
	case AV_CODEC_ID_VMNC:
		codec_name = "VMNC";
		break;
	case AV_CODEC_ID_VP5:
		codec_name = "VP5";
		break;
	case AV_CODEC_ID_VP6:
		codec_name = "VP6";
		break;
	case AV_CODEC_ID_VP6F:
		codec_name = "VP6F";
		break;
	case AV_CODEC_ID_TARGA:
		codec_name = "TARGA";
		break;
	case AV_CODEC_ID_DSICINVIDEO:
		codec_name = "DSICINVIDEO";
		break;
	case AV_CODEC_ID_TIERTEXSEQVIDEO:
		codec_name = "TIERTEXSEQVIDEO";
		break;
	case AV_CODEC_ID_TIFF:
		codec_name = "TIFF";
		break;
	case AV_CODEC_ID_GIF:
		codec_name = "GIF";
		break;
	case AV_CODEC_ID_DXA:
		codec_name = "DXA";
		break;
	case AV_CODEC_ID_DNXHD:
		codec_name = "DNXHD";
		break;
	case AV_CODEC_ID_THP:
		codec_name = "THP";
		break;
	case AV_CODEC_ID_SGI:
		codec_name = "SGI";
		break;
	case AV_CODEC_ID_C93:
		codec_name = "C93";
		break;
	case AV_CODEC_ID_BETHSOFTVID:
		codec_name = "BETHSOFTVID";
		break;
	case AV_CODEC_ID_PTX:
		codec_name = "PTX";
		break;
	case AV_CODEC_ID_TXD:
		codec_name = "TXD";
		break;
	case AV_CODEC_ID_VP6A:
		codec_name = "VP6A";
		break;
	case AV_CODEC_ID_AMV:
		codec_name = "AMV";
		break;
	case AV_CODEC_ID_VB:
		codec_name = "VB";
		break;
	case AV_CODEC_ID_PCX:
		codec_name = "PCX";
		break;
	case AV_CODEC_ID_SUNRAST:
		codec_name = "SUNRAST";
		break;
	case AV_CODEC_ID_INDEO4:
		codec_name = "INDEO4";
		break;
	case AV_CODEC_ID_INDEO5:
		codec_name = "INDEO5";
		break;
	case AV_CODEC_ID_MIMIC:
		codec_name = "MIMIC";
		break;
	case AV_CODEC_ID_RL2:
		codec_name = "RL2";
		break;
	case AV_CODEC_ID_ESCAPE124:
		codec_name = "ESCAPE124";
		break;
	case AV_CODEC_ID_DIRAC:
		codec_name = "DIRAC";
		break;
	case AV_CODEC_ID_BFI:
		codec_name = "BFI";
		break;
	case AV_CODEC_ID_CMV:
		codec_name = "CMV";
		break;
	case AV_CODEC_ID_MOTIONPIXELS:
		codec_name = "MOTIONPIXELS";
		break;
	case AV_CODEC_ID_TGV:
		codec_name = "TGV";
		break;
	case AV_CODEC_ID_TGQ:
		codec_name = "TGQ";
		break;
	case AV_CODEC_ID_TQI:
		codec_name = "TQI";
		break;
	case AV_CODEC_ID_AURA:
		codec_name = "AURA";
		break;
	case AV_CODEC_ID_AURA2:
		codec_name = "AURA2";
		break;
	case AV_CODEC_ID_V210X:
		codec_name = "V210X";
		break;
	case AV_CODEC_ID_TMV:
		codec_name = "TMV";
		break;
	case AV_CODEC_ID_V210:
		codec_name = "V210";
		break;
	case AV_CODEC_ID_DPX:
		codec_name = "DPX";
		break;
	case AV_CODEC_ID_MAD:
		codec_name = "MAD";
		break;
	case AV_CODEC_ID_FRWU:
		codec_name = "FRWU";
		break;
	case AV_CODEC_ID_FLASHSV2:
		codec_name = "FLASHSV2";
		break;
	case AV_CODEC_ID_CDGRAPHICS:
		codec_name = "CDGRAPHICS";
		break;
	case AV_CODEC_ID_R210:
		codec_name = "R210";
		break;
	case AV_CODEC_ID_ANM:
		codec_name = "ANM";
		break;
	case AV_CODEC_ID_BINKVIDEO:
		codec_name = "BINKVIDEO";
		break;
	case AV_CODEC_ID_IFF_ILBM:
		codec_name = "IFF_ILBM";
		break;
	case AV_CODEC_ID_KGV1:
		codec_name = "KGV1";
		break;
	case AV_CODEC_ID_YOP:
		codec_name = "YOP";
		break;
	case AV_CODEC_ID_VP8:
		codec_name = "VP8";
		break;
	case AV_CODEC_ID_PICTOR:
		codec_name = "PICTOR";
		break;
	case AV_CODEC_ID_ANSI:
		codec_name = "ANSI";
		break;
	case AV_CODEC_ID_A64_MULTI:
		codec_name = "A64_MULTI";
		break;
	case AV_CODEC_ID_A64_MULTI5:
		codec_name = "A64_MULTI5";
		break;
	case AV_CODEC_ID_R10K:
		codec_name = "R10K";
		break;
	case AV_CODEC_ID_MXPEG:
		codec_name = "MXPEG";
		break;
	case AV_CODEC_ID_LAGARITH:
		codec_name = "LAGARITH";
		break;
	case AV_CODEC_ID_PRORES:
		codec_name = "PRORES";
		break;
	case AV_CODEC_ID_JV:
		codec_name = "JV";
		break;
	case AV_CODEC_ID_DFA:
		codec_name = "DFA";
		break;
	case AV_CODEC_ID_WMV3IMAGE:
		codec_name = "WMV3IMAGE";
		break;
	case AV_CODEC_ID_VC1IMAGE:
		codec_name = "VC1IMAGE";
		break;
	case AV_CODEC_ID_UTVIDEO:
		codec_name = "UTVIDEO";
		break;
	case AV_CODEC_ID_BMV_VIDEO:
		codec_name = "BMV_VIDEO";
		break;
	case AV_CODEC_ID_VBLE:
		codec_name = "VBLE";
		break;
	case AV_CODEC_ID_DXTORY:
		codec_name = "DXTORY";
		break;
	case AV_CODEC_ID_V410:
		codec_name = "V410";
		break;
	case AV_CODEC_ID_XWD:
		codec_name = "XWD";
		break;
	case AV_CODEC_ID_CDXL:
		codec_name = "CDXL";
		break;
	case AV_CODEC_ID_XBM:
		codec_name = "XBM";
		break;
	case AV_CODEC_ID_ZEROCODEC:
		codec_name = "ZEROCODEC";
		break;
	case AV_CODEC_ID_MSS1:
		codec_name = "MSS1";
		break;
	case AV_CODEC_ID_MSA1:
		codec_name = "MSA1";
		break;
	case AV_CODEC_ID_TSCC2:
		codec_name = "TSCC2";
		break;
	case AV_CODEC_ID_MTS2:
		codec_name = "MTS2";
		break;
	case AV_CODEC_ID_CLLC:
		codec_name = "CLLC";
		break;
	case AV_CODEC_ID_MSS2:
		codec_name = "MSS2";
		break;
	case AV_CODEC_ID_VP9:
		codec_name = "VP9";
		break;
	case AV_CODEC_ID_AIC:
		codec_name = "AIC";
		break;
	case AV_CODEC_ID_ESCAPE130:
		codec_name = "ESCAPE130";
		break;
	case AV_CODEC_ID_G2M:
		codec_name = "G2M";
		break;
	case AV_CODEC_ID_WEBP:
		codec_name = "WEBP";
		break;
	case AV_CODEC_ID_HNM4_VIDEO:
		codec_name = "HNM4_VIDEO";
		break;
	case AV_CODEC_ID_HEVC:
		codec_name = "HEVC";
		break;
	case AV_CODEC_ID_FIC:
		codec_name = "FIC";
		break;
	case AV_CODEC_ID_ALIAS_PIX:
		codec_name = "ALIAS_PIX";
		break;
	case AV_CODEC_ID_BRENDER_PIX:
		codec_name = "BRENDER_PIX";
		break;
	case AV_CODEC_ID_PAF_VIDEO:
		codec_name = "PAF_VIDEO";
		break;
	case AV_CODEC_ID_EXR:
		codec_name = "EXR";
		break;
	case AV_CODEC_ID_VP7:
		codec_name = "VP7";
		break;
	case AV_CODEC_ID_SANM:
		codec_name = "SANM";
		break;
	case AV_CODEC_ID_SGIRLE:
		codec_name = "SGIRLE";
		break;
	case AV_CODEC_ID_MVC1:
		codec_name = "MVC1";
		break;
	case AV_CODEC_ID_MVC2:
		codec_name = "MVC2";
		break;
	case AV_CODEC_ID_HQX:
		codec_name = "HQX";
		break;
	case AV_CODEC_ID_TDSC:
		codec_name = "TDSC";
		break;
	case AV_CODEC_ID_HQ_HQA:
		codec_name = "HQ_HQA";
		break;
	case AV_CODEC_ID_HAP:
		codec_name = "HAP";
		break;
	case AV_CODEC_ID_DDS:
		codec_name = "DDS";
		break;
	case AV_CODEC_ID_DXV:
		codec_name = "DXV";
		break;
	case AV_CODEC_ID_SCREENPRESSO:
		codec_name = "SCREENPRESSO";
		break;
	case AV_CODEC_ID_RSCC:
		codec_name = "RSCC";
		break;
	case AV_CODEC_ID_Y41P:
		codec_name = "Y41P";
		break;
	case AV_CODEC_ID_AVRP:
		codec_name = "AVRP";
		break;
	case AV_CODEC_ID_012V:
		codec_name = "012V";
		break;
	case AV_CODEC_ID_AVUI:
		codec_name = "AVUI";
		break;
	case AV_CODEC_ID_AYUV:
		codec_name = "AYUV";
		break;
	case AV_CODEC_ID_TARGA_Y216:
		codec_name = "TARGA_Y216";
		break;
	case AV_CODEC_ID_V308:
		codec_name = "V308";
		break;
	case AV_CODEC_ID_V408:
		codec_name = "V408";
		break;
	case AV_CODEC_ID_YUV4:
		codec_name = "YUV4";
		break;
	case AV_CODEC_ID_AVRN:
		codec_name = "AVRN";
		break;
	case AV_CODEC_ID_CPIA:
		codec_name = "CPIA";
		break;
	case AV_CODEC_ID_XFACE:
		codec_name = "XFACE";
		break;
	case AV_CODEC_ID_SNOW:
		codec_name = "SNOW";
		break;
	case AV_CODEC_ID_SMVJPEG:
		codec_name = "SMVJPEG";
		break;
	case AV_CODEC_ID_APNG:
		codec_name = "APNG";
		break;
	case AV_CODEC_ID_DAALA:
		codec_name = "DAALA";
		break;
	case AV_CODEC_ID_CFHD:
		codec_name = "CFHD";
		break;
	case AV_CODEC_ID_TRUEMOTION2RT:
		codec_name = "TRUEMOTION2RT";
		break;
	case AV_CODEC_ID_M101:
		codec_name = "M101";
		break;
	case AV_CODEC_ID_MAGICYUV:
		codec_name = "MAGICYUV";
		break;
	case AV_CODEC_ID_SHEERVIDEO:
		codec_name = "SHEERVIDEO";
		break;
	case AV_CODEC_ID_YLC:
		codec_name = "YLC";
		break;
	case AV_CODEC_ID_PSD:
		codec_name = "PSD";
		break;
	case AV_CODEC_ID_PIXLET:
		codec_name = "PIXLET";
		break;
	case AV_CODEC_ID_SPEEDHQ:
		codec_name = "SPEEDHQ";
		break;
	case AV_CODEC_ID_FMVC:
		codec_name = "FMVC";
		break;
	case AV_CODEC_ID_SCPR:
		codec_name = "SCPR";
		break;
	case AV_CODEC_ID_CLEARVIDEO:
		codec_name = "CLEARVIDEO";
		break;
	case AV_CODEC_ID_XPM:
		codec_name = "XPM";
		break;
	case AV_CODEC_ID_AV1:
		codec_name = "AV1";
		break;
	case AV_CODEC_ID_PCM_S16LE:
		codec_name = "PCM_S16LE";
		break;
	case AV_CODEC_ID_PCM_S16BE:
		codec_name = "PCM_S16BE";
		break;
	case AV_CODEC_ID_PCM_U16LE:
		codec_name = "PCM_U16LE";
		break;
	case AV_CODEC_ID_PCM_U16BE:
		codec_name = "PCM_U16BE";
		break;
	case AV_CODEC_ID_PCM_S8:
		codec_name = "PCM_S8";
		break;
	case AV_CODEC_ID_PCM_U8:
		codec_name = "PCM_U8";
		break;
	case AV_CODEC_ID_PCM_MULAW:
		codec_name = "PCM_MULAW";
		break;
	case AV_CODEC_ID_PCM_ALAW:
		codec_name = "PCM_ALAW";
		break;
	case AV_CODEC_ID_PCM_S32LE:
		codec_name = "PCM_S32LE";
		break;
	case AV_CODEC_ID_PCM_S32BE:
		codec_name = "PCM_S32BE";
		break;
	case AV_CODEC_ID_PCM_U32LE:
		codec_name = "PCM_U32LE";
		break;
	case AV_CODEC_ID_PCM_U32BE:
		codec_name = "PCM_U32BE";
		break;
	case AV_CODEC_ID_PCM_S24LE:
		codec_name = "PCM_S24LE";
		break;
	case AV_CODEC_ID_PCM_S24BE:
		codec_name = "PCM_S24BE";
		break;
	case AV_CODEC_ID_PCM_U24LE:
		codec_name = "PCM_U24LE";
		break;
	case AV_CODEC_ID_PCM_U24BE:
		codec_name = "PCM_U24BE";
		break;
	case AV_CODEC_ID_PCM_S24DAUD:
		codec_name = "PCM_S24DAUD";
		break;
	case AV_CODEC_ID_PCM_ZORK:
		codec_name = "PCM_ZORK";
		break;
	case AV_CODEC_ID_PCM_S16LE_PLANAR:
		codec_name = "PCM_S16LE_PLANAR";
		break;
	case AV_CODEC_ID_PCM_DVD:
		codec_name = "PCM_DVD";
		break;
	case AV_CODEC_ID_PCM_F32BE:
		codec_name = "PCM_F32BE";
		break;
	case AV_CODEC_ID_PCM_F32LE:
		codec_name = "PCM_F32LE";
		break;
	case AV_CODEC_ID_PCM_F64BE:
		codec_name = "PCM_F64BE";
		break;
	case AV_CODEC_ID_PCM_F64LE:
		codec_name = "PCM_F64LE";
		break;
	case AV_CODEC_ID_PCM_BLURAY:
		codec_name = "PCM_BLURAY";
		break;
	case AV_CODEC_ID_PCM_LXF:
		codec_name = "PCM_LXF";
		break;
	case AV_CODEC_ID_S302M:
		codec_name = "S302M";
		break;
	case AV_CODEC_ID_PCM_S8_PLANAR:
		codec_name = "PCM_S8_PLANAR";
		break;
	case AV_CODEC_ID_PCM_S24LE_PLANAR:
		codec_name = "PCM_S24LE_PLANAR";
		break;
	case AV_CODEC_ID_PCM_S32LE_PLANAR:
		codec_name = "PCM_S32LE_PLANAR";
		break;
	case AV_CODEC_ID_PCM_S16BE_PLANAR:
		codec_name = "PCM_S16BE_PLANAR";
		break;
	case AV_CODEC_ID_PCM_S64LE:
		codec_name = "PCM_S64LE";
		break;
	case AV_CODEC_ID_PCM_S64BE:
		codec_name = "PCM_S64BE";
		break;
	case AV_CODEC_ID_PCM_F16LE:
		codec_name = "PCM_F16LE";
		break;
	case AV_CODEC_ID_PCM_F24LE:
		codec_name = "PCM_F24LE";
		break;
	case AV_CODEC_ID_ADPCM_IMA_QT:
		codec_name = "ADPCM_IMA_QT";
		break;
	case AV_CODEC_ID_ADPCM_IMA_WAV:
		codec_name = "ADPCM_IMA_WAV";
		break;
	case AV_CODEC_ID_ADPCM_IMA_DK3:
		codec_name = "ADPCM_IMA_DK3";
		break;
	case AV_CODEC_ID_ADPCM_IMA_DK4:
		codec_name = "ADPCM_IMA_DK4";
		break;
	case AV_CODEC_ID_ADPCM_IMA_WS:
		codec_name = "ADPCM_IMA_WS";
		break;
	case AV_CODEC_ID_ADPCM_IMA_SMJPEG:
		codec_name = "ADPCM_IMA_SMJPEG";
		break;
	case AV_CODEC_ID_ADPCM_MS:
		codec_name = "ADPCM_MS";
		break;
	case AV_CODEC_ID_ADPCM_4XM:
		codec_name = "ADPCM_4XM";
		break;
	case AV_CODEC_ID_ADPCM_XA:
		codec_name = "ADPCM_XA";
		break;
	case AV_CODEC_ID_ADPCM_ADX:
		codec_name = "ADPCM_ADX";
		break;
	case AV_CODEC_ID_ADPCM_EA:
		codec_name = "ADPCM_EA";
		break;
	case AV_CODEC_ID_ADPCM_G726:
		codec_name = "ADPCM_G726";
		break;
	case AV_CODEC_ID_ADPCM_CT:
		codec_name = "ADPCM_CT";
		break;
	case AV_CODEC_ID_ADPCM_SWF:
		codec_name = "ADPCM_SWF";
		break;
	case AV_CODEC_ID_ADPCM_YAMAHA:
		codec_name = "ADPCM_YAMAHA";
		break;
	case AV_CODEC_ID_ADPCM_SBPRO_4:
		codec_name = "ADPCM_SBPRO_4";
		break;
	case AV_CODEC_ID_ADPCM_SBPRO_3:
		codec_name = "ADPCM_SBPRO_3";
		break;
	case AV_CODEC_ID_ADPCM_SBPRO_2:
		codec_name = "ADPCM_SBPRO_2";
		break;
	case AV_CODEC_ID_ADPCM_THP:
		codec_name = "ADPCM_THP";
		break;
	case AV_CODEC_ID_ADPCM_IMA_AMV:
		codec_name = "ADPCM_IMA_AMV";
		break;
	case AV_CODEC_ID_ADPCM_EA_R1:
		codec_name = "ADPCM_EA_R1";
		break;
	case AV_CODEC_ID_ADPCM_EA_R3:
		codec_name = "ADPCM_EA_R3";
		break;
	case AV_CODEC_ID_ADPCM_EA_R2:
		codec_name = "ADPCM_EA_R2";
		break;
	case AV_CODEC_ID_ADPCM_IMA_EA_SEAD:
		codec_name = "ADPCM_IMA_EA_SEAD";
		break;
	case AV_CODEC_ID_ADPCM_IMA_EA_EACS:
		codec_name = "ADPCM_IMA_EA_EACS";
		break;
	case AV_CODEC_ID_ADPCM_EA_XAS:
		codec_name = "ADPCM_EA_XAS";
		break;
	case AV_CODEC_ID_ADPCM_EA_MAXIS_XA:
		codec_name = "ADPCM_EA_MAXIS_XA";
		break;
	case AV_CODEC_ID_ADPCM_IMA_ISS:
		codec_name = "ADPCM_IMA_ISS";
		break;
	case AV_CODEC_ID_ADPCM_G722:
		codec_name = "ADPCM_G722";
		break;
	case AV_CODEC_ID_ADPCM_IMA_APC:
		codec_name = "ADPCM_IMA_APC";
		break;
	case AV_CODEC_ID_ADPCM_VIMA:
		codec_name = "ADPCM_VIMA";
		break;
	case AV_CODEC_ID_ADPCM_AFC:
		codec_name = "ADPCM_AFC";
		break;
	case AV_CODEC_ID_ADPCM_IMA_OKI:
		codec_name = "ADPCM_IMA_OKI";
		break;
	case AV_CODEC_ID_ADPCM_DTK:
		codec_name = "ADPCM_DTK";
		break;
	case AV_CODEC_ID_ADPCM_IMA_RAD:
		codec_name = "ADPCM_IMA_RAD";
		break;
	case AV_CODEC_ID_ADPCM_G726LE:
		codec_name = "ADPCM_G726LE";
		break;
	case AV_CODEC_ID_ADPCM_THP_LE:
		codec_name = "ADPCM_THP_LE";
		break;
	case AV_CODEC_ID_ADPCM_PSX:
		codec_name = "ADPCM_PSX";
		break;
	case AV_CODEC_ID_ADPCM_AICA:
		codec_name = "ADPCM_AICA";
		break;
	case AV_CODEC_ID_ADPCM_IMA_DAT4:
		codec_name = "ADPCM_IMA_DAT4";
		break;
	case AV_CODEC_ID_ADPCM_MTAF:
		codec_name = "ADPCM_MTAF";
		break;
	case AV_CODEC_ID_AMR_NB:
		codec_name = "AMR_NB";
		break;
	case AV_CODEC_ID_AMR_WB:
		codec_name = "AMR_WB";
		break;
	case AV_CODEC_ID_RA_144:
		codec_name = "RA_144";
		break;
	case AV_CODEC_ID_RA_288:
		codec_name = "RA_288";
		break;
	case AV_CODEC_ID_ROQ_DPCM:
		codec_name = "ROQ_DPCM";
		break;
	case AV_CODEC_ID_INTERPLAY_DPCM:
		codec_name = "INTERPLAY_DPCM";
		break;
	case AV_CODEC_ID_XAN_DPCM:
		codec_name = "XAN_DPCM";
		break;
	case AV_CODEC_ID_SOL_DPCM:
		codec_name = "SOL_DPCM";
		break;
	case AV_CODEC_ID_SDX2_DPCM:
		codec_name = "SDX2_DPCM";
		break;
	case AV_CODEC_ID_MP2:
		codec_name = "MP2";
		break;
	case AV_CODEC_ID_MP3:
		codec_name = "MP3";
		break;
	case AV_CODEC_ID_AAC:
		codec_name = "AAC";
		break;
	case AV_CODEC_ID_AC3:
		codec_name = "AC3";
		break;
	case AV_CODEC_ID_DTS:
		codec_name = "DTS";
		break;
	case AV_CODEC_ID_VORBIS:
		codec_name = "VORBIS";
		break;
	case AV_CODEC_ID_DVAUDIO:
		codec_name = "DVAUDIO";
		break;
	case AV_CODEC_ID_WMAV1:
		codec_name = "WMAV1";
		break;
	case AV_CODEC_ID_WMAV2:
		codec_name = "WMAV2";
		break;
	case AV_CODEC_ID_MACE3:
		codec_name = "MACE3";
		break;
	case AV_CODEC_ID_MACE6:
		codec_name = "MACE6";
		break;
	case AV_CODEC_ID_VMDAUDIO:
		codec_name = "VMDAUDIO";
		break;
	case AV_CODEC_ID_FLAC:
		codec_name = "FLAC";
		break;
	case AV_CODEC_ID_MP3ADU:
		codec_name = "MP3ADU";
		break;
	case AV_CODEC_ID_MP3ON4:
		codec_name = "MP3ON4";
		break;
	case AV_CODEC_ID_SHORTEN:
		codec_name = "SHORTEN";
		break;
	case AV_CODEC_ID_ALAC:
		codec_name = "ALAC";
		break;
	case AV_CODEC_ID_WESTWOOD_SND1:
		codec_name = "WESTWOOD_SND1";
		break;
	case AV_CODEC_ID_GSM:
		codec_name = "GSM";
		break;
	case AV_CODEC_ID_QDM2:
		codec_name = "QDM2";
		break;
	case AV_CODEC_ID_COOK:
		codec_name = "COOK";
		break;
	case AV_CODEC_ID_TRUESPEECH:
		codec_name = "TRUESPEECH";
		break;
	case AV_CODEC_ID_TTA:
		codec_name = "TTA";
		break;
	case AV_CODEC_ID_SMACKAUDIO:
		codec_name = "SMACKAUDIO";
		break;
	case AV_CODEC_ID_QCELP:
		codec_name = "QCELP";
		break;
	case AV_CODEC_ID_WAVPACK:
		codec_name = "WAVPACK";
		break;
	case AV_CODEC_ID_DSICINAUDIO:
		codec_name = "DSICINAUDIO";
		break;
	case AV_CODEC_ID_IMC:
		codec_name = "IMC";
		break;
	case AV_CODEC_ID_MUSEPACK7:
		codec_name = "MUSEPACK7";
		break;
	case AV_CODEC_ID_MLP:
		codec_name = "MLP";
		break;
	case AV_CODEC_ID_GSM_MS:
		codec_name = "GSM_MS";
		break;
	case AV_CODEC_ID_ATRAC3:
		codec_name = "ATRAC3";
		break;
	case AV_CODEC_ID_APE:
		codec_name = "APE";
		break;
	case AV_CODEC_ID_NELLYMOSER:
		codec_name = "NELLYMOSER";
		break;
	case AV_CODEC_ID_MUSEPACK8:
		codec_name = "MUSEPACK8";
		break;
	case AV_CODEC_ID_SPEEX:
		codec_name = "SPEEX";
		break;
	case AV_CODEC_ID_WMAVOICE:
		codec_name = "WMAVOICE";
		break;
	case AV_CODEC_ID_WMAPRO:
		codec_name = "WMAPRO";
		break;
	case AV_CODEC_ID_WMALOSSLESS:
		codec_name = "WMALOSSLESS";
		break;
	case AV_CODEC_ID_ATRAC3P:
		codec_name = "ATRAC3P";
		break;
	case AV_CODEC_ID_EAC3:
		codec_name = "EAC3";
		break;
	case AV_CODEC_ID_SIPR:
		codec_name = "SIPR";
		break;
	case AV_CODEC_ID_MP1:
		codec_name = "MP1";
		break;
	case AV_CODEC_ID_TWINVQ:
		codec_name = "TWINVQ";
		break;
	case AV_CODEC_ID_TRUEHD:
		codec_name = "TRUEHD";
		break;
	case AV_CODEC_ID_MP4ALS:
		codec_name = "MP4ALS";
		break;
	case AV_CODEC_ID_ATRAC1:
		codec_name = "ATRAC1";
		break;
	case AV_CODEC_ID_BINKAUDIO_RDFT:
		codec_name = "BINKAUDIO_RDFT";
		break;
	case AV_CODEC_ID_BINKAUDIO_DCT:
		codec_name = "BINKAUDIO_DCT";
		break;
	case AV_CODEC_ID_AAC_LATM:
		codec_name = "AAC_LATM";
		break;
	case AV_CODEC_ID_QDMC:
		codec_name = "QDMC";
		break;
	case AV_CODEC_ID_CELT:
		codec_name = "CELT";
		break;
	case AV_CODEC_ID_G723_1:
		codec_name = "G723_1";
		break;
	case AV_CODEC_ID_G729:
		codec_name = "G729";
		break;
	case AV_CODEC_ID_8SVX_EXP:
		codec_name = "8SVX_EXP";
		break;
	case AV_CODEC_ID_8SVX_FIB:
		codec_name = "ID_8SVX_FIB";
		break;
	case AV_CODEC_ID_BMV_AUDIO:
		codec_name = "BMV_AUDIO";
		break;
	case AV_CODEC_ID_RALF:
		codec_name = "RALF";
		break;
	case AV_CODEC_ID_IAC:
		codec_name = "IAC";
		break;
	case AV_CODEC_ID_ILBC:
		codec_name = "ILBC";
		break;
	case AV_CODEC_ID_OPUS:
		codec_name = "OPUS";
		break;
	case AV_CODEC_ID_COMFORT_NOISE:
		codec_name = "COMFORT_NOISE";
		break;
	case AV_CODEC_ID_TAK:
		codec_name = "TAK";
		break;
	case AV_CODEC_ID_METASOUND:
		codec_name = "METASOUND";
		break;
	case AV_CODEC_ID_PAF_AUDIO:
		codec_name = "PAF_AUDIO";
		break;
	case AV_CODEC_ID_ON2AVC:
		codec_name = "ON2AVC";
		break;
	case AV_CODEC_ID_DSS_SP:
		codec_name = "DSS_SP";
		break;
	case AV_CODEC_ID_FFWAVESYNTH:
		codec_name = "FFWAVESYNTH";
		break;
	case AV_CODEC_ID_SONIC:
		codec_name = "SONIC";
		break;
	case AV_CODEC_ID_SONIC_LS:
		codec_name = "SONIC_LS";
		break;
	case AV_CODEC_ID_EVRC:
		codec_name = "EVRC";
		break;
	case AV_CODEC_ID_SMV:
		codec_name = "SMV";
		break;
	case AV_CODEC_ID_DSD_LSBF:
		codec_name = "DSD_LSBF";
		break;
	case AV_CODEC_ID_DSD_MSBF:
		codec_name = "DSD_MSBF";
		break;
	case AV_CODEC_ID_DSD_LSBF_PLANAR:
		codec_name = "DSD_LSBF_PLANAR";
		break;
	case AV_CODEC_ID_DSD_MSBF_PLANAR:
		codec_name = "DSD_MSBF_PLANAR";
		break;
	case AV_CODEC_ID_4GV:
		codec_name = "4GV";
		break;
	case AV_CODEC_ID_INTERPLAY_ACM:
		codec_name = "INTERPLAY_ACM";
		break;
	case AV_CODEC_ID_XMA1:
		codec_name = "XMA1";
		break;
	case AV_CODEC_ID_XMA2:
		codec_name = "XMA2";
		break;
	case AV_CODEC_ID_DST:
		codec_name = "DST";
		break;
	case AV_CODEC_ID_ATRAC3AL:
		codec_name = "ATRAC3AL";
		break;
	case AV_CODEC_ID_ATRAC3PAL:
		codec_name = "ATRAC3PAL";
		break;
	case AV_CODEC_ID_DVD_SUBTITLE:
		codec_name = "DVD_SUBTITLE";
		break;
	case AV_CODEC_ID_DVB_SUBTITLE:
		codec_name = "DVB_SUBTITLE";
		break;
	case AV_CODEC_ID_TEXT:
		codec_name = "TEXT";
		break;
	case AV_CODEC_ID_XSUB:
		codec_name = "XSUB";
		break;
	case AV_CODEC_ID_SSA:
		codec_name = "SSA";
		break;
	case AV_CODEC_ID_MOV_TEXT:
		codec_name = "MOV_TEXT";
		break;
	case AV_CODEC_ID_HDMV_PGS_SUBTITLE:
		codec_name = "HDMV_PGS_SUBTITLE";
		break;
	case AV_CODEC_ID_DVB_TELETEXT:
		codec_name = "DVB_TELETEXT";
		break;
	case AV_CODEC_ID_SRT:
		codec_name = "SRT";
		break;
	case AV_CODEC_ID_MICRODVD:
		codec_name = "MICRODVD";
		break;
	case AV_CODEC_ID_EIA_608:
		codec_name = "EIA_608";
		break;
	case AV_CODEC_ID_JACOSUB:
		codec_name = "JACOSUB";
		break;
	case AV_CODEC_ID_SAMI:
		codec_name = "SAMI";
		break;
	case AV_CODEC_ID_REALTEXT:
		codec_name = "REALTEXT";
		break;
	case AV_CODEC_ID_STL:
		codec_name = "STL";
		break;
	case AV_CODEC_ID_SUBVIEWER1:
		codec_name = "SUBVIEWER1";
		break;
	case AV_CODEC_ID_SUBVIEWER:
		codec_name = "SUBVIEWER";
		break;
	case AV_CODEC_ID_SUBRIP:
		codec_name = "SUBRIP";
		break;
	case AV_CODEC_ID_WEBVTT:
		codec_name = "WEBVTT";
		break;
	case AV_CODEC_ID_MPL2:
		codec_name = "MPL2";
		break;
	case AV_CODEC_ID_VPLAYER:
		codec_name = "VPLAYER";
		break;
	case AV_CODEC_ID_PJS:
		codec_name = "PJS";
		break;
	case AV_CODEC_ID_ASS:
		codec_name = "ASS";
		break;
	case AV_CODEC_ID_HDMV_TEXT_SUBTITLE:
		codec_name = "HDMV_TEXT_SUBTITLE";
		break;
	case AV_CODEC_ID_TTF:
		codec_name = "TTF";
		break;
	case AV_CODEC_ID_SCTE_35:
		codec_name = "SCTE_35";
		break;
	case AV_CODEC_ID_BINTEXT:
		codec_name = "BINTEXT";
		break;
	case AV_CODEC_ID_XBIN:
		codec_name = "XBIN";
		break;
	case AV_CODEC_ID_IDF:
		codec_name = "IDF";
		break;
	case AV_CODEC_ID_OTF:
		codec_name = "OTF";
		break;
	case AV_CODEC_ID_SMPTE_KLV:
		codec_name = "SMPTE_KLV";
		break;
	case AV_CODEC_ID_DVD_NAV:
		codec_name = "DVD_NAV";
		break;
	case AV_CODEC_ID_TIMED_ID3:
		codec_name = "TIMED_ID3";
		break;
	case AV_CODEC_ID_BIN_DATA:
		codec_name = "BIN_DATA";
		break;
	case AV_CODEC_ID_PROBE:
		codec_name = "PROBE";
		break;
	case AV_CODEC_ID_MPEG2TS:
		codec_name = "MPEG2TS";
		break;
	case AV_CODEC_ID_MPEG4SYSTEMS:
		codec_name = "MPEG4SYSTEMS";
		break;
	case AV_CODEC_ID_FFMETADATA:
		codec_name = "FFMETADATA";
		break;
	case AV_CODEC_ID_WRAPPED_AVFRAME:
		codec_name = "WRAPPED_AVFRAME";
		break;
	default:
		codec_name = "unknown";
		break;
	}

	return codec_name;
}


void EnumInputOutputFormat()
{
	// 寻找第一个支持的输入格式
	std::cout<<"枚举所有支持的输入格式"<<std::endl;
	LOG(INFO)<<"枚举所有支持的输入格式";
	AVInputFormat *input_format = av_iformat_next(nullptr);

	while(input_format != nullptr)
	{
		// 输出相关信息
		std::cout<<"AVInputFormat::name : "<<input_format->name<<std::endl;
		LOG(INFO)<<"AVInputFormat::name : "<<input_format->name;
		std::cout<<"AVInputFormat::long_name : "<<input_format->long_name<<std::endl;
		LOG(INFO)<<"AVInputFormat::long_name : "<<input_format->long_name;
		std::cout<<"-----------------------------------"<<std::endl;
		LOG(INFO)<<"-----------------------------------";

		// 寻找下一个支持的输入格式
		input_format = av_iformat_next(input_format);
	}

	std::cout<<"枚举所有支持的输出格式"<<std::endl;
	LOG(INFO)<<"枚举所有支持的输出格式";
	AVOutputFormat *output_format = av_oformat_next(nullptr);

	while(output_format != nullptr)
	{
		// 输出相关信息
		std::cout<<"AVOutputFormat::name : "<<output_format->name<<std::endl;
		LOG(INFO)<<"AVOutputFormat::name : "<<output_format->name;
		std::cout<<"AVOutputFormat::long_name : "<<output_format->long_name<<std::endl;
		LOG(INFO)<<"AVOutputFormat::long_name : "<<output_format->long_name;
		std::cout<<"-----------------------------------"<<std::endl;
		LOG(INFO)<<"-----------------------------------";

		// 寻找下一个支持的输入格式
		output_format = av_oformat_next(output_format);
	}
}

void GetMediaInfo(std::string media_path_)
{
	base::FilePath media_path(base::SysMultiByteToWide(media_path_, CP_ACP));

	AVFormatContext *input_context = nullptr;
	int errCode = avformat_open_input(&input_context, media_path_.c_str(), nullptr, nullptr);
	if (0 != errCode)
	{
		std::cout<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode;
		return ;
	}

	errCode = avformat_find_stream_info(input_context, NULL);
	if (errCode < 0)
	{
		std::cout<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode;
		return ;
	}

	std::cout<<"======= 媒体信息 ======="<<std::endl;
	int number_of_streams = input_context->nb_streams;
	for (int index = 0; index < number_of_streams; ++index)
	{
		AVStream *stream = input_context->streams[index];
		AVCodecContext *avcodec_context = stream->codec;
		std::string media_type;
		switch (avcodec_context->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			media_type = "Video";
			break;
		case AVMEDIA_TYPE_AUDIO:
			media_type = "Audio";
			break;
		default:
			break;
		}

		std::cout<<"\n媒体流 - "<<index<<" 信息："<<std::endl;
		if (avcodec_context->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			std::cout<<"\n\t媒体类型："<<media_type.c_str()<<"\n\tCodec 名称："<<GetCodecNameFromCodecId(avcodec_context->codec_id).c_str()<<"\n\t比特率："<<avcodec_context->bit_rate<<
				"\n\t图像尺寸（宽*高）："<<avcodec_context->width<<"*"<<avcodec_context->height<<"\n\t流尺寸（宽*高）"<<avcodec_context->coded_width<<"*"<<avcodec_context->coded_height<<"\n";

		} else if (avcodec_context->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			std::cout<<"\n\t媒体类型："<<media_type.c_str()<<"\n\tCodec 名称："<<GetCodecNameFromCodecId(avcodec_context->codec_id).c_str()<<"\n\t比特率："<<avcodec_context->bit_rate<<
				avcodec_context->coded_height<<"\n\t音频采样率："<<avcodec_context->sample_rate<<"\n\t音轨数量："<<avcodec_context->channels<<"\n\t每个音轨的帧数量："<<avcodec_context->frame_size<<"\n";
		}
	}
}

void ConvertMediaFormat(std::string media_path_, std::string target_format_)
{
	// 处理输入媒体文件
	AVFormatContext *input_context = nullptr;
	int errCode = avformat_open_input(&input_context, media_path_.c_str(), nullptr, nullptr);
	if (0 != errCode)
	{
		std::cout<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode;
		return ;
	}

	errCode = avformat_find_stream_info(input_context, NULL);
	if (errCode < 0)
	{
		std::cout<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode;
		return ;
	}

	// 处理输出媒体文件
	std::string media_path_output = media_path_.append(".").append(target_format_);
	AVFormatContext *output_context = nullptr;
	avformat_alloc_output_context2(&output_context, nullptr, nullptr, media_path_output.c_str());
	if (!output_context)
	{
		std::cout<<"创建转换媒体文件 "<<media_path_output.c_str()<<" 失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"创建转换媒体文件 "<<media_path_output.c_str()<<" 失败！错误码："<<errCode;
		return ;
	}

	std::cout<<"======= 媒体信息 ======="<<std::endl;
	int number_of_streams = input_context->nb_streams;
	for (int index = 0; index < number_of_streams; ++index)
	{
		AVStream *input_stream = input_context->streams[index];
		AVStream *output_stream = avformat_new_stream(output_context, input_stream->codec->codec);

		if (!output_stream)
		{
			std::cout<<"创建输出流对象失败！"<<std::endl;
			break;
		}

		errCode = avcodec_copy_context(output_stream->codec, input_stream->codec);
		if (errCode < 0)
		{
			std::cout<<"复制编码上下文失败！"<<std::endl;
			break;
		}

		output_stream->codec->codec_tag = 0;
		if (output_context->oformat->flags & AVFMT_GLOBALHEADER)
		{
			output_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
		}
	}

	// 写文件头
	errCode = avformat_write_header(output_context, nullptr);
	if (errCode < 0)
	{
		std::cout<<"输出文件，写文件头时出错！"<<std::endl;
		return ;
	}

	// 逐帧写入
	int frame_index = 0;
	while (true)
	{
		AVPacket *av_packet = nullptr;

		// 读取一个带编码的帧
		errCode = av_read_frame(input_context, av_packet);
		if (errCode < 0)
			break;

		AVStream *input_stream = input_context->streams[av_packet->stream_index];
		AVStream *output_stream = output_context->streams[av_packet->stream_index];

		// 转换PTS/DTS
		// PTS: AVPacket中的显示时间戳
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	base::AtExitManager _at_exit_manager;
	base::MessageLoop _message_loop;

#ifdef WIN32
	LPCTSTR commandline = GetCommandLine();
#endif
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, NULL);

	base::FilePath current_dir;
	bool result = file_util::GetCurrentDirectory(&current_dir);
	base::FilePath logging_file = current_dir.Append(_T("log"));

	// 不存在这个目录，则创建之
	if (!file_util::PathExists(logging_file))
		result = file_util::CreateDirectoryW(logging_file);

	// 得到今天的日期
	base::Time today = base::Time::Now();

	logging_file = logging_file.Append(_T("log.txt"));

	// 初始化日志
	result = logging::InitLogging(logging_file.LossyDisplayName().c_str(), 
		logging::LOG_ONLY_TO_FILE, 
		logging::DONT_LOCK_LOG_FILE, 
		logging::APPEND_TO_OLD_LOG_FILE, 
		logging::DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS);

	logging::SetMinLogLevel(logging::LOG_INFO);

	av_register_all();
	avformat_network_init();

	while (true)
	{
		std::cout<<"========FFmepg学习教程========"<<std::endl;
		std::cout<<"1. 枚举当前支持的输入输出格式"<<std::endl;
		std::cout<<"2. 查询视音频文件基本信息"<<std::endl;
		std::cout<<"3. 视音频文件格式转换(不改变编码)"<<std::endl;
		std::cout<<"4. 视音频文件编码转换（不改变格式）"<<std::endl;
		std::cout<<"5. 视音频文件格式转换(改变编码)"<<std::endl;
		std::cout<<"6. 视音频文件编码转换（改变格式）"<<std::endl;
		std::cout<<"7. "<<std::endl;
		std::cout<<"8. "<<std::endl;
		std::cout<<"9. "<<std::endl;
		std::cout<<"0. 退出"<<std::endl;
		std::cout<<"请输入选项：";

		char input_data[512] = {0};
		std::cin.clear();
		std::cin>>input_data;

		if (_stricmp(input_data, "1") == 0){
			EnumInputOutputFormat();
		} else if (_stricmp(input_data, "2") == 0){
			std::cout<<"请输入视频文件路径：";
			char media_path[512] = {0};
			std::cin.clear();
			std::cin>>media_path;

			std::string path = media_path;
			GetMediaInfo(path);
		} else if (_stricmp(input_data, "3") == 0){
			std::cout<<"请输入视频文件路径：";
			char media_path[512] = {0};
			std::cin.clear();
			std::cin>>media_path;

			GetMediaInfo(path);

			std::cout<<"请输入转换目标格式：";
			char format_name[512] = {0};
			std::cin.clear();
			std::cin>>format_name;
		} else if (_stricmp(input_data, "4") == 0){
		} else if (_stricmp(input_data, "5") == 0){
		} else if (_stricmp(input_data, "6") == 0){
		} else if (_stricmp(input_data, "7") == 0){
		} else if (_stricmp(input_data, "8") == 0){
		} else if (_stricmp(input_data, "9") == 0){
		} else if (_stricmp(input_data, "0") == 0){
		}

		std::cout<<"\n\n"<<std::endl;
	}
	

	return 0;
}

