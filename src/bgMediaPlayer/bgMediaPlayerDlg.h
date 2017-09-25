
// bgMediaPlayerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include "bgMediaDecoder/bgMediaDecoder.h"
#include "SDL.h"


// CbgMediaPlayerDlg 对话框
class CbgMediaPlayerDlg : public CDialogEx , public bgMediaDecoderCallback
{
// 构造
public:
	CbgMediaPlayerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BGMEDIAPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	virtual void VideoSizeCallback(int width, int height);
	virtual void MediaDecoderCallback(void* frame_data, int frame_data_len);

public:
	bgMediaDecoder *decoder;

public:
	SDL_Window *sdl_window;
	SDL_Renderer *sdl_renderer;
	SDL_Texture *sdl_texture;

public:
	int video_width;
	int video_height;
	int frame_index;
	CEdit m_cMediaUrl;
	afx_msg void OnBnClickedButtonBrowser();
	afx_msg void OnBnClickedButtonPlay();
};
