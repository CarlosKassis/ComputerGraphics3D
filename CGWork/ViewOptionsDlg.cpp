#include "ViewOptionsDlg.h"
#include "Globals.h"

IMPLEMENT_DYNAMIC(CPersOptionsDlg, CDialog)

BEGIN_MESSAGE_MAP(CPersOptionsDlg, CDialog)

END_MESSAGE_MAP()

CPersOptionsDlg::CPersOptionsDlg(int closeClip, double farClip, double d, double orthographicSize)
	: CDialog(CPersOptionsDlg::IDD), Close(closeClip), Far(farClip), D(d), OrthographicSize(orthographicSize)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

CPersOptionsDlg::~CPersOptionsDlg()
{
}

BOOL CPersOptionsDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CloseSlider.SetRange(MIN_CLOSE_CLIP, MAX_CLOSE_CLIP, TRUE);
	CloseSlider.SetPos(Close);

	FarSlider.SetRange(MIN_FAR_CLIP, MAX_FAR_CLIP, TRUE);
	FarSlider.SetPos(Far);

	DSlider.SetRange(MIN_D, MAX_D, TRUE);
	DSlider.SetPos(D);

	OrthographicSizeSlider.SetRange(MIN_ORTHOGRAPHIC_SIZE, MAX_ORTHOGRAPHIC_SIZE, TRUE);
	OrthographicSizeSlider.SetPos(OrthographicSize);

	return TRUE; // return TRUE unless you set the focus to a control
}

void CPersOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Slider(pDX, IDC_SLIDER_CLOSE, Close);
	DDX_Slider(pDX, IDC_SLIDER_FAR, Far);
	DDX_Slider(pDX, IDC_SLIDER_D, D);
	DDX_Slider(pDX, IDC_ORTHOGRAPHIC_SIZE, OrthographicSize);

	DDX_Control(pDX, IDC_SLIDER_CLOSE, CloseSlider);
	DDX_Control(pDX, IDC_SLIDER_FAR, FarSlider);
	DDX_Control(pDX, IDC_SLIDER_D, DSlider);
	DDX_Control(pDX, IDC_ORTHOGRAPHIC_SIZE, OrthographicSizeSlider);
}

