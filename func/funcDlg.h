// funcDlg.h : 头文件
//

#pragma once
#include "base\poly.h"
#include "afxwin.h"


// CfuncDlg 对话框
class CfuncDlg : public CDialog
{
// 构造
public:
	CfuncDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FUNC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
    // 函数式
    Poly poly;
public:
    CEdit polyedit;
    // 出错信息或计算结果列表
    CListBox msglist;
    afx_msg void OnBnClickedOk();
private:
    void show_dump(void);
public:
    CButton setvars;
    CButton calc;
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnEnChangeEdit1();
};
