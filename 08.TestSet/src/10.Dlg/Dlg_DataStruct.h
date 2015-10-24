#pragma once
#include "resource.h"
#include "DataStruct/B+Tree.h"

// CDlg_DataStruct �Ի���

class CDlg_DataStruct : public CDialog
{
	DECLARE_DYNAMIC(CDlg_DataStruct)

public:
	CDlg_DataStruct(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlg_DataStruct();

// �Ի�������
	enum { IDD = IDD_DLG_DATASTRUCT };

	CBTree BTree;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedInsert();
	afx_msg void OnBnClickedCode();
    afx_msg void OnBnClickedDatastructBtnCdma();
    afx_msg void OnBnClickedDatastructBtnBigMatch();
};
