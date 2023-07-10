#pragma once
#include "stdafx.h"
#include "resource.h"
#include "Globals.h"

class CGeneralDlg : public CDialog
{
	DECLARE_DYNAMIC(CGeneralDlg)

public:
	CGeneralDlg(
		int tesselation,
		bool image,
		bool stretch,
		bool defaultSize, 
		CString filePath, 
		int height, 
		int width, 
		COLORREF bg, 
		bool alwaysCalc, 
		bool inverse);

	virtual ~CGeneralDlg();

	// Dialog Data
	enum { IDD = IDD_GENERAL_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL OnInitDialog();
public:

	int m_tesselation = DEFAULT_TESSELATION;

	CSliderCtrl m_tesselationSliderControl;
	DECLARE_MESSAGE_MAP()


private:
	CButton m_cOutputDefaultSize;
	CButton m_cBGImage;
	CEdit m_cOutputHeight;
	CEdit m_cOutputWidth;
	CButton m_cBGColor;
	CButton m_cBGRepeat;
	CButton m_cBGStretch;


public:
	afx_msg void OnBnClickedBgImg();
	afx_msg void OnBnClickedBgColor();
	afx_msg void OnBnClickedOutSize();
	afx_msg void OnBnClickedBgImgBtn();
	afx_msg void OnBnClickedBgColorBtn();


	void GetParams(
		bool& image, 
		bool& stretch,
		bool& defaultSize,
		CString& filePath,
		int& height, 
		int& width,
		COLORREF& bg,
		bool& alwaysCalc,
		bool& inverse);

	BOOL m_bImageRepeat;
	BOOL m_bBGImage;
	BOOL m_bOutputDefaultSize;
	int m_nOutputHeight;
	int m_nOutputWidth;

	COLORREF m_backgroundColor;
	CString m_sPathName;
	BOOL m_bAlwaysCalcNormals;
	BOOL m_bInverseNormals;
	CButton m_cAlwaysCalcNormals;
	CButton m_cInverseNoramls;

};