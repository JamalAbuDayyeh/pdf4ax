// Frm.h : Frm �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"       // ���C�� �V���{��


// CFrmApp:
// ���̃N���X�̎����ɂ��ẮAFrm.cpp ���Q�Ƃ��Ă��������B
//

class CFrmApp : public CWinApp
{
public:
	CFrmApp();


// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CFrmApp theApp;