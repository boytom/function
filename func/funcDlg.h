// funcDlg.h : ͷ�ļ�
//

#pragma once
#include "base\poly.h"
#include "afxwin.h"


// CfuncDlg �Ի���
class CfuncDlg : public CDialog
{
// ����
public:
	CfuncDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FUNC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
    // ����ʽ
    Poly poly;
public:
    CEdit polyedit;
    // ������Ϣ��������б�
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
