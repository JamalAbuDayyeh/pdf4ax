// AxVw.h : CAxVw �N���X�̃C���^�[�t�F�C�X
//

#pragma once

//#include "UseG.h"

// CAxVw �E�B���h�E

class CAxVw : public CWnd
{
// �R���X�g���N�V����
public:
	CAxVw();

// ����
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

// ����
public:
	HRESULT LoadPDF(LPCTSTR psz);
	bool MovePage(int iDelta, bool force = false);
	bool SetPage(int iPage, bool force = false);
	void Setft(FitMode ft);
	void Zoom(double fDelta);
	void UpdScroll();
	void UnloadPDF();

	CRect GetPageRect(int top);

// �I�[�o�[���C�h
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ����
public:
	virtual ~CAxVw();

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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

