#pragma once

// PDF4Ax.h : PDF4Ax.DLL �̃��C�� �w�b�_�[ �t�@�C��

#if !defined( __AFXCTL_H__ )
#error "���̃t�@�C�����C���N���[�h����O�� 'afxctl.h' ���C���N���[�h���Ă��������B"
#endif

#include "resource.h"       // ���C�� �V���{��


// CPDF4AxApp : �����Ɋւ��Ă� PDF4Ax.cpp ���Q�Ƃ��Ă��������B

class CPDF4AxApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

