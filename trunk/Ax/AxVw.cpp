// AxVw.cpp : CAxVw クラスの実装
//

#include "stdafx.h"
#include "resource.h"
#include "AxVw.h"

#undef max
#undef min

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAxVw

CAxVw::CAxVw() {
	m_pdfdoc = NULL;
}

CAxVw::~CAxVw() {
	UnloadPDF();
}

BEGIN_MESSAGE_MAP(CAxVw, CWnd)
	ON_WM_PAINT()
	ON_COMMAND_EX_RANGE(ID_PAGE_UP, ID_ZOOM_MINUS, OnPageUp)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PAGE_UP, ID_ZOOM_MINUS, OnUpdatePageUp)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CAxVw メッセージ ハンドラ

BOOL CAxVw::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.style |= WS_HSCROLL|WS_VSCROLL;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
	// reinterpret_cast<HBRUSH>(COLOR_WINDOW+1)

	return TRUE;
}

CRect CAxVw::GetPageRect(int top) {
	Page *page = NULL;
	if (m_pdfdoc != NULL)
		page = m_pdfdoc->getPage(1 +top);
	if (page != NULL) {
		PDFRectangle *prc = page->getMediaBox();
		switch (page->getRotate()) {
			case 90:
			case 270:
				return CRect(
					int(prc->y1),
					int(prc->x1),
					int(prc->y2),
					int(prc->x2)
					);
			default:
				return CRect(
					int(prc->x1),
					int(prc->y1),
					int(prc->x2),
					int(prc->y2)
					);
		}
	}
	return CRect();
}

void CAxVw::OnPaint() 
{
	CPaintDC dc(this);
	
	CRect rc;
	GetClientRect(&rc);

	if (m_pdfdoc == NULL) {
		CBrush br1;
		br1.CreateStockObject(WHITE_BRUSH);
		dc.FillRect(rc, &br1);
		return;
	}

	CRect rcPage = GetPageRect(m_top);

	double scale = 1;

	int ox = m_hsc.nPos;
	int oy = m_vsc.nPos;

	switch (m_ft) {
		case ftW:
			{
				double cxPg = rcPage.Width();
				if (cxPg != 0) {
					double f = rc.Width() / cxPg;
					scale = f;
				}
				ox = 0;
				break;
			}
		case ftWH:
			{
				double cxPg = rcPage.Width();
				double cyPg = rcPage.Height();
				if (cxPg != 0 && cyPg != 0 && (rc.Width() < cxPg || rc.Height() < cyPg)) {
					double fx = rc.Width() / cxPg;
					double fy = rc.Height() / cyPg;
					double f = std::min(fx, fy);
					scale = f;
				}
				ox = oy = 0;
				break;
			}
		case ftZoom:
			{
				scale = m_scalexy;
				break;
			}
	}

	ox = std::max<int>(m_hsc.nMin, std::min<int>(m_hsc.nMax - m_hsc.nPage, ox));
	oy = std::max<int>(m_vsc.nMin, std::min<int>(m_vsc.nMax - m_vsc.nPage, oy));

	double dpi = 72 * scale;

	rcPage.right = rcPage.left + LONG(rcPage.Width() * scale);
	rcPage.bottom = rcPage.top + LONG(rcPage.Height() * scale);

	SplashColor paperColor;
	paperColor[0] = 255;
	paperColor[1] = 255;
	paperColor[2] = 255;
	std::auto_ptr<SplashOutputDev> splashOut;
	splashOut.reset(new SplashOutputDev(splashModeRGB8, 4, gFalse, paperColor));
	splashOut->startDoc(m_pdfdoc->getXRef());

	int slw = std::max(1, std::min(rc.Width(), rcPage.Width() - ox));
	int slh = std::max(1, std::min(rc.Height(), rcPage.Height() - oy));
	m_pdfdoc->displayPageSlice(
		splashOut.get(), 
		1 +m_top, dpi, dpi, 
		0,
		gTrue, gFalse, gFalse,
		ox, oy, slw, slh
		);
	SplashBitmap *bitmap = splashOut->getBitmap();
	int pcx = bitmap->getWidth();
	int pcy = bitmap->getHeight();
	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = pcx;
	bi.bmiHeader.biHeight = -pcy;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biPlanes = 1;

	for (int y=0; y<pcy; y++) {
		SplashColorPtr pRow = bitmap->getDataPtr() + bitmap->getRowSize() * y;
		for (int x=0; x<pcx; x++, pRow += 3) {
			//Guchar tmp = pRow[0];
			//pRow[2] = pRow[0];
			//pRow[0] = tmp;
		}
	}

	SetDIBitsToDevice(
		dc, 0, 0, pcx, pcy, 0, 0, 0, pcy, bitmap->getDataPtr(), &bi, DIB_RGB_COLORS
		);
	dc.ExcludeClipRect(0, 0, pcx, pcy);

	CBrush br2;
	br2.CreateStockObject(GRAY_BRUSH);
	dc.FillRect(rc, &br2);
}

void CAxVw::UnloadPDF() {
	if (m_pdfdoc != NULL) {
		delete m_pdfdoc;
		m_pdfdoc = NULL;
	}

	m_strUrl.Empty();
}

HRESULT CAxVw::LoadPDF(LPCTSTR newVal) {
	UnloadPDF();

	DWORD atts = GetFileAttributes(newVal);
	if (atts == 0xFFFFFFFFU || (atts & FILE_ATTRIBUTE_DIRECTORY) != 0)
		return E_FAIL;

	CStringW str = newVal;

	m_pdfdoc = new PDFDoc(const_cast<wchar_t *>(static_cast<LPCWSTR>(str)), str.GetLength());
	if (!m_pdfdoc->isOk()) {
		UnloadPDF();
		return E_FAIL;
	}

	m_top = 0;
	m_cx = 0;
	m_ft = ftW;
	m_scalexy = 1;

	m_cx = m_pdfdoc->getNumPages();

	SetPage(0, true);

	m_strUrl = newVal;
	return S_OK;
}

void CAxVw::OnUpdatePageUp(CCmdUI *pUI) {
	switch (pUI->m_nID) {
		case ID_PAGE_UP:
			pUI->Enable(m_top != 0);
			break;
		case ID_PAGE_DOWN:
			pUI->Enable(m_top < m_cx -1);
			break;
		case ID_PAGE_FITW:
			pUI->SetRadio(m_ft == ftW);
			break;
		case ID_PAGE_FITWH:
			pUI->SetRadio(m_ft == ftWH);
			break;
	}
}

BOOL CAxVw::OnPageUp(UINT nID) {
	switch (nID) {
		case ID_PAGE_UP:
			MovePage(-1);
			return true;
		case ID_PAGE_DOWN:
			MovePage(1);
			return true;
		case ID_PAGE_FITW:
			Setft(ftW);
			return true;
		case ID_PAGE_FITWH:
			Setft(ftWH);
			return true;
		case ID_ZOOM_PLUS:
			Zoom(1.5);
			return true;
		case ID_ZOOM_MINUS:
			Zoom(1 / 1.5);
			return true;
	}

	return false;
}

int CAxVw::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ZeroMemory(&m_hsc, sizeof(m_hsc));
	ZeroMemory(&m_vsc, sizeof(m_vsc));

	m_hsc.cbSize = sizeof(m_hsc);
	m_hsc.fMask = SIF_DISABLENOSCROLL|SIF_POS|SIF_RANGE|SIF_PAGE;

	m_vsc.cbSize = sizeof(m_hsc);
	m_vsc.fMask = SIF_DISABLENOSCROLL|SIF_POS|SIF_RANGE|SIF_PAGE;

	UpdScroll();
	return 0;
}

bool CAxVw::MovePage(int iDelta, bool force) {
	int i = m_top + iDelta;
	i = std::min(i, m_cx -1);
	i = std::max(0, i);

	return SetPage(i, force);
}

bool CAxVw::SetPage(int i, bool force) {
	if (i == m_top && !force)
		return false;

	m_top = i;
	m_hsc.nPos = 0;
	m_vsc.nPos = 0;
	if (m_hWnd != NULL) {
		Invalidate();
		UpdScroll();
	}
	return true;
}

void CAxVw::Setft(FitMode ft) {
	switch (ft) {
		case ftW:
		case ftWH:
		case ftZoom:
			m_ft = ft;
			Invalidate();
			UpdScroll();
	}
}

void CAxVw::Zoom(double fDelta) {
	m_ft = ftZoom;
	m_scalexy *= fDelta;
	Invalidate();
	UpdScroll();
}

void CAxVw::UpdScroll() {
	CRect rc;
	GetClientRect(rc);

	m_hsc.nMax = 0;
	m_vsc.nMax = 0;

	CRect rcPg = GetPageRect(m_top);

	if (!rcPg.IsRectEmpty()) {
		switch (m_ft) {
			case ftZoom:
				m_hsc.nMax = (int)(rcPg.Width() * m_scalexy);
				m_vsc.nMax = (int)(rcPg.Height() * m_scalexy);
				break;
			case ftW:
				m_vsc.nMax = (int)(rc.Width() / (float)rcPg.Width() * rcPg.Height());
				break;
			case ftWH:
				break;
		}
	}

	m_hsc.nPage = rc.Width();
	SetScrollInfo(SB_HORZ, &m_hsc);

	m_vsc.nPage = rc.Height();
	SetScrollInfo(SB_VERT, &m_vsc);
}

void CAxVw::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	UpdScroll();
}

void CAxVw::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	SCROLLINFO &si = m_hsc;
	const int nBar = SB_HORZ;

	int newPos = si.nPos;
	switch (nSBCode) {
		case SB_LEFT:
			newPos = si.nMin;
			break;
		case SB_RIGHT:
			newPos = si.nMax;
			break;
		case SB_LINELEFT:
			newPos--;
			break;
		case SB_LINERIGHT:
			newPos++;
			break;
		case SB_PAGELEFT:
			newPos -= si.nPage;
			break;
		case SB_PAGERIGHT:
			newPos += si.nPage;
			break;
		case SB_THUMBPOSITION:
			newPos = nPos;
			break;
		case SB_THUMBTRACK:
			newPos = nPos;
			break;
	}

	newPos = std::max(newPos, si.nMin);
	newPos = std::min(newPos, si.nMax - (int)si.nPage);

	if (newPos != si.nPos) {
		si.nPos = newPos;
		SetScrollInfo(nBar, &si);
		Invalidate();
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAxVw::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	SCROLLINFO &si = m_vsc;
	const int nBar = SB_VERT;

	int newPos = si.nPos;
	switch (nSBCode) {
		case SB_LEFT:
			newPos = si.nMin;
			break;
		case SB_RIGHT:
			newPos = si.nMax;
			break;
		case SB_LINELEFT:
			newPos--;
			break;
		case SB_LINERIGHT:
			newPos++;
			break;
		case SB_PAGELEFT:
			newPos -= si.nPage;
			break;
		case SB_PAGERIGHT:
			newPos += si.nPage;
			break;
		case SB_THUMBPOSITION:
			newPos = nPos;
			break;
		case SB_THUMBTRACK:
			newPos = nPos;
			break;
	}

	newPos = std::max(newPos, si.nMin);
	newPos = std::min(newPos, si.nMax - (int)si.nPage);

	if (newPos != si.nPos) {
		si.nPos = newPos;
		SetScrollInfo(nBar, &si);
		Invalidate();
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CAxVw::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	if (zDelta != 0) {
		SCROLLINFO &si = m_vsc;
		const int nBar = SB_VERT;

		int newPos = si.nPos - zDelta;

		newPos = std::max(newPos, si.nMin);
		newPos = std::min(newPos, si.nMax - (int)si.nPage);

		if (newPos != si.nPos) {
			si.nPos = newPos;
			SetScrollInfo(nBar, &si);
			Invalidate();
		}
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
