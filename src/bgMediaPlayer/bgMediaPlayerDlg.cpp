
// bgMediaPlayerDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CbgMediaPlayerDlg �Ի���




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


// CbgMediaPlayerDlg ��Ϣ�������

BOOL CbgMediaPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CWnd *pPlayerCWnd = GetDlgItem(IDC_STATIC_PLAYER_SCREEN);

	char variable[4096] = {0};
	sprintf_s(variable, 4096, "SDL_WINDOWID=0x%1x", pPlayerCWnd->GetSafeHwnd());

	//SDL_putenv()

	// SDLģ��ĳ�ʼ��
	int errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (!errCode)
	{
		MessageBox(_T("Initialize SDL failed."), _T("Error"), MB_ICONERROR|MB_OK);
		return TRUE;
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CbgMediaPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CbgMediaPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CbgMediaPlayerDlg::VideoSizeCallback(int width, int height)
{
	video_width = width;
	video_height = height;

	// SDL ������Ƶ���
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

	// ���ﲻ���Ʋ���֡���Ļ���ÿ����Դ���ߴ�400+֡������Ƶ����
	// �������ǵ���SDL���д���
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
