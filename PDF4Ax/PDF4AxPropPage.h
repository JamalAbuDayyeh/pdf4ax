#pragma once

// PDF4AxPropPage.h : CPDF4AxPropPage �v���p�e�B �y�[�W �N���X�̐錾�ł��B


// CPDF4AxPropPage : �����Ɋւ��Ă� PDF4AxPropPage.cpp ���Q�Ƃ��Ă��������B

class CPDF4AxPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CPDF4AxPropPage)
	DECLARE_OLECREATE_EX(CPDF4AxPropPage)

// �R���X�g���N�^
public:
	CPDF4AxPropPage();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_PROPPAGE_PDF4AX };

// ����
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ���b�Z�[�W �}�b�v
protected:
	DECLARE_MESSAGE_MAP()
};

