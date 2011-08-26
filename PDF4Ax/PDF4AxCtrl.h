#pragma once

// PDF4AxCtrl.h : CPDF4AxCtrl ActiveX コントロール クラスの宣言です。

#include "AxFrm.h"

// CPDF4AxCtrl : 実装に関しては PDF4AxCtrl.cpp を参照してください。

class CPDF4AxCtrl : public COleControl
{
	DECLARE_DYNCREATE(CPDF4AxCtrl)

// コンストラクタ
public:
	CPDF4AxCtrl();

// オーバーライド
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual BOOL IsInvokeAllowed(DISPID id) {
		return TRUE;
	}

// 実装
protected:
	~CPDF4AxCtrl();

	CAxFrame m_frm;

	DWORD m_dwSafety;

	CComPtr<IMoniker> m_pimkDL;

	void LoadFromMoniker(LPBC pibc, LPMONIKER pimkDL);

	DECLARE_OLECREATE_EX(CPDF4AxCtrl)    // クラス ファクトリ と guid
	DECLARE_OLETYPELIB(CPDF4AxCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CPDF4AxCtrl)     // プロパティ ページ ID
	DECLARE_OLECTLTYPE(CPDF4AxCtrl)		// タイプ名とその他のステータス

// メッセージ マップ
	DECLARE_MESSAGE_MAP()

	BEGIN_INTERFACE_PART(ObjectSafety, IObjectSafety)
		STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD __RPC_FAR *pdwSupportedOptions, DWORD __RPC_FAR *pdwEnabledOptions);
		STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);
	END_INTERFACE_PART(ObjectSafety)

    DECLARE_INTERFACE_MAP()

// ディスパッチ マップ
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// イベント マップ
	DECLARE_EVENT_MAP()

// ディスパッチ と イベント ID
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

