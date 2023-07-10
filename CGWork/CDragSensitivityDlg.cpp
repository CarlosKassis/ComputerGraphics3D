#include "CDragSensitivityDlg.h"


CDragSensitivityDlg::CDragSensitivityDlg(int scale, double rotate, double translate)
	: CDialog(CDragSensitivityDlg::IDD), m_scaleSensitivity(scale), m_rotateSensitivity(rotate), m_translateSensitivity(translate)
{
}

CDragSensitivityDlg::~CDragSensitivityDlg()
{
}

void CDragSensitivityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Slider(pDX, IDC_SCALE_SENSITIVITY, m_scaleSensitivity);
	DDX_Slider(pDX, IDC_ROTATE_SENSITIVITY, m_rotateSensitivity);
	DDX_Slider(pDX, IDC_TRANSLATE_SENSITIVITY, m_translateSensitivity);

	DDX_Control(pDX, IDC_SCALE_SENSITIVITY, m_scaleSliderControl);
	DDX_Control(pDX, IDC_ROTATE_SENSITIVITY, m_rotateSliderControl);
	DDX_Control(pDX, IDC_TRANSLATE_SENSITIVITY, m_translateSliderControl);
}

BOOL CDragSensitivityDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	m_scaleSliderControl.SetRange(MIN_SCALE_SENSITIVITY, MAX_SCALE_SENSITIVITY, TRUE);
	m_scaleSliderControl.SetPos(m_scaleSensitivity);

	m_rotateSliderControl.SetRange(MIN_ROTATE_SENSITIVITY, MAX_ROTATE_SENSITIVITY, TRUE);
	m_rotateSliderControl.SetPos(m_rotateSensitivity);

	m_translateSliderControl.SetRange(MIN_TRANSLATE_SENSITIVITY, MAX_TRANSLATE_SENSITIVITY, TRUE);
	m_translateSliderControl.SetPos(m_translateSensitivity);

	return TRUE; // return TRUE unless you set the focus to a control
}

IMPLEMENT_DYNAMIC(CDragSensitivityDlg, CDialog)

BEGIN_MESSAGE_MAP(CDragSensitivityDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()