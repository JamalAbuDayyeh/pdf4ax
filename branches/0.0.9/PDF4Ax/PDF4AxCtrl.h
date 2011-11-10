#pragma once

// PDF4AxCtrl.h : CPDF4AxCtrl ActiveX �R���g���[�� �N���X�̐錾�ł��B

#include "AxFrm.h"

// CPDF4AxCtrl : �����Ɋւ��Ă� PDF4AxCtrl.cpp ���Q�Ƃ��Ă��������B

class CPDF4AxCtrl : public COleControl
{
	DECLARE_DYNCREATE(CPDF4AxCtrl)

// �R���X�g���N�^
public:
	CPDF4AxCtrl();

// �I�[�o�[���C�h
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual BOOL IsInvokeAllowed(DISPID id) {
		return TRUE;
	}

// ����
protected:
	~CPDF4AxCtrl();

	CAxFrame m_frm;

	DWORD m_dwSafety;

	CComPtr<IMoniker> m_pimkDL;

	void LoadFromMoniker(LPBC pibc, LPMONIKER pimkDL);

	DECLARE_OLECREATE_EX(CPDF4AxCtrl)    // �N���X �t�@�N�g�� �� guid
	DECLARE_OLETYPELIB(CPDF4AxCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CPDF4AxCtrl)     // �v���p�e�B �y�[�W ID
	DECLARE_OLECTLTYPE(CPDF4AxCtrl)		// �^�C�v���Ƃ��̑��̃X�e�[�^�X

// ���b�Z�[�W �}�b�v
	DECLARE_MESSAGE_MAP()

	BEGIN_INTERFACE_PART(ObjectSafety, IObjectSafety)
		STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD __RPC_FAR *pdwSupportedOptions, DWORD __RPC_FAR *pdwEnabledOptions);
		STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);
	END_INTERFACE_PART(ObjectSafety)

    DECLARE_INTERFACE_MAP()

// �f�B�X�p�b�` �}�b�v
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// �C�x���g �}�b�v
	DECLARE_EVENT_MAP()

// �f�B�X�p�b�` �� �C�x���g ID
public:
	enum {
		dispidsrc = 1001
	};
protected:
	BSTR Getsrc(void);
	void Setsrc(LPCTSTR newVal);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

