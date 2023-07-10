#pragma once
#include "stdafx.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CPersOptions dialog used for Perspective options

class CPersOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CPersOptionsDlg)
public:
	CPersOptionsDlg(int closeClip, double farClip, double d, double orthographicSize);
	virtual ~CPersOptionsDlg();

	int Close;
	int Far;
	int D;
	int OrthographicSize;
	CSliderCtrl CloseSlider;
	CSliderCtrl FarSlider;
	CSliderCtrl DSlider;
	CSliderCtrl OrthographicSizeSlider;

	enum { IDD = IDD_PERS_OPTIONS };

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnInitDialog();

};


