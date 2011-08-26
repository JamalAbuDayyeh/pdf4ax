// AxVw.h : CAxVw クラスのインターフェイス
//

#pragma once

//#include "UseG.h"

// CAxVw ウィンドウ

class CAxVw : public CWnd
{
// コンストラクション
public:
	CAxVw();

// 属性
public:
	CString m_strUrl;

protected:
	PDFDoc *m_pdfdoc;
	int m_top, m_cx;
	double m_scalexy;
	typedef enum {
		ftZoom,
		ftW,
		ftWH,
	} FitMode;
	FitMode m_ft;
	SCROLLINFO m_hsc, m_vsc;

// 操作
public:
	HRESULT LoadPDF(LPCTSTR psz);
	bool MovePage(int iDelta, bool force = false);
	bool SetPage(int iPage, bool force = false);
	void Setft(FitMode ft);
	void Zoom(double fDelta);
	void UpdScroll();
	void UnloadPDF();

	CRect GetPageRect(int top);

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
};

