
// HuajiaoRecordDownloadDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CHuajiaoRecordDownloadDlg �Ի���
class CHuajiaoRecordDownloadDlg : public CDialogEx
{
// ����
public:
	CHuajiaoRecordDownloadDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HUAJIAORECORDDOWNLOAD_DIALOG };

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
	CEdit m_cURL;
	CListCtrl m_cState;

public:
	afx_msg void OnBnClickedButtonDownload();

public:
	static void DownloadTask(CHuajiaoRecordDownloadDlg *dlg, base::string16 url);
};
