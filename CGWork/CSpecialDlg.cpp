// CSpecialDlg.cpp : implementation file
//
#include "CSpecialDlg.h"


// CSpecialDlg dialog

IMPLEMENT_DYNAMIC(CSpecialDlg, CDialog)


CSpecialDlg::CSpecialDlg(double fogFar, double motionBlur, COLORREF fogColor, FogMode fogMode, int AAType, int AASize) :
	CDialog(CSpecialDlg::IDD),
	m_nFogFar(int(fogFar)),
	m_nMotionBlur(int(motionBlur * 100)),
	m_fogColor(fogColor),
	m_fogMode(fogMode),
	m_nAASize(AASize),
	m_nAAType(AAType)
{
}

CSpecialDlg::~CSpecialDlg()
{
}

BOOL CSpecialDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cFogFarSlider.SetRange(MIN_FOG_FAR, MAX_FOG_FAR, TRUE);
	m_cFogFarSlider.SetPos(m_nFogFar);

	m_cMotionBlurSlider.SetRange(MIN_MOTION_BLUR, MAX_MOTION_BLUR, TRUE);
	m_cMotionBlurSlider.SetPos(m_nMotionBlur);

	m_comboAASize.SetCurSel(m_nAASize);
	m_comboAAKernelType.SetCurSel(m_nAAType);

	m_cFogColor.SetCheck(m_fogMode == FogMode::Color);
	m_cFogTransparent.SetCheck(m_fogMode == FogMode::Transparent);
	m_cFogOff.SetCheck(m_fogMode == FogMode::Off);

	return TRUE;
}

void CSpecialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RADIO_FOG_COLOR, m_cFogColor);
	DDX_Control(pDX, IDC_RADIO_FOG_TRANSPARENT, m_cFogTransparent);
	DDX_Control(pDX, IDC_RADIO_FOG_OFF, m_cFogOff);
	DDX_Control(pDX, IDD_SLIDER_FOG_FAR, m_cFogFarSlider);
	DDX_Control(pDX, IDC_SLIDER_MOTIONBLUR, m_cMotionBlurSlider);

	DDX_Slider(pDX, IDC_SLIDER_MOTIONBLUR, m_nMotionBlur);
	DDX_Slider(pDX, IDD_SLIDER_FOG_FAR, m_nFogFar);
	DDX_Control(pDX, IDC_COMBO_AA_TYPE, m_comboAAKernelType);
	DDX_Control(pDX, IDC_COMBO_AA_SIZE, m_comboAASize);
}

void CSpecialDlg::GetParams(double& fogFar, double& motionBlur, COLORREF& fogColor, FogMode& fogMode, int& AAType, int& AASize)
{
	fogFar = double(m_nFogFar);
	motionBlur = double(m_nMotionBlur) / 100.0;
	fogColor = m_fogColor;
	fogMode = m_fogMode;
	AASize = m_nAASize;
	AAType = m_nAAType;
}


BEGIN_MESSAGE_MAP(CSpecialDlg, CDialog)
	ON_BN_CLICKED(IDC_FOG_COLOR_BTN, &CSpecialDlg::OnBtnClickedFogColor)
	ON_BN_CLICKED(IDC_RADIO_FOG_COLOR, &CSpecialDlg::OnBnClickedFogColor)
	ON_BN_CLICKED(IDC_RADIO_FOG_TRANSPARENT, &CSpecialDlg::OnBnClickedFogTransparent)
	ON_BN_CLICKED(IDC_RADIO_FOG_OFF, &CSpecialDlg::OnBnClickedFogOff)
	ON_CBN_SELCHANGE(IDC_COMBO_AA_SIZE, &CSpecialDlg::OnCbnSelchangeComboAaSize)
	ON_CBN_SELCHANGE(IDC_COMBO_AA_TYPE, &CSpecialDlg::OnCbnSelchangeComboAaType)
END_MESSAGE_MAP()


void CSpecialDlg::OnBnClickedFogColor()
{
	m_fogMode = FogMode::Color;
}

void CSpecialDlg::OnBtnClickedFogColor()
{
	CColorDialog colorDlg(m_fogColor);
	if (colorDlg.DoModal() == IDOK) {
		m_fogColor = colorDlg.GetColor();
	}
}

void CSpecialDlg::OnBnClickedFogTransparent()
{
	m_fogMode = FogMode::Transparent;
}


void CSpecialDlg::OnBnClickedFogOff()
{
	m_fogMode = FogMode::Off;
}

// CSpecialDlg message handlers


void CSpecialDlg::OnCbnSelchangeComboAaSize()
{
	m_nAASize = m_comboAASize.GetCurSel();
}


void CSpecialDlg::OnCbnSelchangeComboAaType()
{
	m_nAAType = m_comboAAKernelType.GetCurSel();
}
