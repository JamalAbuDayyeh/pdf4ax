// AxFrm.cpp : CAxFrame �N���X�̎���
//

#include "stdafx.h"
#include "resource.h"
#include "AxFrm.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAxFrame

#define MY_WM_UPDATE_UI (WM_APP+0x100)

BEGIN_MESSAGE_MAP(CAxFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_TIMER()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // �X�e�[�^�X ���C�� �C���W�P�[�^
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CAxFrame �R���X�g���N�V����/�f�X�g���N�V����

CAxFrame::CAxFrame()
{

}

CAxFrame::~CAxFrame()
{

}

int CAxFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL)) {
		return -1;
	}
	
	if (false
		|| !m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT)
		|| !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)
	) {
		return -1;
	}

	if (false
		|| !m_wndReBar.Create(this)
		|| !m_wndReBar.AddBar(&m_wndToolBar)
	) {
		return -1;      // �쐬�ł��܂���ł����B
	}

	if (false
		|| !m_wndStatusBar.Create(this)
		|| !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT))
	) {
		return -1;      // �쐬�ł��܂���ł����B
	}

	SetTimer(0x0100, 100, NULL);

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	//m_wndView.LoadPDF(_T("C:\\Documents and Settings\\KU\\My Documents\\PDFs\\�f�W�^���h���t�B���Y�o�[�W�����A�b�v.pdf.pdf"));

	return 0;
}

BOOL CAxFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//  �C�����Ă��������B

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.hMenu = NULL;
	return TRUE;
}


// CAxFrame �f�f

#ifdef _DEBUG
void CAxFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CAxFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CAxFrame ���b�Z�[�W �n���h��

void CAxFrame::OnSetFocus(CWnd* /*pOldWnd*/) {
	// �r���[ �E�B���h�E�Ƀt�H�[�J�X��^���܂��B
	m_wndView.SetFocus();
}

BOOL CAxFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return true;
	if (CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return true;

	return false;
}

void CAxFrame::PostNcDestroy() {
}

void CAxFrame::OnAppAbout() {
	CAboutDlg wndDlg;
	wndDlg.DoModal();
}

#define WM_IDLEUPDATECMDUI  0x0363  // wParam == bDisableIfNoHandler

void CAxFrame::OnTimer(UINT_PTR nIDEvent) {
	if (nIDEvent == 0x0100) {
		m_wndToolBar.SendMessage(WM_IDLEUPDATECMDUI, 1);
	}

	CFrameWnd::OnTimer(nIDEvent);
}
