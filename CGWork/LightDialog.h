#pragma once

#include "Light.h"
// CLightDialog dialog



class CLightDialog : public CDialog
{
	DECLARE_DYNAMIC(CLightDialog)

public:
	CLightDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLightDialog();

	//dialog interface
	void SetDialogData(LightID id,const LightParams& light);
	void SetDialogGlobals(double ambientIntensity, double diffuseIntensity, double specuralIntensity, int specuralExponent);
	LightParams GetDialogData(LightID id);
	void GetDialogGlobals(double& ambientIntensity, double& diffuseIntensity, double& specuralIntensity, int& specuralExponent);

// Dialog Data
	enum { IDD = IDD_LIGHTS_DLG };

protected:
	LightParams m_lights[MAX_LIGHT];
	LightParams m_ambiant;
	int m_currentLightIdx;
	int GetCurrentLightIndex();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedRadioLight();
//    afx_msg void On();
    virtual BOOL OnInitDialog();
protected:
	double m_dAmbientIntensity;
	double m_dDiffuseIntensity;
	double m_dSpecularIntensity;
	int m_nSpecularExponent;
};
