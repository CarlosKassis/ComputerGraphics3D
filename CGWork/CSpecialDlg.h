#pragma once
#include "stdafx.h"
#include "resource.h"
#include "Globals.h"


// CSpecialDlg dialog

class CSpecialDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpecialDlg)

public:
	CSpecialDlg(
		double fogFar,
		double motionBlur,
		COLORREF m_fogColor,
		FogMode m_fogMode,
		int AAType,
		int AASize);   // standard constructor
	virtual ~CSpecialDlg();
	enum { IDD = IDD_SPECIAL_DIALOG };
// Dialog Data

	BOOL OnInitDialog();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	DECLARE_MESSAGE_MAP()

private:

	CButton m_cFogColor;
	CButton m_cFogTransparent;
	CButton m_cFogOff;

public:

	void OnBnClickedFogColor();

	void OnBtnClickedFogColor();

	void OnBnClickedFogTransparent();

	void OnBnClickedFogOff();


	void GetParams(
		double& fogFar,
		double& motionBlur,
		COLORREF& fogColor,
		FogMode& fogMode,
		int& AAtype,
		int& AASize);


	int m_nFogFar;
	int m_nAASize;
	int m_nAAType;

	int m_nMotionBlur;
	COLORREF m_fogColor;
	FogMode m_fogMode;


private:
	CSliderCtrl m_cFogFarSlider;
	CSliderCtrl m_cMotionBlurSlider;
	CComboBox m_comboAAKernelType;
	CComboBox m_comboAASize;
public:
	afx_msg void OnCbnSelchangeComboAaSize();
	afx_msg void OnCbnSelchangeComboAaType();
};
