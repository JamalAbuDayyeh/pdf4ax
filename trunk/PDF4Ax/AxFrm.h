// AxFrm.h : CAxFrame �N���X�̃C���^�[�t�F�C�X
//


#pragma once

#include "AxVw.h"

class CAxFrame : public CFrameWnd
{
	
public:
	CAxFrame();
protected: 

// ����
public:

// ����
public:

// �I�[�o�[���C�h
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// ����
public:
	virtual ~CAxFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �R���g���[�� �o�[�p�����o
	CStatusBar  m_wndStatusBar;
	CReBar      m_wndReBar;
	CToolBar    m_wndToolBar;

public:
	CAxVw    m_wndView;

// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	afx_msg void OnAppAbout();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


