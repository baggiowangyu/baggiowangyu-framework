
// bgMediaPlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "bgMediaPlayer.h"
#include "bgMediaPlayerDlg.h"
#include "afxdialogex.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#ifdef __cplusplus
};
#endif

#include "SDL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CbgMediaPlayerDlg 对话框




CbgMediaPlayerDlg::CbgMediaPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CbgMediaPlayerDlg::IDD, pParent)
	, decoder(new bgMediaDecoder(this))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CbgMediaPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MEDIA_URL, m_cMediaUrl);
}

BEGIN_MESSAGE_MAP(CbgMediaPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, &CbgMediaPlayerDlg::OnBnClickedButtonBrowser)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CbgMediaPlayerDlg::OnBnClickedButtonPlay)
END_MESSAGE_MAP()


// CbgMediaPlayerDlg 消息处理程序

BOOL CbgMediaPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CWnd *pPlayerCWnd = GetDlgItem(IDC_STATIC_PLAYER_SCREEN);

	char variable[4096] = {0};
	sprintf_s(variable, 4096, "SDL_WINDOWID=0x%1x", pPlayerCWnd->GetSafeHwnd());

	//SDL_putenv()

	// SDL模块的初始化
	int errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (!errCode)
	{
		MessageBox(_T("Initialize SDL failed."), _T("Error"), MB_ICONERROR|MB_OK);
		return TRUE;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CbgMediaPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CbgMediaPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CbgMediaPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CbgMediaPlayerDlg::VideoSizeCallback(int width, int height)
{
	video_width = width;
	video_height = height;

	// SDL 设置视频宽高
	sdl_window = SDL_CreateWindow("bgMediaPlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, video_width, video_height, SDL_WINDOW_OPENGL);
	if (!sdl_window)
	{
		TRACE("Create SDL window failed.\n");
	}

	sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
	sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, video_width, video_height);
}

void CbgMediaPlayerDlg::MediaDecoderCallback(void* frame_data, int frame_data_len)
{
	AVFrame *av_frame = (AVFrame *)frame_data;

	SYSTEMTIME st;
	GetLocalTime(&st);
	TRACE("frame : %d ||| date-time : %d-%02d-%02d %02d:%02d:%02d.%03d\n", frame_index, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	++frame_index;

	// 这里不限制播放帧数的话，每秒可以处理高达400+帧的视音频数据
	// 这里我们调用SDL进行处理
}


void CbgMediaPlayerDlg::OnBnClickedButtonBrowser()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, 6UL, _T(""), this);
	INT_PTR ret = fileDlg.DoModal();

	if (ret == IDOK)
	{
		CString media_path = fileDlg.GetPathName();
		m_cMediaUrl.SetWindowText(media_path);
	}
}


void CbgMediaPlayerDlg::OnBnClickedButtonPlay()
{
	CString media_url_t;
	m_cMediaUrl.GetWindowText(media_url_t);

	frame_index = 0;

	base::FilePath media_path = base::FilePath::FromWStringHack(media_url_t.GetString());
	decoder->OpenMedia(media_path);

	decoder->StartDecode();
}
