
// HuajiaoRecordDownloadDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CHuajiaoRecordDownloadDlg 对话框
class CHuajiaoRecordDownloadDlg : public CDialogEx
{
// 构造
public:
	CHuajiaoRecordDownloadDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HUAJIAORECORDDOWNLOAD_DIALOG };

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
	CEdit m_cURL;
	CListCtrl m_cState;

public:
	afx_msg void OnBnClickedButtonDownload();

public:
	static void DownloadTask(CHuajiaoRecordDownloadDlg *dlg, base::string16 url);
};
