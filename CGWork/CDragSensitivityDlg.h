#pragma once
#include "stdafx.h"
#include "resource.h"
#include "Globals.h"
class CDragSensitivityDlg : public CDialog
{
	DECLARE_DYNAMIC(CDragSensitivityDlg)

public:
	CDragSensitivityDlg(int scale, double rotate, double translate);
	virtual ~CDragSensitivityDlg();

	// Dialog Data
	enum { IDD = IDD_DRAG_SENSITIVITY_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL OnInitDialog();
public:

	int m_scaleSensitivity = 1.0;
	int m_rotateSensitivity = 1.0;
	int m_translateSensitivity = 1.0;

	CSliderCtrl m_scaleSliderControl;
	CSliderCtrl m_rotateSliderControl;
	CSliderCtrl m_translateSliderControl;
	DECLARE_MESSAGE_MAP()
};