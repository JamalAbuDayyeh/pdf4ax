// AxFrm.cpp : CAxFrame �N���X�̎���
//

#include "stdafx.h"
#include "resource.h"
#include "AxFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAxFrame

IMPLEMENT_DYNAMIC(CAxFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CAxFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEACTIVATE()
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
	
//	if (false
//		|| !m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT)
//		|| !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)
//	) {
//		return -1;
//	}

//	if (false
//		|| !m_wndReBar.Create(this)
//		|| !m_wndReBar.AddBar(&m_wndToolBar)
//	) {
//		return -1;      // �쐬�ł��܂���ł����B
//	}

//	if (false
//		|| !m_wndStatusBar.Create(this)
//		|| !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT))
//	) {
//		return -1;      // �쐬�ł��܂���ł����B
//	}

//	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	//m_wndView.LoadPDF(_T("C:\\Documents and Settings\\KU\\My Documents\\PDFs\\�f�W�^���h���t�B���Y�o�[�W�����A�b�v.pdf.pdf"));
	//m_wndView.LoadPDF(_T("O:\\DL\\saitama.pdf"));
	m_wndView.LoadPDF(_T("O:\\DL\\map_tokyo.pdf"));
	//m_wndView.LoadPDF(_T("H:\\DL\\OpenGL\\GDC2003_OGL_ARBFragmentProgram.pdf"));
	//m_wndView.LoadPDF(_T("O:\\DL\\TIFF6.pdf"));

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

BOOL CAxFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// �r���[�ɍŏ��ɃR�}���h����������@���^���܂��B
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ����ȊO�̏ꍇ�́A����̏������s���܂��B
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

int CAxFrame::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) {
//	if (nHitTest == HTCLIENT) {
//		return MA_ACTIVATEANDEAT;
//	}

	return CFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
