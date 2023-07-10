#include "CColorsDlg.h"

IMPLEMENT_DYNAMIC(CColorsDlg, CDialog)

CColorsDlg::CColorsDlg(COLORREF wireframeColor, COLORREF normalColor, COLORREF boundryColor, COLORREF silhouetteColor)
    : CDialog(CColorsDlg::IDD), m_wireframeColor(wireframeColor), m_normalColor(normalColor), m_boundryColor(boundryColor), m_silhouetteColor(silhouetteColor)
{
}

CColorsDlg::~CColorsDlg()
{
}

void CColorsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

}

BOOL CColorsDlg::OnInitDialog()

{
    return 0;
}

void CColorsDlg::OnBnClickedWireframe()
{
    // TODO: Add your control notification handler code here
    CColorDialog colorDlg(m_wireframeColor);
    if (colorDlg.DoModal() == IDOK) {
        m_wireframeColor = colorDlg.GetColor();
    }
}

void CColorsDlg::OnBnClickedNormal()
{
    // TODO: Add your control notification handler code here
    CColorDialog colorDlg(m_normalColor);
    if (colorDlg.DoModal() == IDOK) {
        m_normalColor = colorDlg.GetColor();
    }
}

void CColorsDlg::OnBnClickedBoundry()
{
    // TODO: Add your control notification handler code here
    CColorDialog colorDlg(m_boundryColor);
    if (colorDlg.DoModal() == IDOK) {
        m_boundryColor = colorDlg.GetColor();
    }
}

void CColorsDlg::OnBnClickedSilhouette()
{
    // TODO: Add your control notification handler code here
    CColorDialog colorDlg(m_silhouetteColor);
    if (colorDlg.DoModal() == IDOK) {
        m_silhouetteColor = colorDlg.GetColor();
    }
}

BEGIN_MESSAGE_MAP(CColorsDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_WIREFRAME_COLOR, &CColorsDlg::OnBnClickedWireframe)
    ON_BN_CLICKED(IDC_BUTTON_NORMAL_COLOR, &CColorsDlg::OnBnClickedNormal)
    ON_BN_CLICKED(IDC_BUTTON_BOUNDRY_COLOR, &CColorsDlg::OnBnClickedBoundry)
    ON_BN_CLICKED(IDC_BUTTON_SILHOUETTE_COLOR, &CColorsDlg::OnBnClickedSilhouette)
END_MESSAGE_MAP()