#pragma once
#include "stdafx.h"
#include "resource.h"

class CColorsDlg : public CDialog
{
	DECLARE_DYNAMIC(CColorsDlg)

public:
	CColorsDlg(COLORREF wireframeColor, COLORREF normalColor, COLORREF boundryColor, COLORREF silhouetteColor);
	virtual ~CColorsDlg();

	// Dialog Data
	enum { IDD = IDD_COLOR_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnBnClickedWireframe();
	void OnBnClickedNormal();
	void OnBnClickedBoundry();
	void OnBnClickedSilhouette();

	BOOL OnInitDialog();
public:

	COLORREF m_wireframeColor;
	COLORREF m_normalColor;
	COLORREF m_boundryColor;
	COLORREF m_silhouetteColor;
	DECLARE_MESSAGE_MAP()
};

