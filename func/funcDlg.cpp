// funcDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "func.h"
#include "funcDlg.h"
#include ".\funcdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CfuncDlg �Ի���



CfuncDlg::CfuncDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CfuncDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CfuncDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, polyedit);
    DDX_Control(pDX, IDC_LIST1, msglist);
    DDX_Control(pDX, IDC_BUTTON1, setvars);
    DDX_Control(pDX, IDC_BUTTON2, calc);
}

BEGIN_MESSAGE_MAP(CfuncDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
    ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()


// CfuncDlg ��Ϣ�������

BOOL CfuncDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CfuncDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
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
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CfuncDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CfuncDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    //OnOK();
    // ���ú������ʽ
    CString polystr;
    char *p;
    bool enable = false;

    polyedit.GetWindowText(polystr);
    p = polystr.GetBuffer();// x1+x2+(99+)+0
    enable = poly.set_poly(p, polystr.GetLength());
    setvars.EnableWindow(enable && poly.nvars() != 0);
    calc.EnableWindow(enable && poly.nvars() == 0);
    show_dump();
    polystr.ReleaseBuffer();
}

void CfuncDlg::show_dump(void)
{
    // ��ʾ��ϸ��Ϣ
    char *p, *q;

    msglist.ResetContent();
    q = (char *)poly.dump().c_str() - 1;
    while(q) {
        p = strchr(++q, '\n');
        if(p)
            *p = '\0';
        msglist.AddString(q);
        q = p;
    }
}

void CfuncDlg::OnBnClickedButton1()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    // ���ñ���
    FILE *fp;
    int i = 0;
    double *vars;
    CFileDialog file_dialog(TRUE);//, "*.*\0\0", NULL, OFN_HIDEREADONLY, "*.*\0\0");

    if(poly.nvars() == 0)
        return;
    if(poly.nvars() == 0 || file_dialog.DoModal() != IDOK)
        return;
    if((fp = fopen((LPCTSTR)file_dialog.GetPathName(), "rt")) == NULL) {
        MessageBox((LPCTSTR)file_dialog.GetPathName(), "���ļ�����", MB_OK);
        return;
    }
    vars = new double[poly.nvars()];
    memset(vars, 0, sizeof(double) * poly.nvars());

    while(i < poly.nvars()) {
        if(fscanf(fp, "%lf", &vars[i]) == 1)
            i++;
        else
            break;
    }
    if(i < poly.nvars() || !poly.set_vars(vars, poly.nvars())) {
        MessageBox("���ñ�������", "���ñ�������", MB_OK);
        calc.EnableWindow(false);
    }
    else {
        calc.EnableWindow(true);
        show_dump();
    }
    delete[] vars;
    fclose(fp);
}

void CfuncDlg::OnBnClickedButton2()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    // ���㺯�����ʽ��ֵ
    char buf[128] = {0};

    sprintf(buf, "��������%lf", poly.calc());
    msglist.AddString(buf);
}

void CfuncDlg::OnEnChangeEdit1()
{
    // TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
    // ���͸�֪ͨ��������д CDialog::OnInitDialog()
    // ���������� CRichEditCtrl().SetEventMask()��
    // ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

    // TODO:  �ڴ���ӿؼ�֪ͨ����������
    setvars.EnableWindow(false);
    calc.EnableWindow(false);
}
