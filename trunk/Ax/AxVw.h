// AxVw.h : CAxVw クラスのインターフェイス
//

#pragma once

//#include "UseG.h"

class CPvRender {
public:
	virtual CBitmap *GetThumb(int iPage, int cx) = NULL;
};

// CAxVw ウィンドウ

class CAxVw : public CWnd, public CPvRender
{
// コンストラクション
public:
	CAxVw();

// 属性
public:
	CString m_strUrl;

protected:
	PDFDoc *m_pdfdoc;
	int m_iPage;
	float m_fZoom;
	typedef enum {
		ftZoom,
		ftW,
		ftWH,
	} FitMode;
	FitMode m_ft;
	SCROLLINFO m_hsc, m_vsc;

	CBitmap m_bmPrev, m_bmNext, m_bmAbout, m_bmMag, m_bmMagBtn, m_bmMove, m_bmMoveBtn, m_bmZoomVal, m_bmPageDisp;
	CRect m_rcPaint, m_rcPrev, m_rcNext, m_rcDisp, m_rcAbout, m_rcFitWH, m_rcFitW;
	CRect m_rcMMSel, m_rcZoomVal;
	bool m_toolZoom;
	CPoint m_ptBegin, m_ptScrollFrm;
	bool m_fDrag;
	CScrollBar m_sbH, m_sbV;
	bool m_fFitOnSmall;
	CAutoPtrArray<CBitmap> m_pThumbs;

// 操作
public:
	HRESULT LoadPDF(LPCTSTR psz);
	bool MovePage(int iDelta, bool force = false);
	bool SetPage(int iPage, bool force = false);
	void Setft(FitMode ft);
	void Zoom(float fDelta);
	void UpdScroll();
	void UnloadPDF();

	int CntPages();

	CRect GetPageRect(int top) const;

	void LayoutClient();

	void Zoomat(bool fIn, CPoint at);
	void ZoomatR(float zf, CPoint at);
	void SetzoomR(float zf);

	CPoint GetCenterPos() const;
	CPoint GetAbsPosAt(CPoint pt) const;
	void SetCenterAt(CPoint pt, CPoint clientpt);

	CPoint GetScrollOff() const {
		return CPoint(m_hsc.nPos, m_vsc.nPos);
	}
	void Setzf(float zf) {
		m_fZoom = zf;
		SetFit(ftZoom);
		InvalidateRect(m_rcZoomVal,false);
	}
	void SetFit(FitMode fit) {
		Setft(fit);
	}
	float Getzf() const;

	int Newxp(int v) {
		return max(m_hsc.nMin, min(m_hsc.nMax - (int)m_hsc.nPage + 1, v));
	}
	int Newyp(int v) const {
		return max(m_vsc.nMin, min(m_vsc.nMax - (int)m_vsc.nPage + 1, v));
	}
	CSize GetZoomedSize();

	bool IsActive() {
		CWnd *p = GetFocus();
		CWnd *p2 = this;
		while (p2 != NULL) {
			if (p == p2)
				return true;
			p2 = p2->GetParent();
		}
		return false;
	}

	class Fitrect {
	public:
		static CRect Fit(CRect rcMax, CSize rcBox) {
            if (rcMax.Height() == 0 || rcBox.cy == 0)
                return rcMax;
            float frMax = rcMax.Width() / (float)rcMax.Height();
            float frBox = rcBox.cx / (float)rcBox.cy;
            float centerx = ((float)rcMax.left + rcMax.right) / 2;
			float centery = ((float)rcMax.top + rcMax.bottom) / 2;
            if (frMax >= frBox) {
                // 縦長
                float v = rcBox.cx * rcMax.Height() / rcBox.cy / 2;
                return CRect(
                    int(centerx - v),
                    int(rcMax.top),
                    int(centerx + v),
                    int(rcMax.bottom)
                    );
            }
            else {
                // 横長
                float v = rcBox.cy * rcMax.Width() / rcBox.cx / 2;
                return CRect(
                    int(rcMax.left),
                    int(centery - v),
                    int(rcMax.right),
                    int(centery + v)
                    );
            }
        }
	};

	CBitmap *GetThumb(int iPage, int cx);

// オーバーライド
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 実装
public:
	virtual ~CAxVw();

	// 生成された、メッセージ割り当て関数
protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnPageUp(UINT nID);
	afx_msg void OnUpdatePageUp(CCmdUI *pUI);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSelCmd(UINT nID);
	afx_msg void OnUpdateSelCmd(CCmdUI *pUI);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHWheel(WPARAM, LPARAM);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnPageSel(UINT nID);
	afx_msg void OnUpdatePageSel(CCmdUI *pUI);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};
