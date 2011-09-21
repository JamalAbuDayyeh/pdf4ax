// AxVw.cpp : CAxVw �N���X�̎���
//

#include "stdafx.h"
#include "resource.h"
#include "AxRes.h"
#include "AxVw.h"

#undef max
#undef min

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SET_RENDERINF (WM_APP+0x0010)

// CPvMenu

CMutex s_lockpdf;

class CPvMenu : public CMenu {
	CPvRender &pv;
	int cxThumb;
public:

	CPvMenu(CPvRender &pv, int cxThumb = 150): pv(pv), cxThumb(cxThumb) {
	}

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
		DRAWITEMSTRUCT &di = *lpDrawItemStruct;

		CDC *pDC = CDC::FromHandle(di.hDC);
		if (pDC == NULL)
			return;

		bool fSel = 0 != (di.itemState & ODS_SELECTED);

		CRect rc = di.rcItem;

		COLORREF clrBk = GetSysColor(fSel ? COLOR_HIGHLIGHT : COLOR_WINDOW);
		CBrush br(clrBk);
		pDC->FillRect(rc, &br);

		rc.right = rc.left + 26;
		rc.DeflateRect(1, 1);
		int iPage = di.itemID - IDC_PAGE1;
		CString str; str.Format(_T("%d"), iPage + 1);

		pDC->SetTextColor(GetSysColor(fSel ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
		pDC->SetBkColor(clrBk);

		pDC->DrawText(str, rc, DT_SINGLELINE|DT_TOP|DT_LEFT);

		CBitmap *pic = pv.GetThumb(iPage, cxThumb);
		if (pic != NULL) {
			CDC dcMem;
			VERIFY(dcMem.CreateCompatibleDC(pDC));
			CBitmap *pOrg = dcMem.SelectObject(pic);
			pDC->BitBlt(rc.left +26, rc.top, cxThumb -2, cxThumb -2, &dcMem, 0, 0, SRCCOPY);
			dcMem.SelectObject(pOrg);
		}
	}
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) {
		lpMeasureItemStruct->itemHeight = cxThumb +2;
		lpMeasureItemStruct->itemWidth = cxThumb +26 +2;
	}
};

// CAxVw

CAxVw::CAxVw() {
	m_pdfdoc = NULL;
}

CAxVw::~CAxVw() {
	UnloadPDF();
}

#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif // WM_MOUSEHWHEEL

BEGIN_MESSAGE_MAP(CAxVw, CWnd)
	ON_WM_PAINT()
	ON_COMMAND_EX_RANGE(ID_PAGE_UP, ID_ZOOM_MINUS, OnPageUp)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PAGE_UP, ID_ZOOM_MINUS, OnUpdatePageUp)
	ON_COMMAND_EX_RANGE(IDC_MAG, IDC_FITWH, OnSelCmd)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_MAG, IDC_FITWH, OnUpdateSelCmd)
	ON_COMMAND_EX_RANGE(IDC_PAGE1, IDC_PAGE1000, OnPageSel)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_PAGE1, IDC_PAGE1000, OnUpdatePageSel)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSEHWHEEL, OnMouseHWheel)

	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_SET_RENDERINF, OnSetRenderInf)
END_MESSAGE_MAP()

// CAxVw ���b�Z�[�W �n���h��

BOOL CAxVw::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
	// reinterpret_cast<HBRUSH>(COLOR_WINDOW+1)

	return TRUE;
}

CRect CAxVw::GetPageRect(int top) const {
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

UINT DrawPDFProc(LPVOID lpv) {
	CSingleLock lck(&s_lockpdf);
	std::auto_ptr<CRenderInf> inf(reinterpret_cast<CRenderInf *>(lpv));

	SplashColor paperColor;
	paperColor[0] = 255;
	paperColor[1] = 255;
	paperColor[2] = 255;
	std::auto_ptr<SplashOutputDev> splashOut;
	splashOut.reset(new SplashOutputDev(splashModeRGB8, 4, gFalse, paperColor));
	splashOut->startDoc(inf->pdfdoc->getXRef());

	double dpi = inf->dpi;
	int iPage = inf->iPage;
	CSize sizeIn = inf->sizeIn;
	CRect rcOut = inf->rcPartial;
	if (sizeIn.cx * sizeIn.cy * 3 > 1024*1024*20) {
		inf->partial = true;
	}
	else {
		inf->partial = false;

		rcOut = CRect(CPoint(0, 0), inf->sizeIn);
	}

	inf->pdfdoc->displayPageSlice(
		splashOut.get(), 
		1 +iPage, dpi, dpi, 
		0,
		gTrue, gFalse, gFalse,
		rcOut.left, rcOut.top, rcOut.Width(), rcOut.Height()
		);

	lck.Unlock();

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
			Guchar tmp = pRow[2];
			pRow[2] = pRow[0];
			pRow[0] = tmp;
		}
	}

	inf->dpi = dpi;
	inf->splashOut = splashOut;
	inf->bi = bi;
	if (::PostMessage(inf->hwndCb, inf->nMsg, 0, reinterpret_cast<LPARAM>(inf.get()))) {
		inf.release();
	}
	else {

	}

	return 0;
}

void CAxVw::OnPaint() 
{
	CPaintDC dc(this);
	
	CRect rc = m_rcPaint;

	if (m_pdfdoc == NULL) {
		CBrush br1;
		br1.CreateStockObject(WHITE_BRUSH);
		dc.FillRect(rc, &br1);
	}
	else if (dc.RectVisible(m_rcPaint)) {
		CSize size = GetZoomedSize();
		int cx = size.cx;
		int cy = size.cy;

		int xp = -m_hsc.nPos;
		int yp = -m_vsc.nPos;

		if (cx < m_rcPaint.Width()) {
			xp = (m_rcPaint.Width() - cx) / 2;
		}
		if (cy < m_rcPaint.Height()) {
			yp = (m_rcPaint.Height() - cy) / 2;
		}

		int vx = std::max(0, -xp);
		int vy = std::max(0, -yp);

		int dx = std::max(0, xp - vx);
		int dy = std::max(0, yp - vy);

		double scale = Getzf();

		double dpi = 72 * scale;

		bool need = false;
		bool drawwip = false;
		bool clear = false;

		int tx = std::max(m_hsc.nPos, 0);
		int ty = std::max(m_vsc.nPos, 0);
		int tcx = std::min(cx, rc.Width());
		int tcy = std::min(cy, rc.Height());
		CRect rcPartial(tx, ty, tx+tcx, ty+tcy);

		if (m_renderPart.get() != NULL && m_renderPart->iPage == m_iPage && m_renderPart->rcPartial == rcPartial) {
			SplashBitmap *bitmap = m_renderPart->splashOut->getBitmap();
			int pcx = bitmap->getWidth();
			int pcy = bitmap->getHeight();

			int state = dc.SaveDC();
			dc.IntersectClipRect(m_rcPaint);
			dc.SetStretchBltMode(HALFTONE);
			dc.SetBrushOrg(xp, yp);
			SetDIBitsToDevice(
				dc, xp +rcPartial.left, yp +rcPartial.top, rcPartial.Width(), rcPartial.Height(), 0, 0, 0, rcPartial.Height(), bitmap->getDataPtr(), &m_renderPart->bi, DIB_RGB_COLORS
				);
			if (state != 0) dc.RestoreDC(state);
		}
		else if (m_renderAll.get() != NULL) {
			SplashBitmap *bitmap = m_renderAll->splashOut->getBitmap();
			int pcx = bitmap->getWidth();
			int pcy = bitmap->getHeight();

			int state = dc.SaveDC();
			dc.IntersectClipRect(m_rcPaint);
			dc.SetStretchBltMode(HALFTONE);
			dc.SetBrushOrg(xp, yp);
			StretchDIBits(
				dc, xp, yp, cx, cy, 0, 0, pcx, pcy, bitmap->getDataPtr(), &m_renderAll->bi, DIB_RGB_COLORS, SRCCOPY
				);
			if (state != 0) dc.RestoreDC(state);

			if (fabs(m_renderAll->dpi - dpi) > 1) {
				need = true;
			}
			if (m_renderAll->iPage != m_iPage) {
				need = true;
				drawwip = true;
			}
		}
		else {
			clear = true;
			need = true;
			drawwip = true;
		}

		if (need) {
			if (cntBGDraw == 0) {
				CRenderInf *inf = new CRenderInf();
				inf->sizeIn = size;
				inf->rcPartial = rcPartial;
				inf->iPage = m_iPage;
				inf->dpi = dpi;
				inf->pdfdoc = m_pdfdoc;
				inf->hwndCb = *this;
				inf->nMsg = WM_SET_RENDERINF;

				AfxBeginThread(DrawPDFProc, inf);
			}
			cntBGDraw++;
		}

		if (drawwip) {
			CRect rc(dx, dy, dx + cx, dy + cy);
			CRect rc2;
			if (rc2.IntersectRect(rc, m_rcPaint))
				rc = rc2;
			if (clear) {
				CBrush br0;
				br0.CreateStockObject(WHITE_BRUSH);
				dc.FillRect(rc, &br0);
			}
			else {
#if 0
				CDC dcMem;
				dcMem.CreateCompatibleDC(&dc);
				CBitmap *pOrg = dcMem.SelectObject(&m_bmMask10);
				for (int by = rc.top; by < rc.bottom; by += 64) {
					for (int bx = rc.left; bx < rc.right; bx += 64) {
						dc.BitBlt(bx, by, std::min((int)rc.right - bx, 64), std::min((int)rc.bottom - by, 64), &dcMem, 0, 0, SRCPAINT);
					}
				}
				dcMem.SelectObject(pOrg);
#endif
			}
			CFont font;
			font.CreateStockObject(DEFAULT_GUI_FONT);
			LOGFONT lf;
			if (0 != font.GetLogFont(&lf)) {
				lf.lfHeight *= 3;
				font.DeleteObject();
				font.CreateFontIndirect(&lf);
			}
			{
				CFont *pOrg = dc.SelectObject(&font);
				dc.DrawText(_T("���҂���������..."), rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
				dc.SelectObject(pOrg);
			}
		}

		int state = dc.SaveDC();
		dc.ExcludeClipRect(dx, dy, dx + cx, dy + cy);
		CBrush br;
		br.CreateStockObject(GRAY_BRUSH);
		dc.FillRect(m_rcPaint, &br);
		dc.RestoreDC(state);

		dc.ExcludeClipRect(m_rcPaint);
	}

	if (dc.RectVisible(m_rcPrev)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(&m_bmPrev);
		dc.BitBlt(m_rcPrev.left, m_rcPrev.top, m_rcPrev.Width(), m_rcPrev.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);
		dc.ExcludeClipRect(m_rcPrev);
	}
	if (dc.RectVisible(m_rcNext)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(&m_bmNext);
		dc.BitBlt(m_rcNext.left, m_rcNext.top, m_rcNext.Width(), m_rcNext.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);
		dc.ExcludeClipRect(m_rcNext);
	}
	if (dc.RectVisible(m_rcDisp)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(&m_bmPageDisp);
		dc.BitBlt(m_rcDisp.left, m_rcDisp.top, m_rcDisp.Width(), m_rcDisp.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);

		dc.SelectStockObject(DEFAULT_GUI_FONT);
		CString str;
		if (m_iPage < CntPages())
			str.Format(_T("%u/%u"), 1+m_iPage, CntPages());

		//COLORREF lastBkClr = dc.SetBkColor(GetSysColor(COLOR_WINDOW));
		UINT lastMode = dc.SetTextAlign(TA_CENTER|TA_TOP);
		CSize size =  dc.GetTextExtent(str);
		CPoint pt = m_rcDisp.CenterPoint() - CPoint(0, size.cy / 2);
		dc.ExtTextOut(pt.x, pt.y, ETO_CLIPPED, m_rcDisp, str, str.GetLength(), NULL);
		dc.SetTextAlign(lastMode);
		//dc.SetBkColor(lastBkClr);

		dc.ExcludeClipRect(m_rcDisp);
	}
	if (dc.RectVisible(m_rcAbout)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(&m_bmAbout);
		dc.BitBlt(m_rcAbout.left, m_rcAbout.top, m_rcAbout.Width(), m_rcAbout.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);
		dc.ExcludeClipRect(m_rcAbout);
	}
	if (dc.RectVisible(m_rcMMSel)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(m_toolZoom ? &m_bmMagBtn : &m_bmMoveBtn);
		dc.BitBlt(m_rcMMSel.left, m_rcMMSel.top, m_rcMMSel.Width(), m_rcMMSel.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);
		dc.ExcludeClipRect(m_rcMMSel);
	}
	if (dc.RectVisible(m_rcZoomVal)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(&m_bmZoomVal);
		dc.BitBlt(m_rcZoomVal.left, m_rcZoomVal.top, m_rcZoomVal.Width(), m_rcZoomVal.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);

		dc.SelectStockObject(DEFAULT_GUI_FONT);
		CString str;
		str.Format(_T("%u %%"), (int)(100 * Getzf()));

		//COLORREF lastBkClr = dc.SetBkColor(GetSysColor(COLOR_WINDOW));
		UINT lastMode = dc.SetTextAlign(TA_CENTER|TA_TOP);
		COLORREF lastTextClr = dc.SetTextColor(RGB(0, 0, 0));
		CSize size =  dc.GetTextExtent(str);
		CPoint pt = m_rcZoomVal.CenterPoint() - CPoint(0, size.cy / 2);
		dc.ExtTextOut(pt.x, pt.y, ETO_CLIPPED, m_rcZoomVal, str, str.GetLength(), NULL);
		dc.SetTextColor(lastTextClr);
		dc.SetTextAlign(lastMode);
		//dc.SetBkColor(lastBkClr);

		dc.ExcludeClipRect(m_rcZoomVal);
	}

	{
		CBrush br;
		br.CreateSysColorBrush(COLOR_WINDOW);
		CRect rc;
		GetClientRect(rc); //dc.GetClipBox(rc);
		dc.FillRect(rc, &br);
	}
}

void CAxVw::UnloadPDF() {
	m_pdfdoc = NULL;
	m_prefpdf.Release();

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

	m_prefpdf.Release();
	m_prefpdf.Attach(new CPDFRef(m_pdfdoc));

	m_iPage = 0;
	m_ft = ftW;
	m_fZoom = 1;

	SetPage(0, true);

	m_strUrl = newVal;
	return S_OK;
}

int CAxVw::CntPages() {
	if (m_pdfdoc != NULL)
		return std::min(1000, m_pdfdoc->getNumPages());
	return 0;
}

void CAxVw::OnUpdatePageUp(CCmdUI *pUI) {
	switch (pUI->m_nID) {
		case ID_PAGE_UP:
			pUI->Enable(m_iPage != 0);
			break;
		case ID_PAGE_DOWN:
			pUI->Enable(m_iPage < CntPages() -1);
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
			Zoom(1.5f);
			return true;
		case ID_ZOOM_MINUS:
			Zoom(1 / 1.5f);
			return true;
	}

	return false;
}

int CAxVw::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (false
		|| !m_sbH.Create(WS_CHILDWINDOW|WS_VISIBLE|SBS_HORZ|SBS_BOTTOMALIGN, CRect(), this, IDC_HORZ)
		|| !m_sbV.Create(WS_CHILDWINDOW|WS_VISIBLE|SBS_VERT|SBS_RIGHTALIGN, CRect(), this, IDC_VERT))
		return -1;

	if (false
		|| !m_bmPrev.LoadBitmap(IDB_PREV)
		|| !m_bmNext.LoadBitmap(IDB_NEXT)
		|| !m_bmAbout.LoadBitmap(IDB_ABOUT)
		|| !m_bmMag.LoadBitmap(IDB_MAG)
		|| !m_bmMagBtn.LoadBitmap(IDB_MAGBTN)
		|| !m_bmMove.LoadBitmap(IDB_MOVE)
		|| !m_bmMoveBtn.LoadBitmap(IDB_MOVEBTN)
		|| !m_bmZoomVal.LoadBitmap(IDB_ZOOMVAL)
		|| !m_bmPageDisp.LoadBitmap(IDB_PAGE_DISP)
		|| !m_bmMask10.LoadBitmap(IDB_MASK10)
		)
		return -1;

	if (false
		|| (m_hcZoom = AfxGetApp()->LoadCursor(IDC_ZOOM)) == NULL
		)
		return -1;

	ZeroMemory(&m_hsc, sizeof(m_hsc));
	ZeroMemory(&m_vsc, sizeof(m_vsc));

	m_hsc.cbSize = sizeof(m_hsc);
	m_hsc.fMask = SIF_DISABLENOSCROLL|SIF_POS|SIF_RANGE|SIF_PAGE;

	m_vsc.cbSize = sizeof(m_hsc);
	m_vsc.fMask = SIF_DISABLENOSCROLL|SIF_POS|SIF_RANGE|SIF_PAGE;

	m_toolZoom = true;
	m_fDrag = false;
	m_fFitOnSmall = true;
	m_iPage = 0;
	m_pThumbs.RemoveAll();

	cntBGDraw = 0;

	LayoutClient();
	return 0;
}

bool CAxVw::MovePage(int iDelta, bool force) {
	int i = m_iPage + iDelta;
	i = std::min(i, CntPages() -1);
	i = std::max(0, i);

	return SetPage(i, force);
}

bool CAxVw::SetPage(int i, bool force) {
	if (i == m_iPage && !force)
		return false;

	m_iPage = i;
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
			UpdScroll();
			InvalidateRect(m_rcZoomVal);
			InvalidateRect(m_rcMMSel);
			InvalidateRect(m_rcPaint);
			break;
	}
}

void CAxVw::Zoom(float fDelta) {
	m_ft = ftZoom;
	m_fZoom *= fDelta;
	Invalidate();
	UpdScroll();
}

void CAxVw::UpdScroll() {
	CSize size = GetZoomedSize();

	m_hsc.nMax = size.cx;
	m_vsc.nMax = size.cy;

	{
		SCROLLINFO &si = m_hsc;
		int &newPos = si.nPos;

		newPos = std::min(newPos, si.nMax - (int)si.nPage);
		newPos = std::max(newPos, si.nMin);
	}

	{
		SCROLLINFO &si = m_vsc;
		int &newPos = si.nPos;

		newPos = std::min(newPos, si.nMax - (int)si.nPage);
		newPos = std::max(newPos, si.nMin);
	}

	m_hsc.nPage = m_rcPaint.Width();
	m_sbH.SetScrollInfo(&m_hsc);

	m_vsc.nPage = m_rcPaint.Height();
	m_sbV.SetScrollInfo(&m_vsc);
}

void CAxVw::LayoutClient() {
	CRect rc;
	GetClientRect(&rc);
	int curx = 0;
	const int cyBar = 24;
	const int cxBMPrev = 32;
	const int cxBMNext = 32;
	const int cxBMDisp = 55;

	{
		m_rcMMSel.left = curx;
		m_rcMMSel.bottom = rc.bottom;
		m_rcMMSel.right = (curx += 24);
		m_rcMMSel.top = rc.bottom - cyBar;

		m_rcZoomVal.left = curx;
		m_rcZoomVal.bottom = rc.bottom;
		m_rcZoomVal.right = (curx += 48);
		m_rcZoomVal.top = rc.bottom - cyBar;

		m_rcPrev.left = curx;
		m_rcPrev.bottom = rc.bottom;
		m_rcPrev.right = curx = (curx += cxBMPrev);
		m_rcPrev.top = rc.bottom - cyBar;

		m_rcDisp.left = curx;
		m_rcDisp.bottom = rc.bottom;
		m_rcDisp.right = curx = (curx += cxBMDisp);
		m_rcDisp.top = rc.bottom - cyBar;

		m_rcNext.left = curx;
		m_rcNext.bottom = rc.bottom;
		m_rcNext.right = curx = (curx += cxBMNext);
		m_rcNext.top = rc.bottom - cyBar;

		m_rcAbout.left = curx;
		m_rcAbout.bottom = rc.bottom;
		m_rcAbout.right = (curx += 24);
		m_rcAbout.top = rc.bottom - cyBar;
	}

	int cex = (rc.Width() - curx) / 2;
	if (cex > 0) {
		m_rcMMSel.OffsetRect(cex, 0);
		m_rcZoomVal.OffsetRect(cex, 0);
		m_rcPrev.OffsetRect(cex, 0);
		m_rcDisp.OffsetRect(cex, 0);
		m_rcNext.OffsetRect(cex, 0);
		m_rcAbout.OffsetRect(cex, 0);
	}

	rc.bottom -= cyBar;

	m_rcPaint = rc;

	UpdScroll();
}

void CAxVw::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	LayoutClient();
}

void CAxVw::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	SCROLLINFO &si = m_hsc;
	const int nBar = SB_HORZ;

	if (pScrollBar == &m_sbH) {
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
			pScrollBar->SetScrollInfo(&si);
			Invalidate();
		}
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAxVw::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	SCROLLINFO &si = m_vsc;
	const int nBar = SB_VERT;

	if (pScrollBar == &m_sbV) {
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
			pScrollBar->SetScrollInfo(&si);
			Invalidate();
		}
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
			m_sbV.SetScrollInfo(&si);
			Invalidate();
		}
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CAxVw::OnLButtonDown(UINT nFlags, CPoint point) {
	if (IsActive()) {
		if (false) { }
		if (m_toolZoom && m_rcPaint.PtInRect(point)) {
			Zoomat(true, point);
		}
		else if (!m_toolZoom && m_rcPaint.PtInRect(point)) {
			m_ptBegin = point;
			m_ptScrollFrm = CPoint(m_hsc.nPos, m_vsc.nPos);
			m_fDrag = true;

			SetCapture();
		}
		else if (m_rcPrev.PtInRect(point)) {
			if (m_iPage > 0) {
				m_iPage--;
				LayoutClient();
				InvalidateRect(m_rcPaint,false);
				InvalidateRect(m_rcDisp,false);
			}
		}
		else if (m_rcNext.PtInRect(point)) {
			if (m_iPage +1 < CntPages()) {
				m_iPage++;
				LayoutClient();
				InvalidateRect(m_rcPaint,false);
				InvalidateRect(m_rcDisp,false);
			}
		}
		else if (m_rcAbout.PtInRect(point)) {
			CWnd *p = GetOwner();
			if (p)
				p->OnCmdMsg(ID_APP_ABOUT, 0, NULL, NULL);
		}
		else if (m_rcMMSel.PtInRect(point)) {
			CMenu aSel;
			VERIFY(aSel.CreatePopupMenu());
			VERIFY(aSel.AppendMenu(MF_BITMAP, IDC_MAG, &m_bmMag));
			VERIFY(aSel.AppendMenu(MF_BITMAP, IDC_MOVE, &m_bmMove));
			CPoint ptMenu = m_rcMMSel.TopLeft();
			ClientToScreen(&ptMenu);
			aSel.TrackPopupMenu(TPM_LEFTALIGN, ptMenu.x, ptMenu.y, GetParentFrame());
		}
		else if (m_rcZoomVal.PtInRect(point)) {
			CMenu aSel;
			VERIFY(aSel.LoadMenu(IDR_MENU_P));
			CMenu *pMenu = aSel.GetSubMenu(0);
			CPoint ptMenu = m_rcZoomVal.TopLeft();
			ClientToScreen(&ptMenu);
			if (pMenu != NULL)
				pMenu->TrackPopupMenu(TPM_LEFTALIGN, ptMenu.x, ptMenu.y, GetParentFrame());
		}
		else if (m_rcDisp.PtInRect(point)) {
			CPvMenu aSel(*this);
			VERIFY(aSel.CreatePopupMenu());
			int cy = CntPages();
			for (int y=0; y<cy; y++) {
				CString str; str.Format(_T("%d"), 1 +y);
#if 1
				aSel.AppendMenu(MF_STRING|((y != 0 && (0 == (y % 50))) ? MF_MENUBREAK : 0), IDC_PAGE1 +y, str);
#else
				aSel.AppendMenu(MF_OWNERDRAW, IDC_PAGE1 +y, str);
#endif
			}
			CPoint ptMenu(m_rcDisp.left, m_rcDisp.bottom);
			ClientToScreen(&ptMenu);
			aSel.TrackPopupMenu(TPM_LEFTALIGN, ptMenu.x, ptMenu.y, GetParentFrame());
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

BOOL CAxVw::OnSelCmd(UINT nID) {
	switch (nID) {
		case IDC_MAG: m_toolZoom = true; break;
		case IDC_MOVE: m_toolZoom = false; break;

		case IDC_P6   : SetzoomR(0.06f); return true;
		case IDC_P12  : SetzoomR(0.12f); return true;
		case IDC_P25  : SetzoomR(0.25f); return true;
		case IDC_P50  : SetzoomR(0.5f); return true;
		case IDC_P100 : SetzoomR(1); return true;
		case IDC_P200 : SetzoomR(2); return true;
		case IDC_P400 : SetzoomR(4); return true;
		case IDC_P800 : SetzoomR(8); return true;
		case IDC_P1600: SetzoomR(16); return true;

		case IDC_FITW: Setft(ftW); return true;
		case IDC_FITWH: Setft(ftWH); return true;

		default: return false;
	}
	InvalidateRect(m_rcMMSel,false);
	return true;
}

void CAxVw::OnUpdateSelCmd(CCmdUI *pUI) {
	switch (pUI->m_nID) {
		case IDC_MAG: pUI->SetCheck(m_toolZoom); break;
		case IDC_MOVE: pUI->SetCheck(!m_toolZoom); break;
		case IDC_FITW: pUI->SetRadio(m_ft == ftW); break;
		case IDC_FITWH: pUI->SetRadio(m_ft == ftWH); break;
	}
}

void CAxVw::SetzoomR(float zf) {
	CPoint posat = GetAbsPosAt(m_rcPaint.CenterPoint() + GetScrollOff());
	Setzf(zf);
	LayoutClient();
	SetCenterAt(posat, m_rcPaint.CenterPoint());
	InvalidateRect(m_rcPaint,false);
}

void CAxVw::Zoomat(bool fIn, CPoint mouseat) {
	float zf = Getzf();
	ZoomatR(fIn ? zf * 2 : zf / 2, mouseat);
}

void CAxVw::ZoomatR(float zf, CPoint mouseat) {
	CPoint clientpt = mouseat;
	CPoint posat = GetAbsPosAt(GetScrollOff() + clientpt);
	Setzf(std::max(0.0625f, std::min(16.0f, zf)));
	LayoutClient();
	SetCenterAt(posat, clientpt);
	InvalidateRect(m_rcPaint,false);
}

CPoint CAxVw::GetCenterPos() const {
	CPoint pt = m_rcPaint.CenterPoint();
	float zf = Getzf();
	return CPoint(
		(int)(pt.x / zf),
		(int)(pt.y / zf)
		);
}

CPoint CAxVw::GetAbsPosAt(CPoint pt) const {
	float zf = Getzf();
	return CPoint(
		(int)(pt.x / zf),
		(int)(pt.y / zf)
		);
}

void CAxVw::SetCenterAt(CPoint pt, CPoint clientpt) {
	float zf = Getzf();
	{
		int xp = Newxp((int)(pt.x * zf - m_rcPaint.Width()/2 + (m_rcPaint.Width()/2 - clientpt.x) ));
		if (xp != m_hsc.nPos)
			m_sbH.SetScrollPos(m_hsc.nPos = xp);
	}
	{
		int yp = Newyp((int)(pt.y * zf - m_rcPaint.Height()/2 + (m_rcPaint.Height()/2 - clientpt.y) ));
		if (yp != m_vsc.nPos)
			m_sbV.SetScrollPos(m_vsc.nPos = yp);
	}
}

CSize CAxVw::GetZoomedSize() {
	CRect rc = GetPageRect(m_iPage);
	if (!rc.IsRectEmpty()) {
		CSize size = rc.Size();
		switch (m_ft) {
		case ftZoom:
			return CSize((int)(size.cx * m_fZoom), (int)(size.cy * m_fZoom));
		case ftW:
			if (m_fFitOnSmall || m_rcPaint.Width() < size.cx) {
				return CSize(
					m_rcPaint.Width(),
					(size.cx != 0)
						? (int)(size.cy * (m_rcPaint.Width() / (float)size.cx))
						: 0
					);
			}
			return size;
		case ftWH:
			if (m_fFitOnSmall || m_rcPaint.Width() < size.cx || m_rcPaint.Height() < size.cy) {
				return Fitrect::Fit(m_rcPaint, size).Size();
			}
			return size;
		}
	}
	return CSize(0,0);
}

float CAxVw::Getzf() const {
	CRect rc = GetPageRect(m_iPage);
	if (!rc.IsRectEmpty()) {
		CSize size = rc.Size();
		switch (m_ft) {
		default:
		case ftZoom:
			return m_fZoom;
		case ftW:
			if (m_fFitOnSmall || m_rcPaint.Width() < size.cx) {
				return (size.cx != 0)
					? (m_rcPaint.Width() / (float)size.cx)
					: 1
					;
			}
			return 1;
		case ftWH:
			if (m_fFitOnSmall || m_rcPaint.Width() < size.cx || m_rcPaint.Height() < size.cy) {
				CSize newSize = Fitrect::Fit(m_rcPaint, size).Size();
				float fx = (size.cx != 0) ? ((float)newSize.cx / size.cx) : 0;
				float fy = (size.cy != 0) ? ((float)newSize.cy / size.cy) : 0;
				if (fx != 0) return fx;
				if (fy != 0) return fy;
				return 1;
			}
			return 1;
		}
	}
	return 1;
}

void CAxVw::OnLButtonUp(UINT nFlags, CPoint point) {
	if (m_fDrag) {
		m_fDrag = false;
		if (GetCapture() == this)
			ReleaseCapture();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CAxVw::OnMouseMove(UINT nFlags, CPoint point) {
	if (m_fDrag && 0 != (nFlags & MK_LBUTTON) && GetCapture() == this) {
		CPoint pt = m_ptScrollFrm + m_ptBegin - point;

		bool moved = false;

		int xp = Newxp(pt.x);
		if (xp != m_hsc.nPos)
			m_sbH.SetScrollPos(m_hsc.nPos = xp), moved=true;

		int yp = Newyp(pt.y);
		if (yp != m_vsc.nPos)
			m_sbV.SetScrollPos(m_vsc.nPos = yp), moved=true;

		if (moved)
			InvalidateRect(m_rcPaint,false);
	}

	CWnd::OnMouseMove(nFlags, point);
}

// http://msdn.microsoft.com/en-us/library/ms645614(VS.85).aspx
LRESULT CAxVw::OnMouseHWheel(WPARAM wp, LPARAM) {
	short zDelta = HIWORD(wp);
	{
		int oldpos = m_sbH.GetScrollPos();
		int newpos = oldpos;

		newpos = Newxp(newpos +zDelta);

		if (oldpos != newpos) {
			m_hsc.nPos = newpos;
			m_sbH.SetScrollPos(newpos);
			this->InvalidateRect(m_rcPaint, false);
			return 1;
		}
	}
	return 0;
}

void CAxVw::OnRButtonDown(UINT nFlags, CPoint point) {
	if (IsActive()) {
		if (m_toolZoom && m_rcPaint.PtInRect(point)) {
			Zoomat(false, point);
			return;
		}
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CAxVw::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
	OnLButtonUp(nFlags, point);

//	CWnd::OnLButtonDblClk(nFlags, point);
}

void CAxVw::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	OnRButtonDown(nFlags, point);
	OnRButtonUp(nFlags, point);

//	CWnd::OnRButtonDblClk(nFlags, point);
}

BOOL CAxVw::OnPageSel(UINT nID) {
	SetPage(nID -IDC_PAGE1);
	return true;
}

void CAxVw::OnUpdatePageSel(CCmdUI *pUI) {
	pUI->SetRadio(m_iPage == (pUI->m_nID - IDC_PAGE1));
}

CBitmap *CAxVw::GetThumb(int iPage, int cx) {
	if (m_pThumbs.GetCount() <= (size_t)iPage || m_pThumbs[iPage] == NULL) {
		CSingleLock lck(&s_lockpdf);

		CRect rcDraw(0, 0, cx, cx);
		CRect rcPage = GetPageRect(iPage);
		float scale = (float)cx / std::max(rcPage.Height(), rcPage.Width());

		double dpi = 72 * scale;

		SplashColor paperColor;
		paperColor[0] = 255;
		paperColor[1] = 255;
		paperColor[2] = 255;
		std::auto_ptr<SplashOutputDev> splashOut;
		splashOut.reset(new SplashOutputDev(splashModeRGB8, 4, gFalse, paperColor));
		splashOut->startDoc(m_pdfdoc->getXRef());

		const int cy = cx;

		CRect rcDst(0, 0, 
			int(rcPage.Width() * scale),
			int(rcPage.Height() * scale)
			);

		int slx = (cx - rcDst.Width()) / 2;
		int sly = (cy - rcDst.Height()) / 2;

		m_pdfdoc->displayPageSlice(
			splashOut.get(), 
			1 +iPage, dpi, dpi, 
			0,
			gTrue, gFalse, gFalse,
			slx, sly, rcDst.Width(), rcDst.Height()
			);

		lck.Unlock();

		SplashBitmap *bitmap = splashOut->getBitmap();
		int pcx = bitmap->getWidth();
		int pcy = bitmap->getHeight();
		BITMAPINFO bi;
		ZeroMemory(&bi, sizeof(bi));
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth = pcx;
		bi.bmiHeader.biHeight = pcy;
		bi.bmiHeader.biBitCount = 24;
		bi.bmiHeader.biPlanes = 1;

		void *pvBits = NULL;
		HANDLE h = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pvBits, NULL, 0);
		if (h != NULL) {
			for (int y=0; y<pcy; y++) {
				SplashColorPtr pSrc = bitmap->getDataPtr() + bitmap->getRowSize() * y;
				BYTE *pDst = reinterpret_cast<PBYTE>(pvBits) +bitmap->getRowSize() * (cy -y -1);
				for (int x=0; x<pcx; x++, pSrc += 3, pDst += 3) {
					pDst[0] = pSrc[2];
					pDst[1] = pSrc[1];
					pDst[2] = pSrc[0];
				}
			}

			CBitmap *pBM = new CBitmap();
			pBM->Attach(h);
			m_pThumbs.SetAtGrow(iPage, CAutoPtr<CBitmap>(pBM));
		}
	}

	if (m_pThumbs.GetCount() <= (size_t)iPage)
		return NULL;

	return static_cast<CBitmap *>(m_pThumbs[iPage]);
}

int CAxVw::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) {
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

BOOL CAxVw::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	if (nHitTest == HTCLIENT) {
		DWORD xy = GetMessagePos();
		CPoint pt(GET_X_LPARAM(xy), GET_Y_LPARAM(xy));
		ScreenToClient(&pt);
		if (m_rcPaint.PtInRect(pt)) {
			if (m_toolZoom) {
				SetCursor(m_hcZoom);
				return true;
			}
		}
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CAxVw::OnSetRenderInf(WPARAM, LPARAM lParam) {
	cntBGDraw = 0;
	CRenderInf *inf = reinterpret_cast<CRenderInf *>(lParam);
	if (inf->partial) {
		m_renderPart.reset(inf);
	}
	else {
		m_renderAll.reset(inf);
	}
	InvalidateRect(m_rcPaint);
	return 0;
}
