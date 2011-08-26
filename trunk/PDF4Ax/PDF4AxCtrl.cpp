// PDF4AxCtrl.cpp :  CPDF4AxCtrl ActiveX �R���g���[�� �N���X�̎���

#include "stdafx.h"
#include "PDF4Ax.h"
#include "PDF4AxCtrl.h"
#include "PDF4AxPropPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CPDF4AxCtrl, COleControl)

BEGIN_INTERFACE_MAP(CPDF4AxCtrl, COleControl)
  INTERFACE_PART(CPDF4AxCtrl, IID_IObjectSafety, ObjectSafety)
END_INTERFACE_MAP()

// ���b�Z�[�W �}�b�v

BEGIN_MESSAGE_MAP(CPDF4AxCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// �f�B�X�p�b�` �}�b�v

BEGIN_DISPATCH_MAP(CPDF4AxCtrl, COleControl)
	DISP_FUNCTION_ID(CPDF4AxCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX_ID(CPDF4AxCtrl, "src", dispidsrc, Getsrc, Setsrc, VT_BSTR)
END_DISPATCH_MAP()



// �C�x���g �}�b�v

BEGIN_EVENT_MAP(CPDF4AxCtrl, COleControl)
END_EVENT_MAP()



// �v���p�e�B �y�[�W

// TODO: �v���p�e�B �y�[�W��ǉ����āABEGIN �s�̍Ō�ɂ���J�E���g�𑝂₵�Ă��������B
BEGIN_PROPPAGEIDS(CPDF4AxCtrl, 1)
	PROPPAGEID(CPDF4AxPropPage::guid)
END_PROPPAGEIDS(CPDF4AxCtrl)



// �N���X �t�@�N�g������� GUID �����������܂��B

IMPLEMENT_OLECREATE_EX(CPDF4AxCtrl, "PDF4Ax.PDFVw.1",
	0xfe687896, 0xf410, 0x4d10, 0x87, 0x40, 0xd5, 0x84, 0xda, 0x23, 0xc7, 0x4d)



// �^�C�v ���C�u���� ID ����уo�[�W����

IMPLEMENT_OLETYPELIB(CPDF4AxCtrl, _tlid, _wVerMajor, _wVerMinor)



// �C���^�[�t�F�C�X ID

const IID BASED_CODE IID_DPDF4Ax =
		{ 0x3FF92EA8, 0x9206, 0x48D0, { 0x90, 0xF2, 0x0, 0xF8, 0xA9, 0xA9, 0x8E, 0x16 } };
const IID BASED_CODE IID_DPDF4AxEvents =
		{ 0x7D43C1E0, 0xB4DF, 0x4E6F, { 0x9E, 0x34, 0x4D, 0xDB, 0xF5, 0x25, 0x89, 0x6 } };



// �R���g���[���̎�ނ̏��

static const DWORD BASED_CODE _dwPDF4AxOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CPDF4AxCtrl, IDS_PDF4AX, _dwPDF4AxOleMisc)



// CPDF4AxCtrl::CPDF4AxCtrlFactory::UpdateRegistry -
// CPDF4AxCtrl �̃V�X�e�� ���W�X�g�� �G���g����ǉ��܂��͍폜���܂��B

BOOL CPDF4AxCtrl::CPDF4AxCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: �R���g���[���� apartment ���f���̃X���b�h�����̋K���ɏ]���Ă��邱�Ƃ�
	// �m�F���Ă��������B�ڍׂɂ��Ă� MFC �̃e�N�j�J�� �m�[�g 64 ���Q�Ƃ��Ă��������B
	// �A�p�[�g�����g ���f���̃X���b�h�����̋K���ɏ]��Ȃ��R���g���[���̏ꍇ�́A6 �Ԗ�
	// �̃p�����[�^���ȉ��̂悤�ɕύX���Ă��������B
	// afxRegApartmentThreading �� 0 �ɐݒ肵�܂��B

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_PDF4AX,
			IDB_PDF4AX,
			afxRegApartmentThreading,
			_dwPDF4AxOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CPDF4AxCtrl::CPDF4AxCtrl - �R���X�g���N�^

CPDF4AxCtrl::CPDF4AxCtrl()
{
	InitializeIIDs(&IID_DPDF4Ax, &IID_DPDF4AxEvents);
	m_dwSafety = 0;
}



// CPDF4AxCtrl::~CPDF4AxCtrl - �f�X�g���N�^

CPDF4AxCtrl::~CPDF4AxCtrl()
{
	// TODO: ���̈ʒu�ɃR���g���[���̃C���X�^���X �f�[�^�̌㏈���p�̃R�[�h��ǉ����Ă�������
}



// CPDF4AxCtrl::OnDraw - �`��֐�

void CPDF4AxCtrl::OnDraw(
			CDC* pDC, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pDC)
		return;

}



// CPDF4AxCtrl::DoPropExchange - �i�����̃T�|�[�g

void CPDF4AxCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	CString &m_src = m_frm.m_wndView.m_strUrl;
	CString m_title;

	PX_String(pPX, _T("src"), m_src);

	HRESULT hr;

	CComPtr<IBindCtx> pibc;
	if (SUCCEEDED(hr = CreateBindCtx(0, &pibc))) {
		IOleClientSite *pClientSite = GetClientSite();
		if (pClientSite != NULL) {
			CComPtr<IMoniker> pimkDL;
			if (pimkDL == NULL) {
				CComQIPtr<IBindHost> pBindHost = pClientSite;
				if (pBindHost != NULL) {
					if (SUCCEEDED(hr = pBindHost->CreateMoniker(CT2W(m_src), pibc, &pimkDL, 0))) {

					}
				}
			}
			if (pimkDL == NULL) {
				CComPtr<IMoniker> pimkName;
				if (SUCCEEDED(hr = pClientSite->GetMoniker(OLEGETMONIKER_FORCEASSIGN, OLEWHICHMK_CONTAINER, &pimkName))) {
					CComPtr<IBindCtx> pibc;
					CreateBindCtx(0, &pibc);
					ULONG chEaten = 0;
					if (SUCCEEDED(hr = pimkName->ParseDisplayName(pibc, pimkName, CT2W(m_src), &chEaten, &pimkDL))) {

					}
				}
			}

			if (pimkDL != NULL) {
				LPOLESTR pszDisplayName = NULL;
				if (SUCCEEDED(hr = pimkDL->GetDisplayName(pibc, NULL, &pszDisplayName))) {
					m_title = pszDisplayName;
					CoTaskMemFree(pszDisplayName);
				}
			}

			m_pimkDL = pimkDL;

			LoadFromMoniker(pibc, pimkDL);
		}
	}
}

#pragma comment(lib, "shlwapi.lib")

void CPDF4AxCtrl::LoadFromMoniker(LPBC pibc, LPMONIKER pimkDL) {
	HRESULT hr;

	bool ok = false;

	if (pimkDL != NULL) {
		if (!ok) {
			LPOLESTR lpcw = NULL;
			if (SUCCEEDED(hr = pimkDL->GetDisplayName(pibc, NULL, &lpcw))) {
				WCHAR wcfp[1000] = {0};
				DWORD cchfp = 256;
				if (UrlIsFileUrl(lpcw)) {
					if (SUCCEEDED(hr = PathCreateFromUrl(lpcw, wcfp, &cchfp, NULL))) {
						Setsrc(CW2T(wcfp));
						ok = true;
					}
				}
				CoTaskMemFree(lpcw);
			}
		}
		if (!ok) {
			CComPtr<IStream> pSt;
			if (SUCCEEDED(hr = pimkDL->BindToStorage(pibc, NULL, IID_IStream, reinterpret_cast<void **>(&pSt)))) {
				TRY
					TCHAR tcdir[MAX_PATH] = {0};
					GetTempPath(256, tcdir);
					TCHAR tctmp[MAX_PATH] = {0};
					GetTempFileName(tcdir, _T("pdf"), 0, tctmp);

					COleStreamFile fIn(pSt);
					CFile fOut;
					if (fOut.Open(tctmp, CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyWrite)) {
						BYTE buff[4000];
						while (true) {
							UINT r = fIn.Read(buff, 4000);
							if (r == 0)
								break;
							fOut.Write(buff, r);
						}
						Setsrc(CW2T(tctmp));
						ok = true;
					}
				CATCH_ALL(e) 
				END_CATCH_ALL
			}
		}
	}
	AfxThrowFileException(CFileException::fileNotFound);
}


// CPDF4AxCtrl::GetControlFlags -
// MFC �� ActiveX �R���g���[���̎����̃J�X�^�}�C�Y�p�t���O�ł��B
//
//DWORD CPDF4AxCtrl::GetControlFlags()
//{
//	DWORD dwFlags = COleControl::GetControlFlags();
//
//
//	// �R���g���[���̓E�B���h�E���쐬�����ɃA�N�e�B�x�[�g�\�ł��B
//	// TODO: �R���g���[���̃��b�Z�[�W �n���h�����쐬����ꍇ�Am_hWnd
//	//		m_hWnd �����o�ϐ��̒l�� NULL �ȊO�ł��邱�Ƃ��ŏ��Ɋm�F
//	//		���Ă���g�p���Ă��������B
//	dwFlags |= 0;
//	return dwFlags;
//}



// CPDF4AxCtrl::OnResetState - �R���g���[��������̏�ԂɃ��Z�b�g���܂��B

void CPDF4AxCtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange ���Ăяo���Ċ���l�Ƀ��Z�b�g

	// TODO: ���̈ʒu�ɃR���g���[���̏�Ԃ����Z�b�g���鏈����ǉ����Ă�������
}



// CPDF4AxCtrl::AboutBox - "�o�[�W�������" �{�b�N�X�����[�U�[�ɕ\�����܂��B

void CPDF4AxCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_PDF4AX);
	dlgAbout.DoModal();
}



// CPDF4AxCtrl ���b�Z�[�W �n���h��

BSTR CPDF4AxCtrl::Getsrc(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult = m_frm.m_wndView.m_strUrl;

	return strResult.AllocSysString();
}

void CPDF4AxCtrl::Setsrc(LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;
	if (FAILED(hr = m_frm.m_wndView.LoadPDF(newVal)))
		AfxThrowOleException(hr);
}

int CPDF4AxCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_frm.LoadFrame(IDR_MAINFRAME, WS_CHILD, this))
		return -1;

	m_frm.InitialUpdateFrame(NULL, true);

	return 0;
}

void CPDF4AxCtrl::OnSize(UINT nType, int cx, int cy) {
	COleControl::OnSize(nType, cx, cy);

	if (m_frm.m_hWnd != NULL)
		m_frm.MoveWindow(0, 0, cx, cy);
}

STDMETHODIMP CPDF4AxCtrl::XObjectSafety::GetInterfaceSafetyOptions(
            REFIID riid, 
            DWORD __RPC_FAR *pdwSupportedOptions, 
            DWORD __RPC_FAR *pdwEnabledOptions)
{
    METHOD_PROLOGUE_EX(CPDF4AxCtrl, ObjectSafety)

	if (!pdwSupportedOptions || !pdwEnabledOptions)
		return E_FAIL;

	LPUNKNOWN pUnk = NULL;
	if (QueryInterface(riid, reinterpret_cast<LPVOID *>(&pUnk)) == E_NOINTERFACE) {
		return E_NOINTERFACE;
	}
	else {
		pUnk->Release();
		pUnk = NULL;
	}

	DWORD &m_dwSafety = pThis->m_dwSafety;

	if (riid == IID_IDispatch) {
		// IDispatch is an interface used for scripting. If your
		// control supports other IDispatch or Dual interfaces, you
		// may decide to add them here as well. Client wants to know
		// if object is safe for scripting. Only indicate safe for
		// scripting when the interface is safe.
		*pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER;
		*pdwEnabledOptions = m_dwSafety & INTERFACESAFE_FOR_UNTRUSTED_CALLER;
		return S_OK;
	}
	else if ((riid == IID_IPersistStreamInit) || (riid == IID_IPersistStorage) || (riid == IID_IPersistPropertyBag)) {
		// IID_IPersistStreamInit and IID_IPersistStorage are
		// interfaces used for Initialization. If your control
		// supports other Persistence interfaces, you may decide to
		// add them here as well. Client wants to know if object is
		// safe for initializing. Only indicate safe for initializing
		// when the interface is safe.
		*pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_DATA;
		*pdwEnabledOptions = m_dwSafety & INTERFACESAFE_FOR_UNTRUSTED_DATA;
		return S_OK;
	}
	else {
		// We are saying that no other interfaces in this control are
		// safe for initializing or scripting.
		*pdwSupportedOptions = 0;
		*pdwEnabledOptions = 0;
		return E_FAIL;
	}
}

STDMETHODIMP CPDF4AxCtrl::XObjectSafety::SetInterfaceSafetyOptions(
        REFIID riid, 
        DWORD dwOptionSetMask, 
        DWORD dwEnabledOptions)
{
    METHOD_PROLOGUE_EX(CPDF4AxCtrl, ObjectSafety)
	
	// http://support.microsoft.com/kb/168371/ja

	if (!dwOptionSetMask && !dwEnabledOptions) return E_FAIL;

	LPUNKNOWN pUnk = NULL;
	if (QueryInterface(riid, reinterpret_cast<LPVOID *>(&pUnk)) == E_NOINTERFACE) {
		return E_NOINTERFACE;
	}
	else if (pUnk) {
		pUnk->Release();
		pUnk = NULL;
	}

	DWORD &m_dwSafety = pThis->m_dwSafety;

	m_dwSafety |= dwEnabledOptions & dwOptionSetMask;

	if ((riid == IID_IDispatch) && (m_dwSafety & INTERFACESAFE_FOR_UNTRUSTED_CALLER) != 0) {
	   // Client wants us to disable any functionality that would
	   // make the control unsafe for scripting. The same applies to
	   // any other IDispatch or Dual interfaces your control may
	   // support. Because our control is safe for scripting by
	   // default we just return S_OK.
		return S_OK;
	}
	else if ((riid == IID_IPersistStreamInit || riid == IID_IPersistStorage || riid == IID_IPersistPropertyBag) && (m_dwSafety & INTERFACESAFE_FOR_UNTRUSTED_DATA) != 0) {
	   // Client wants us to make the control safe for initializing
	   // from persistent data. For these interfaces, this control
	   // is safe so we return S_OK. For Any interfaces that are not
	   // safe, we would return E_FAIL.
		return S_OK;
	}

    return E_FAIL;
}

STDMETHODIMP_(ULONG) CPDF4AxCtrl::XObjectSafety::AddRef()
{
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, ObjectSafety)
    return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CPDF4AxCtrl::XObjectSafety::Release()
{
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, ObjectSafety)
    return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CPDF4AxCtrl::XObjectSafety::QueryInterface(
    REFIID iid, LPVOID* ppvObj)
{
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, ObjectSafety)
    return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

BOOL CPDF4AxCtrl::PreCreateWindow(CREATESTRUCT& cs) {
	if (!COleControl::PreCreateWindow(cs))
		return false;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return true;
}
