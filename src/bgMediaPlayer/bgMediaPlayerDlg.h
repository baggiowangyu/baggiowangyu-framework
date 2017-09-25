
// bgMediaPlayerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

#include "bgMediaDecoder/bgMediaDecoder.h"
#include "SDL.h"


// CbgMediaPlayerDlg �Ի���
class CbgMediaPlayerDlg : public CDialogEx , public bgMediaDecoderCallback
{
// ����
public:
	CbgMediaPlayerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BGMEDIAPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
