// funcDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "func.h"
#include "funcDlg.h"
#include ".\funcdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CfuncDlg 对话框



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


// CfuncDlg 消息处理程序

BOOL CfuncDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CfuncDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CfuncDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CfuncDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    //OnOK();
    // 设置函数表达式
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
    // 显示详细信息
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
    // TODO: 在此添加控件通知处理程序代码
    // 设置变量
    FILE *fp;
    int i = 0;
    double *vars;
    CFileDialog file_dialog(TRUE);//, "*.*\0\0", NULL, OFN_HIDEREADONLY, "*.*\0\0");

    if(poly.nvars() == 0)
        return;
    if(poly.nvars() == 0 || file_dialog.DoModal() != IDOK)
        return;
    if((fp = fopen((LPCTSTR)file_dialog.GetPathName(), "rt")) == NULL) {
        MessageBox((LPCTSTR)file_dialog.GetPathName(), "打开文件出错", MB_OK);
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
        MessageBox("设置变量出错", "设置变量出错", MB_OK);
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
    // TODO: 在此添加控件通知处理程序代码
    // 计算函数表达式的值
    char buf[128] = {0};

    sprintf(buf, "计算结果：%lf", poly.calc());
    msglist.AddString(buf);
}

void CfuncDlg::OnEnChangeEdit1()
{
    // TODO:  如果该控件是 RICHEDIT 控件，则它将不会
    // 发送该通知，除非重写 CDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    setvars.EnableWindow(false);
    calc.EnableWindow(false);
}
