#include "CGeneralDlg.h"


IMPLEMENT_DYNAMIC(CGeneralDlg, CDialog)


CGeneralDlg::CGeneralDlg(int tesselation,
	bool image,
	bool stretch,
	bool defaultSize,
	CString filePath,
	int height,
	int width,
	COLORREF bg,
	bool alwaysCalc,
	bool inverse)
	: CDialog(CGeneralDlg::IDD), m_tesselation(tesselation)
	, m_bBGImage(image ? FALSE : TRUE)
	, m_bImageRepeat(stretch ? FALSE : TRUE)
	, m_nOutputHeight(height)
	, m_nOutputWidth(width)
	, m_backgroundColor(bg)
	, m_sPathName(filePath)
	, m_bOutputDefaultSize(defaultSize ? TRUE : FALSE)
	, m_bAlwaysCalcNormals(alwaysCalc ? TRUE : FALSE)
	, m_bInverseNormals(inverse ? TRUE : FALSE)
{
}

CGeneralDlg::~CGeneralDlg()
{
}

void CGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Slider(pDX, IDD_SLIDER_TESSELATION, m_tesselation);

	DDX_Control(pDX, IDD_SLIDER_TESSELATION, m_tesselationSliderControl);
	DDX_Text(pDX, IDC_OUT_HEIGHT, m_nOutputHeight);
	DDX_Text(pDX, IDC_OUT_WIDTH, m_nOutputWidth);
	DDX_Radio(pDX, IDC_BG_IMG, m_bBGImage);
	DDX_Check(pDX, IDC_OUT_SIZE, m_bOutputDefaultSize);
	DDX_Control(pDX, IDC_OUT_HEIGHT, m_cOutputHeight);
	DDX_Control(pDX, IDC_OUT_WIDTH, m_cOutputWidth);
	DDX_Control(pDX, IDC_OUT_SIZE, m_cOutputDefaultSize);
	DDX_Control(pDX, IDC_BG_IMG, m_cBGImage);
	DDX_Control(pDX, IDC_BG_COLOR, m_cBGColor);
	DDX_Control(pDX, IDC_RADIO_IMG_REPEAT, m_cBGRepeat);
	DDX_Control(pDX, IDC_RADIO_IMG_STRETCH, m_cBGStretch);

	DDX_Control(pDX, IDD_SLIDER_FOG_CLOSE, m_cBGStretch);
	DDX_Control(pDX, IDD_SLIDER_FOG_FAR, m_cBGStretch);

	DDX_Radio(pDX, IDC_RADIO_IMG_REPEAT, m_bImageRepeat);

	DDX_Radio(pDX, IDC_RADIO_IMG_REPEAT, m_bImageRepeat);
	DDX_Check(pDX, IDC_CHECK_ALWAYSCALC, m_bAlwaysCalcNormals);
	DDX_Check(pDX, IDC_CHECK_INVERSENORMALS, m_bInverseNormals);
	DDX_Control(pDX, IDC_CHECK_ALWAYSCALC, m_cAlwaysCalcNormals);
	DDX_Control(pDX, IDC_CHECK_INVERSENORMALS, m_cInverseNoramls);

}

BOOL CGeneralDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	m_tesselationSliderControl.SetRange(MIN_TESSELATION, MAX_TESSELATION, TRUE);
	m_tesselationSliderControl.SetPos(m_tesselation);
	m_cOutputDefaultSize.SetCheck(m_bOutputDefaultSize);
	m_cOutputWidth.EnableWindow(m_bOutputDefaultSize ? FALSE : TRUE);
	m_cOutputHeight.EnableWindow(m_bOutputDefaultSize ? FALSE : TRUE);
	m_cBGColor.SetCheck(m_bBGImage);
	m_cBGImage.SetCheck(!m_bBGImage);
	m_cInverseNoramls.SetCheck(m_bInverseNormals);
	m_cAlwaysCalcNormals.SetCheck(m_bAlwaysCalcNormals);

	return TRUE; // return TRUE unless you set the focus to a control
}


BEGIN_MESSAGE_MAP(CGeneralDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BG_IMG, &CGeneralDlg::OnBnClickedBgImg)
	ON_BN_CLICKED(IDC_BG_COLOR, &CGeneralDlg::OnBnClickedBgColor)
	ON_BN_CLICKED(IDC_OUT_SIZE, &CGeneralDlg::OnBnClickedOutSize)
	ON_BN_CLICKED(IDC_BG_IMG_BTN, &CGeneralDlg::OnBnClickedBgImgBtn)
	ON_BN_CLICKED(IDC_BG_COLOR_BTN, &CGeneralDlg::OnBnClickedBgColorBtn)

END_MESSAGE_MAP()

void CGeneralDlg::OnBnClickedBgImg()
{
	m_bBGImage = TRUE;
}


void CGeneralDlg::OnBnClickedBgColor()
{
	m_bBGImage = FALSE;
}


void CGeneralDlg::OnBnClickedOutSize()
{
	m_bOutputDefaultSize = !m_bOutputDefaultSize;
	m_cOutputWidth.EnableWindow(!m_bOutputDefaultSize);
	m_cOutputHeight.EnableWindow(!m_bOutputDefaultSize);
}

void CGeneralDlg::OnBnClickedBgImgBtn()
{
	TCHAR szFilters[] = _T("PNG | *.png | All Files (*.*)|*.*||");

	CFileDialog fileDlg(TRUE, _T("PNG"), _T("*.png"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if (fileDlg.DoModal() == IDOK)
	{
		//Open file explorer to load a picture
		m_sPathName = fileDlg.GetPathName();
	}

}


void CGeneralDlg::OnBnClickedBgColorBtn()
{
	CColorDialog colorDlg(m_backgroundColor);
	if (colorDlg.DoModal() == IDOK) {
		m_backgroundColor = colorDlg.GetColor();
	}
}

void CGeneralDlg::GetParams(
	bool& image, 
	bool& stretch,
	bool& defaultSize,
	CString& filePath,
	int& height, 
	int& width,
	COLORREF& bg,
	bool& alwaysCalc, 
	bool& inverse)
{
	defaultSize = m_bOutputDefaultSize == TRUE;

	image = m_bBGImage == FALSE;
	stretch = m_bImageRepeat == FALSE;
	height = m_nOutputHeight;
	width = m_nOutputWidth;

	bg = m_backgroundColor;
	filePath = m_sPathName;
	alwaysCalc = m_bAlwaysCalcNormals;
	inverse = m_bInverseNormals;

}


