// CGWorkView.cpp : implementation of the CCGWorkView class
//

#include <omp.h>
#include <vector>
#include "stdafx.h"
#include "CGWork.h"

#include "CGWorkDoc.h"
#include "CGWorkView.h"

#include <iostream>
#include <fstream>
#include "Light.h"

using std::cout;
using std::endl;


#include "MaterialDlg.h"
#include "LightDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PngWrapper.h"
#include "iritSkel.h"
#include "stdafx.h"
#include "ViewOptionsDlg.h"
#include "CDragSensitivityDlg.h"
#include <chrono>
#include <math.h>
#include "MatUtils.h"
// For Status Bar access
#include "MainFrm.h"
#include "CGeneralDlg.h"
#include "CColorsDlg.h"
#include "CSpecialDlg.h"

// Use this macro to display text messages in the status bar.
#define STATUS_BAR_TEXT(str) (((CMainFrame*)GetParentFrame())->getStatusBar().SetWindowText(str))


using std::vector;

extern std::vector<GameObject> g_gameObjects;

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView

IMPLEMENT_DYNCREATE(CCGWorkView, CView)

BEGIN_MESSAGE_MAP(CCGWorkView, CView)
	//{{AFX_MSG_MAP(CCGWorkView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_VIEW_ORTHOGRAPHIC, OnViewOrthographic)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORTHOGRAPHIC, OnUpdateViewOrthographic)
	ON_COMMAND(ID_VIEW_PERSPECTIVE, OnViewPerspective)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PERSPECTIVE, OnUpdateViewPerspective)
	ON_COMMAND(ID_ACTION_ROTATE, OnActionRotate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ROTATE, OnUpdateActionRotate)
	ON_COMMAND(ID_ACTION_SCALE, OnActionScale)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SCALE, OnUpdateActionScale)
	ON_COMMAND(ID_ACTION_TRANSLATE, OnActionTranslate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_TRANSLATE, OnUpdateActionTranslate)
	ON_COMMAND(ID_AXIS_X, OnAxisX)
	ON_UPDATE_COMMAND_UI(ID_AXIS_X, OnUpdateAxisX)
	ON_COMMAND(ID_AXIS_Y, OnAxisY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Y, OnUpdateAxisY)
	ON_COMMAND(ID_AXIS_Z, OnAxisZ)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Z, OnUpdateAxisZ)
	ON_COMMAND(ID_AXIS_XY, OnAxisXY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_XY, OnUpdateAxisXY)
	ON_COMMAND(ID_WORLD_SPACE, OnWorldSpace)
	ON_UPDATE_COMMAND_UI(ID_WORLD_SPACE, OnUpdateWorldSpace)
	ON_COMMAND(ID_OBJECT_SPACE, OnObjectSpace)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_SPACE, OnUpdateObjectSpace)
	ON_COMMAND(ID_LIGHT_SHADING_FLAT, OnLightShadingFlat)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_FLAT, OnUpdateLightShadingFlat)
	ON_COMMAND(ID_LIGHT_SHADING_GOURAUD, OnLightShadingGouraud)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_GOURAUD, OnUpdateLightShadingGouraud)
	ON_COMMAND(ID_LIGHT_CONSTANTS, OnLightConstants)
	ON_COMMAND(ID_OPTIONS_PERSPECTIVECONTROL, OnPersOptions)
	ON_COMMAND(ID_OPTIONS_MOUSESENSITIVITY, OnMouseSensitivityOptions)
	ON_COMMAND(ID_OPTIONS_GENERAL, OnGeneralOptions)
	ON_COMMAND(ID_OPTIONS_COLORS, OnColorsOptions)
	ON_COMMAND(ID_BUTTON_TO_FILE, OnRenderToFile)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_TO_FILE, OnUpdateRender)
	ON_COMMAND(ID_BUTTON_TO_SCREEN, OnRenderToScreen)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_TO_SCREEN, OnUpdateRender)
	ON_COMMAND(ID_BUTTON_ALWAYS_RENDER, OnAlwaysRender)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ALWAYS_RENDER, OnUpdateAlwaysRender)
	ON_COMMAND(ID_BUTTON_RECORD_TOGGLE, OnRecordToggle)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_RECORD_TOGGLE, OnUpdateRecordToggle)
	ON_COMMAND(ID_BUTTON_REPLAY, OnReplayToggle)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_REPLAY, OnUpdateReplayToggle)
	ON_COMMAND(ID_BUTTON_REPEAT_REPLAY, OnRepeatReplayToggle)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_REPEAT_REPLAY, OnUpdateRepeatReplayToggle)

	ON_COMMAND(ID_BUTTON_REPLAY_SPEED_HALF, OnReplaySpeedHalf)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_REPLAY_SPEED_HALF, OnUpdateReplaySpeedHalf)
	ON_COMMAND(ID_BUTTON_REPLAY_SPEED_1, OnReplaySpeed1)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_REPLAY_SPEED_1, OnUpdateReplaySpeed1)
	ON_COMMAND(ID_BUTTON_REPLAY_SPEED_2, OnReplaySpeed2)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_REPLAY_SPEED_2, OnUpdateReplaySpeed2)
	ON_COMMAND(ID_BUTTON_REPLAY_SPEED_4, OnReplaySpeed4)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_REPLAY_SPEED_4, OnUpdateReplaySpeed4)
	ON_COMMAND(ID_BUTTON_VIDEO_TOGGLE, OnRenderVideoToggle)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_VIDEO_TOGGLE, OnUpdateRenderVideoToggle)

	ON_COMMAND(ID_NORMALS_REVERSENORMALS, OnInvertCalculatedNormals)
	ON_COMMAND(ID_NORMALS_SHOWCALCULATEDNORMALS, OnUseCalculatedNormals)
	ON_COMMAND(ID_NORMALS_SHOWIMPORTEDNORMALS, OnUseImportedNormals)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()

	ON_COMMAND(ID_AXIS_BOUNDXYZ, &CCGWorkView::OnAxisBoundxyz)
	ON_UPDATE_COMMAND_UI(ID_AXIS_BOUNDXYZ, &CCGWorkView::OnUpdateAxisBoundxyz)
	ON_COMMAND(ID_OPTIONS_COLOURS, &CCGWorkView::OnOptionsColours)
	ON_COMMAND(ID_VIEW_SHOWPOLYGONNORMALS, &CCGWorkView::OnViewShowpolygonnormals)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWPOLYGONNORMALS, &CCGWorkView::OnUpdateViewShowpolygonnormals)
	ON_COMMAND(ID_VIEW_SHOWNORMALS, &CCGWorkView::OnViewShownormals)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWNORMALS, &CCGWorkView::OnUpdateViewShownormals)
	ON_COMMAND(ID_VIEW_SHOWBOUNDINGBOX, &CCGWorkView::OnShowBoundingbox)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWBOUNDINGBOX, &CCGWorkView::OnUpdateShowBoundingbox)
	ON_COMMAND(ID_VIEW_SHOWSILHOUETTE, &CCGWorkView::OnShowSilhouette)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWSILHOUETTE, &CCGWorkView::OnUpdateShowSilhouette)
	ON_WM_TIMER()
	ON_COMMAND(ID_SHADING_PHONG, &CCGWorkView::OnLightShadingPhong)
	ON_UPDATE_COMMAND_UI(ID_SHADING_PHONG, &CCGWorkView::OnUpdateLightShadingPhong)
	ON_COMMAND(ID_VIEW_ENABLEBACKFACECULLING, &CCGWorkView::OnViewEnablebackfaceculling)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ENABLEBACKFACECULLING, &CCGWorkView::OnUpdateViewEnablebackfaceculling)
	ON_COMMAND(ID_OPTIONS_SPECIALEFFECT, &CCGWorkView::OnOptionsSpecialeffect)
END_MESSAGE_MAP()


// A patch to fix GLaux disappearance from VS2005 to VS2008
void auxSolidCone(GLdouble radius, GLdouble height) {
        GLUquadric *quad = gluNewQuadric();
        gluQuadricDrawStyle(quad, GLU_FILL);
        gluCylinder(quad, radius, 0.0, height, 20, 20);
        gluDeleteQuadric(quad);
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView construction/destruction

bool g_prevMousePositionInitialized = false;
bool g_prevMouseDownL = false;
bool g_prevMouseDownM = false;
bool g_prevMouseDownR = false;
int g_mouseSkipFramesAfterOverlap = 0;
CPoint g_prevMousePosition;
CPoint g_currentMousePosition;

CCGWorkView::CCGWorkView()
{
	// Set default values
	m_nAxis = ID_AXIS_X;
	m_nAction = ID_ACTION_ROTATE;

	m_nLightShading = ID_LIGHT_SHADING_FLAT;

	m_lMaterialAmbient = 0.2;
	m_lMaterialDiffuse = 0.8;
	m_lMaterialSpecular = 1.0;
	m_nMaterialCosineFactor = 32;

	//init the first light to be enabled
	double direction[3] = { 0.0, 0.0, 1.0 };
	m_lights[LIGHT_ID_1].enabled = true;
	m_lights[LIGHT_ID_1].type = LIGHT_TYPE_DIRECTIONAL;
	m_lights[LIGHT_ID_1].dirX = direction[0];
	m_lights[LIGHT_ID_1].dirY = direction[1];
	m_lights[LIGHT_ID_1].dirZ = direction[2];

	m_lights[LIGHT_ID_1].posX = 0.0;
	m_lights[LIGHT_ID_1].posY = 0.0;
	m_lights[LIGHT_ID_1].posZ = -3.0;
	m_lights[LIGHT_ID_1].angle = 45.0;

	Light::GetLightsInstance();
	SetLightsData();
	m_pDbBitMap = NULL;
	m_pDbDC = NULL;
}

void CCGWorkView::SetMousePosition(Vector3D& positionInViewPixels)
{
	CRect rect;
	GetWindowRect(&rect);
	
	SetCursorPos(rect.left + positionInViewPixels.X, rect.top + positionInViewPixels.Y);
	g_prevMousePosition.x = positionInViewPixels.X;
	g_prevMousePosition.y = positionInViewPixels.Y;
	g_mouseSkipFramesAfterOverlap = 3;
}

// Offset [0 - 1] relative to the screen
void CCGWorkView::OnMouseDragL(Vector3D& offset)
{
	if (m_clicking)
	{
		return;
	}

	const double edgeDragMargin = 0.1; // Relative to whole view height [0 - 1]
	if (g_currentMousePosition.y > m_viewBuffer.Height() * (1.0 - edgeDragMargin) && offset.Y > 0.0)
	{
		SetMousePosition(Vector3D(g_currentMousePosition.x, g_currentMousePosition.y - m_viewBuffer.Height() * (1 - 3 * edgeDragMargin), 0.0));
	}
	else if (g_currentMousePosition.y < m_viewBuffer.Height() * edgeDragMargin && offset.Y < 0.0)
	{
		SetMousePosition(Vector3D(g_currentMousePosition.x, g_currentMousePosition.y + m_viewBuffer.Height() * (1 - 3 * edgeDragMargin), 0.0));
	}

	if (m_animator.IsReplaying())
		return;

	bool shouldTransformInAxis[3] =
	{
		m_nAxis == ID_AXIS_X || m_nAxis == ID_AXIS_XY || m_nAxis == ID_AXIS_BOUNDXYZ,
		m_nAxis == ID_AXIS_Y || m_nAxis == ID_AXIS_XY || m_nAxis == ID_AXIS_BOUNDXYZ,
		m_nAxis == ID_AXIS_Z || m_nAxis == ID_AXIS_BOUNDXYZ
	};

	double actionAmount = -offset.Y;

	for (int i = 0; i < g_gameObjects.size(); i++)
	{
		GameObject& gameObject = g_gameObjects[i];
		if (!ActionsApplyToGameObject(gameObject))
		{
			continue;
		}

		for (int axis = 0; axis < 3; axis++)
		{
			if (!shouldTransformInAxis[axis])
				continue;

			Action action;
			switch (m_nAction)
			{
				case(ID_ACTION_ROTATE):
				{
					double rotationAmount = actionAmount * (M_PI / 180.0) * m_nRotateSensitivity;
					action = Action(i, ActionType::Rotate, m_transformationType, axis, rotationAmount);
					break;
				}
				case(ID_ACTION_TRANSLATE):
				{
					double translationAmount = actionAmount * 0.2 * m_nTranslateSensitivity;
					action = Action(i, ActionType::Translate, m_transformationType, axis, translationAmount);
					break;
				}
				case(ID_ACTION_SCALE):
				{
					double scaleAmount = actionAmount * m_nScaleSensitivity;
					action = Action(i, ActionType::Scale, m_transformationType, axis, scaleAmount);
					break;
				}
				default:
					break;
			}

			if (m_animator.IsRecording())
			{
				m_animator.AddActionIfRecording(action);
			}

			DoAction(action);
		}
	}

	Invalidate();
}

// Offset [0 - 1] relative to the screen
void CCGWorkView::OnMouseDragR(Vector3D& offset)
{
	m_lights[0].posX += offset.X * 10.0;
	m_lights[0].posZ -= offset.Y * 10.0;
	Light::SetLight(0, Light(m_lights[0]));
	Invalidate();
}

#define PLUS_KEY 187
#define MINUS_KEY 189


void CCGWorkView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool somebodyGotChanged = false;
	// Transparency
	if (PLUS_KEY == nChar || MINUS_KEY == nChar)
	{
		double transparencyOffset = PLUS_KEY == nChar ? 0.1 : -0.1;

		if (IsAnyGameObjectSelected()) {
			for (auto& gameObject : g_gameObjects) {
				if (gameObject.Selected) {
					somebodyGotChanged = true;
					gameObject.AddTransparency(transparencyOffset);
				}
			}
		}
		else
		{
			somebodyGotChanged = true;
			for (auto& gameObject : g_gameObjects) {
				gameObject.AddTransparency(transparencyOffset);
			}
		}

		if (somebodyGotChanged)
		{
			Invalidate();
		}
	}
}

// Offset [0 - 1] relative to the screen
void CCGWorkView::OnMouseDragM(Vector3D& offset)
{
}

bool pnpoly(int nvert, double* vertx, double* verty, float testx, float testy)
{
	int i, j;
	bool c = false;
	for (i = 0, j = nvert - 1; i < nvert; j = i++)
	{
		if (((verty[i] > testy) != (verty[j] > testy)) && (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
		{
			c = !c;
		}
	}

	return c;
}

void CCGWorkView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_clicking)
		return;
	m_clicking = false;

	double closestZ = DBL_MAX;
	GameObject* clickedGameObject = nullptr;
	for (int gameObjectIndex = 0; gameObjectIndex < g_gameObjects.size(); gameObjectIndex++)
	{
		for (int polygonIndex = 0; polygonIndex < g_gameObjects[gameObjectIndex].Polygons.size(); polygonIndex++)
		{
			Polygon3D& polygon = g_gameObjects[gameObjectIndex].Polygons[polygonIndex];
			double coordsX[MAX_POLYGON_VERTICES] = { 0 };
			double coordsY[MAX_POLYGON_VERTICES] = { 0 };
			double coordsZ[MAX_POLYGON_VERTICES] = { 0 };
			for (int i = 0; i < polygon.Vertices.size(); i++)
			{
				Vector3D vertexWorldPosition = m_Camera.WorldToScreen(g_gameObjects[gameObjectIndex].AbsoluteToWorld(polygon.Vertices[i]));

				coordsX[i] = vertexWorldPosition.X;
				coordsY[i] = vertexWorldPosition.Y;
				coordsZ[i] = vertexWorldPosition.Z;
			}

			if (!pnpoly(polygon.Vertices.size(), coordsX, coordsY, point.x, point.y))
			{
				continue;
			}

			if (polygon.Center.Z < closestZ)
			{
				closestZ = polygon.Center.Z;
				clickedGameObject = &g_gameObjects[gameObjectIndex];
			}
		}
	}

	// Unselected all GameObjects if clicked on empty space
	if (clickedGameObject == nullptr)
	{
		for (int gameObjectIndex = 0; gameObjectIndex < g_gameObjects.size(); gameObjectIndex++)
		{
			g_gameObjects[gameObjectIndex].Selected = false;
		}

		Invalidate();
		return;
	}

	// Toggle gameObject
	clickedGameObject->Selected = !clickedGameObject->Selected;

	Invalidate();
}

void CCGWorkView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_downPositionL = point;
	m_clicking = true;
}

void CCGWorkView::OnRButtonDown(UINT nFlags, CPoint point)
{

}

void CCGWorkView::OnMButtonDown(UINT nFlags, CPoint point)
{
	bool anySelected = IsAnyGameObjectSelected();
	bool dirtyScene = false;
	for (GameObject& gameObject : g_gameObjects)
	{
		if (!anySelected || gameObject.Selected)
		{
			dirtyScene = true;
			gameObject.ResetTransformations();
		}
	}

	if (dirtyScene)
	{
		Invalidate();
	}
}

void CCGWorkView::OnMouseMove(UINT nFlags, CPoint point)
{
	g_currentMousePosition = point;
	if (!g_prevMousePositionInitialized)
	{
		g_prevMousePositionInitialized = true;
		g_prevMousePosition = point;
		return;
	}

	if (g_mouseSkipFramesAfterOverlap-- > 0)
	{
		g_prevMousePosition = point;
		return;
	}

	typedef void (CCGWorkView::* OnMouseDrag)(Vector3D&);
	struct MouseDrag
	{
		int Button;
		bool* ButtonPrevMouseDown;
		OnMouseDrag Handler;
	};
	struct MouseDrag actionHandlers[3] = { { MK_LBUTTON, &g_prevMouseDownL, &CCGWorkView::OnMouseDragL }, {MK_MBUTTON, &g_prevMouseDownM, &CCGWorkView::OnMouseDragM},{MK_RBUTTON, &g_prevMouseDownR, &CCGWorkView::OnMouseDragR }};
	for (auto actionHandler : actionHandlers)
	{
		if ((nFlags & actionHandler.Button))
		{
			if (!(*actionHandler.ButtonPrevMouseDown))
			{
				*actionHandler.ButtonPrevMouseDown = true;
			}
			else
			{
				if (m_clicking)
				{
					if (CLICK_THRESHOLD_DISTANCE < sqrt(pow(point.x - m_downPositionL.x, 2) + pow(point.y - m_downPositionL.y, 2)))
					{
						m_clicking = false;
					}
				}
				auto func = actionHandler.Handler;
				Vector3D offset = Vector3D(point.x, point.y, 0.0) - Vector3D(g_prevMousePosition.x, g_prevMousePosition.y, 0.0);
				offset.X /= m_viewBuffer.Width();
				offset.Y /= m_viewBuffer.Height();
				(this->*func)(offset);
			}
		}
		else
		{
			*actionHandler.ButtonPrevMouseDown = false;
		}
	}

	g_prevMousePosition = point;
}

CCGWorkView::~CCGWorkView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView diagnostics

#ifdef _DEBUG
void CCGWorkView::AssertValid() const
{
	CView::AssertValid();
}

void CCGWorkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGWorkDoc* CCGWorkView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGWorkDoc)));
	return (CCGWorkDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView Window Creation - Linkage of windows to CGWork

BOOL CCGWorkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// An CGWork window must be created with the following
	// flags and must NOT include CS_PARENTDC for the
	// class style.

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}



int CCGWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitializeCGWork();
	return 0;
}


// This method initialized the CGWork system.
BOOL CCGWorkView::InitializeCGWork()
{
	m_pDC = new CClientDC(this);
	
	if ( NULL == m_pDC ) { // failure to get DC
		::AfxMessageBox(CString("Couldn't get a valid DC."));
		return FALSE;
	}

	m_frameTime.Start();
	SetTimer(1, 1, NULL);

	CRect r;
	GetClientRect(&r);
	m_Camera.SetMode(m_nView);

	m_pDbDC = new CDC();
	m_pDbDC->CreateCompatibleDC(m_pDC);
	SetTimer(1, 1, NULL);
	m_pDbBitMap = CreateCompatibleBitmap(m_pDC->m_hDC, r.Width(), r.Height());
	m_pDbDC->SelectObject(m_pDbBitMap);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView message handlers


void CCGWorkView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if ( 0 >= cx || 0 >= cy ) {
		return;
	}

	// save the width and height of the current window
	m_WindowWidth = cx;
	m_WindowHeight = cy;

	// compute the aspect ratio
	// this will keep all dimension scales equal
	m_AspectRatio = (GLdouble)m_WindowWidth/(GLdouble)m_WindowHeight;

	CRect r;
	GetClientRect(&r);
	DeleteObject(m_pDbBitMap);
    m_pDbBitMap = CreateCompatibleBitmap(m_pDC->m_hDC, r.Width(), r.Height());
	m_pDbDC->SelectObject(m_pDbBitMap);
}


BOOL CCGWorkView::SetupViewingFrustum(void)
{
    return TRUE;
}


// This viewing projection gives us a constant aspect ration. This is done by
// increasing the corresponding size of the ortho cuube.
BOOL CCGWorkView::SetupViewingOrthoConstAspect(void)
{
	return TRUE;
}


BOOL CCGWorkView::OnEraseBkgnd(CDC* pDC) 
{
	// Windows will clear the window with the background color every time your window 
	// is redrawn, and then CGWork will clear the viewport with its own background color.

	
	return true;
}

int boxLinePointPairs[12][2][3] =
{
	{ { 0, 0, 0}, { 0, 0, 1} },
	{ { 0, 0, 0}, { 0, 1, 0} },
	{ { 0, 0, 0}, { 1, 0, 0} },

	{ { 1, 1, 1}, { 1, 1, 0} },
	{ { 1, 1, 1}, { 1, 0, 1} },
	{ { 1, 1, 1}, { 0, 1, 1} },

	{ { 1, 0, 0}, { 1, 0, 1} },
	{ { 1, 0, 1}, { 0, 0, 1} },
	{ { 0, 0, 1}, { 0, 1, 1} },

	{ { 0, 1, 1}, { 0, 1, 0} },
	{ { 0, 1, 0}, { 1, 1, 0} },
	{ { 1, 1, 0}, { 1, 0, 0} }
};

void CCGWorkView::DrawBoundingBox(BresenhamDraw& pen, GameObject& gameObject)
{
	double minVertexCoord[4] = { gameObject.MinVertexCoord[0], gameObject.MinVertexCoord[1], gameObject.MinVertexCoord[2], 1 };
	double maxVertexCoord[4] = { gameObject.MaxVertexCoord[0], gameObject.MaxVertexCoord[1], gameObject.MaxVertexCoord[2], 1 };
	for (int i = 0; i < 12; i++)
	{
		double startX = boxLinePointPairs[i][0][0] == 0 ? minVertexCoord[0] : maxVertexCoord[0];
		double startY = boxLinePointPairs[i][0][1] == 0 ? minVertexCoord[1] : maxVertexCoord[1];
		double startZ = boxLinePointPairs[i][0][2] == 0 ? minVertexCoord[2] : maxVertexCoord[2];

		
		Vector3D start = m_Camera.WorldToScreen(gameObject.AbsoluteToWorld(Vector3D(startX, startY, startZ)));

		double endX = boxLinePointPairs[i][1][0] == 0 ? minVertexCoord[0] : maxVertexCoord[0];
		double endY = boxLinePointPairs[i][1][1] == 0 ? minVertexCoord[1] : maxVertexCoord[1];
		double endZ = boxLinePointPairs[i][1][2] == 0 ? minVertexCoord[2] : maxVertexCoord[2];
		Vector3D end = m_Camera.WorldToScreen(gameObject.AbsoluteToWorld(Vector3D(endX, endY, endZ)));

		pen.MoveTo(start[0], start[1], start[2]);
		pen.LineTo(end[0], end[1], end[2], m_nBoundryColor, gameObject);
	}
	return;
}

void CCGWorkView::DrawPolygonNormals(BresenhamDraw& pen, GameObject& gameObject)
{
	const double normalLengthMultiplier = 0.1;

	int polygonCount = gameObject.Polygons.size();
#pragma omp parallel
	{
#pragma omp for nowait
		for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++)
		{
			BresenhamDraw myPen(pen); // Create a pen for each thread
			Polygon3D& polygon = gameObject.Polygons[polygonIndex];

			Vector3D polygonNormalStart = m_Camera.WorldToScreen(gameObject.AbsoluteToWorld(polygon.Center));
			Vector3D polygonNormalEnd = m_Camera.WorldToScreen(gameObject.AbsoluteToWorld((polygon.Center + (polygon.Normal() * normalLengthMultiplier))));

			myPen.MoveTo(polygonNormalStart.X, polygonNormalStart.Y, polygonNormalStart.Z);
			myPen.LineTo(polygonNormalEnd.X, polygonNormalEnd.Y, polygonNormalEnd.Z, m_nNormalColor, gameObject, true);
		}
	}
}

void CCGWorkView::DrawVertexNormals(BresenhamDraw& pen, GameObject& gameObject)
{
	const double normalLengthMultiplier = 0.1;

	int polygonCount = gameObject.Polygons.size();
	// Draw first vertex normal and last vertex normal
#pragma omp parallel
	{
#pragma omp for nowait
		for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++)
		{
			BresenhamDraw myPen(pen); // Create a pen for each thread
			for (Vector3D& vertex : gameObject.Polygons[polygonIndex].Vertices)
			{
				Vector3D vertexNormalStart = m_Camera.WorldToScreen(gameObject.AbsoluteToWorld(vertex));
				Vector3D vertexNormalEnd = m_Camera.WorldToScreen(gameObject.AbsoluteToWorld((vertex + (vertex.Normal() * normalLengthMultiplier))));

				myPen.MoveTo(vertexNormalStart.X, vertexNormalStart.Y, vertexNormalStart.Z);
				myPen.LineTo(vertexNormalEnd.X, vertexNormalEnd.Y, vertexNormalEnd.Z, m_nNormalColor, gameObject, true);
			}
		}
	}
}

void CCGWorkView::DrawWireframe(BresenhamDraw &pen, GameObject& gameObject, COLORREF color, bool force)
{
	COLORREF selectionColor = RGB(255, 255, 0);
	int polygonCount = gameObject.Polygons.size();
	{
		for (int i = 0; i < polygonCount; i++)
		{
			Polygon3D& polygon = gameObject.Polygons[i];
			COLORREF color = gameObject.Selected ? selectionColor : polygon.Color.ColorRGB();
			if (m_bBackFaceCulling && IsPolygonBackFacing(gameObject, polygon, m_Camera))
			{
				continue;
			}

			BresenhamDraw myPen(pen); // Create a pen for each thread

			Vector3D firstVertexPosition = m_Camera.WorldToScreen(gameObject.AbsoluteToWorld(polygon.Vertices[0]));
			myPen.MoveTo(firstVertexPosition[0], firstVertexPosition[1], firstVertexPosition[2]);

			for (int i = 1; i < polygon.Vertices.size(); i++)
			{
				Vector3D vertexPosition = m_Camera.WorldToScreen(gameObject.AbsoluteToWorld(polygon.Vertices[i]));
				myPen.LineTo(vertexPosition[0], vertexPosition[1], vertexPosition[2], color, gameObject, force);
			}

			myPen.LineTo(firstVertexPosition[0], firstVertexPosition[1], firstVertexPosition[2], color, gameObject, force);
		}
	}
}

void CCGWorkView::DrawPolygons(ScanConversion::ScanConversion& scan, GameObject& gameObject)
{
	int polyCount = gameObject.Polygons.size();
#pragma omp parallel
	{
#pragma omp for nowait
		for (int i = 0; i < polyCount; i++)
		{
			Polygon3D& polygon = gameObject.Polygons[i];

			Vector3D polygonWorldPosition = gameObject.AbsoluteToWorld(polygon.Center);
			Vector3D polygonScreenPosition = m_Camera.WorldToScreen(polygonWorldPosition);
				CString dotProductString;
			dotProductString.Format(L"(%.2lf,%.2lf,%.2lf), (%.2lf,%.2lf,%.2lf)",
				polygonWorldPosition.X, polygonWorldPosition.Y, polygonWorldPosition.Z,
				polygonScreenPosition.X, polygonScreenPosition.Y, polygonScreenPosition.Z);
			//STATUS_BAR_TEXT(dotProductString);

			if (m_bBackFaceCulling && IsPolygonBackFacing(gameObject, polygon, m_Camera))
			{
				continue;
			}

			scan.DrawPolygon(polygon, RGB(255, 255, 255), m_Camera, gameObject);
		}
	}
}	

COLORREF GetPolygonColor(IPPolygonStruct* polygon)
{
	int colorArray[3] = { 0 };
	auto attr = polygon->PVertex->Attr;
	while (attr != nullptr) {
		if (attr->_AttribID == IRIT_ATTR_ID_rgb) {
			char* new_char = strdup(attr->U.Str);
			char* ptr = strtok(new_char, ",");
			colorArray[0] = atoi(ptr);
			ptr = strtok(NULL, ",");
			colorArray[1] = atoi(ptr);
			ptr = strtok(NULL, ",");
			colorArray[2] = atoi(ptr);
			free(new_char);
			break;
		}
		attr = attr->Pnext;
	}

	return RGB(colorArray[0], colorArray[1], colorArray[2]);
}

void Log(std::string &message)
{
}

void CCGWorkView::DoAction(Action& action)
{
	if (action.actionType == ActionType::NOP)
		return;

	GameObject& gameObject = g_gameObjects[action.gameObjectIndex];
	if (action.actionType == ActionType::Translate)
	{
		gameObject.Translate(action.axis, action.amount, action.space);
	}
	else if (action.actionType == ActionType::Rotate)
	{
		gameObject.Rotate(action.axis, action.amount, action.space);
	}
	else if (action.actionType == ActionType::Scale)
	{
		gameObject.AddScale(action.axis, action.amount, action.space);
	}
}

bool CCGWorkView::IsPolygonBackFacing(GameObject& gameObject, Polygon3D& polygon, Camera& camera)
{
	const Vector3D& vertex0 = camera.WorldToScreen(gameObject.AbsoluteToWorld(polygon.Vertices[0]));
	const Vector3D& vertex1 = camera.WorldToScreen(gameObject.AbsoluteToWorld(polygon.Vertices[1]));
	const Vector3D& vertex2 = camera.WorldToScreen(gameObject.AbsoluteToWorld(polygon.Vertices[2]));
	double value = ((vertex1.X - vertex0.X) * (vertex2.Y - vertex1.Y)) - ((vertex1.Y - vertex0.Y) * (vertex2.X - vertex1.X));
	
	if (polygon.NormalType == NormalType::Calculated && !polygon.IsCalculatedNormalInverted())
	{
		value = -value;
	}

	return value <= 0.01;
}

void CCGWorkView::RenderGameObjects(CRect& rect)
{
	BresenhamDraw draw(&m_viewBuffer);

	const int margin = 20;
	draw.SetLimits(0, rect.Height(), rect.Width(), 0, m_Camera.GetClose(), m_Camera.GetFar());

	ScanConversion::ScanConversion scanConversion(&m_viewBuffer);
	scanConversion.SetShadingMode(m_nShadingMode);
	scanConversion.SetLimits(m_Camera.GetClose(), m_Camera.GetFar());

	for (GameObject& gameObject : g_gameObjects)
	{
		if (m_bRender || m_bRenderToFile)
		{
			DrawPolygons(scanConversion, gameObject);
		}
		else
		{
			if (gameObject.Selected)
			{
				DrawWireframe(draw, gameObject, RGB(255, 255, 0));
			}
			else
			{
				DrawWireframe(draw, gameObject, m_nWireframeColor);
			}
		}

		if (m_bShowPolygonNormals)
		{
			DrawPolygonNormals(draw, gameObject);
		}

		if (m_bShowVertexNormals)
		{
			DrawVertexNormals(draw, gameObject);
		}

		if (m_bShowBoundingBox)
		{
			DrawBoundingBox(draw, gameObject);
		}
	}
}

bool CCGWorkView::IsRenderingVideoToFileAndNotDefaultResolution()
{
	return m_bRenderVideo && m_animator.IsReplaying() && !m_bDefaultSize;
}

void ClockBenchmarkAndAppendResult(Benchmark& benchmark, CString& statusBarText, WCHAR* title)
{
	CString result;
	result.Format(L" %s: %.2lfms,", title, benchmark.ElapsedMilliseconds());
	statusBarText.Append(result);
	benchmark.Start();
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView drawing
/////////////////////////////////////////////////////////////////////////////
void CCGWorkView::OnDraw(CDC* pDC)
{
	CCGWorkDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CString statusBarText;
	CString dummyString;

	auto frameTime = m_frameTime.ElapsedMilliseconds();
	m_frameTime.Start();

	m_viewBufferLock.lock();

	CRect r;
	GetClientRect(&r);

	Benchmark benchmark;

	if (!m_bDefaultSize && m_bRenderToFile || IsRenderingVideoToFileAndNotDefaultResolution())
	{
		r.left = r.top = 0;
		r.right = m_nRenderWidth;
		r.bottom = m_nRenderHeight;
	}

	m_viewBuffer.Resize(r.Width(), r.Height());

	ClockBenchmarkAndAppendResult(benchmark, statusBarText, L"resize");

	m_viewBuffer.ClearZBuffer();
	m_viewBuffer.ClearGameObjects();
	m_viewBuffer.RenderBackground();
	m_viewBuffer.SetLimits(m_Camera.GetClose(), m_Camera.GetFar());

	m_Camera.SetDimensions(r.Height(), r.Width());

	ClockBenchmarkAndAppendResult(benchmark, statusBarText, L"start");

	if (m_bAlwaysRender)
	{
		m_bRender = true;
	}

	RenderGameObjects(r);

	if (m_bShowSilhouette) {
		m_viewBuffer.DrawSilhouette(m_nSilhouetteColor);
	}

	ClockBenchmarkAndAppendResult(benchmark, statusBarText, L"render");

	m_viewBuffer.DrawColorArray();

	ClockBenchmarkAndAppendResult(benchmark, statusBarText, L"colorArray");

	m_viewBuffer.DoMotionBlur(m_dMotionBlur);

	ClockBenchmarkAndAppendResult(benchmark, statusBarText, L"blur");

	m_viewBuffer.SaveViewBufferStateForNextMotionBlur(); // Low cost operation

	m_viewBuffer.AntiAliase();

	ClockBenchmarkAndAppendResult(benchmark, statusBarText, L"antiAliasing");

	if (m_bRenderToFile)
	{
		WriteViewBufferToFile(m_sOutputPath);
	}
	else if (!(IsRenderingVideoToFileAndNotDefaultResolution()))
	{
		StretchDIBits(m_pDC->m_hDC, 0, 0, m_viewBuffer.Width(), m_viewBuffer.Height(), 0, 0, m_viewBuffer.Width(), m_viewBuffer.Height(), m_viewBuffer.Memory(), &m_viewBuffer.Info(), DIB_RGB_COLORS, SRCCOPY);
	}

	m_viewBufferLock.unlock();

	STATUS_BAR_TEXT(statusBarText);

	m_bRender = m_bRenderToFile = false;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView CGWork Finishing and clearing...

void CCGWorkView::OnDestroy() 
{
	CView::OnDestroy();

	// delete the DC
	if ( m_pDC ) {
		delete m_pDC;
	}

	if (m_pDbDC) {
		delete m_pDbDC;
	}
}



/////////////////////////////////////////////////////////////////////////////
// User Defined Functions

void CCGWorkView::RenderScene() {
	// do nothing. This is supposed to be overriden...

	return;
}

void CCGWorkView::LoadGameObjectFromFile(CString& path)
{
		GameObject* importedGameObject;
	if (CGSkelProcessIritDataFiles(path, 1, m_nTesselation, &importedGameObject))
		{
			importedGameObject->CalculateVertexCalculatedNormals();

			if (m_bUseCalculatedNormalsOnImport)
			{
				importedGameObject->UseCalculatedNormals();
			}
			else
			{
				importedGameObject->UseImportedNormals();
			}

			if (m_bInvertNormalsOnImport)
			{
				importedGameObject->InvertCalculatedNormals();
			}
		}
}

void CCGWorkView::OnFileLoad() 
{
	TCHAR szFilters[] = _T ("IRIT Data Files (*.itd)|*.itd|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("itd"), _T("*.itd"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,szFilters);

	if (dlg.DoModal () == IDOK)
	{
		LoadGameObjectFromFile(dlg.GetPathName());
		Invalidate();
	}
}


// VIEW HANDLERS ///////////////////////////////////////////

// Note: that all the following Message Handlers act in a similar way.
// Each control or command has two functions associated with it.

void CCGWorkView::OnViewOrthographic() 
{
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_Camera.SetMode(m_nView);
	Invalidate();
}

void CCGWorkView::OnUpdateViewOrthographic(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_ORTHOGRAPHIC);
}

void CCGWorkView::OnViewPerspective() 
{
	m_nView = ID_VIEW_PERSPECTIVE;
	m_Camera.SetMode(m_nView);
	Invalidate();
}

void CCGWorkView::OnUpdateViewPerspective(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_PERSPECTIVE);
}


void CCGWorkView::OnViewShowpolygonnormals()
{
	m_bShowPolygonNormals = !m_bShowPolygonNormals;
	Invalidate();
}


void CCGWorkView::OnUpdateViewShowpolygonnormals(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowPolygonNormals);
}

void CCGWorkView::OnViewShownormals()
{
	m_bShowVertexNormals = !m_bShowVertexNormals;
	Invalidate();
}


void CCGWorkView::OnUpdateViewShownormals(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowVertexNormals);
}


void CCGWorkView::OnShowBoundingbox()
{
	m_bShowBoundingBox = !m_bShowBoundingBox;
	Invalidate();
}


void CCGWorkView::OnUpdateShowBoundingbox(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowBoundingBox);
}

void CCGWorkView::OnShowSilhouette()
{
	m_bShowSilhouette = !m_bShowSilhouette;
	Invalidate();
}


void CCGWorkView::OnUpdateShowSilhouette(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowSilhouette);
}

void CCGWorkView::OnViewEnablebackfaceculling()
{
	m_bBackFaceCulling = !m_bBackFaceCulling;
	Invalidate();
}


void CCGWorkView::OnUpdateViewEnablebackfaceculling(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bBackFaceCulling);
}



// ACTION HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnActionRotate() 
{
	m_nAction = ID_ACTION_ROTATE;
}

void CCGWorkView::OnUpdateActionRotate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_ROTATE);
}

void CCGWorkView::OnActionTranslate() 
{
	m_nAction = ID_ACTION_TRANSLATE;
}

void CCGWorkView::OnUpdateActionTranslate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_TRANSLATE);
}

void CCGWorkView::OnActionScale() 
{
	m_nAction = ID_ACTION_SCALE;
}

void CCGWorkView::OnUpdateActionScale(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_SCALE);
}




// AXIS HANDLERS ///////////////////////////////////////////


// Gets calles when the X button is pressed or when the Axis->X menu is selected.
// The only thing we do here is set the ChildView member variable m_nAxis to the 
// selected axis.
void CCGWorkView::OnAxisX() 
{
	m_nAxis = ID_AXIS_X;
}

// Gets called when windows has to repaint either the X button or the Axis pop up menu.
// The control is responsible for its redrawing.
// It sets itself disabled when the action is a Scale action.
// It sets itself Checked if the current axis is the X axis.
void CCGWorkView::OnUpdateAxisX(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_X);
}


void CCGWorkView::OnAxisY() 
{
	m_nAxis = ID_AXIS_Y;
}

void CCGWorkView::OnUpdateAxisY(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Y);
}


void CCGWorkView::OnAxisZ() 
{
	m_nAxis = ID_AXIS_Z;
}

void CCGWorkView::OnUpdateAxisZ(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Z);
}



void CCGWorkView::OnAxisXY()
{
	m_nAxis = ID_AXIS_XY;
}

void CCGWorkView::OnUpdateAxisXY(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_XY);
}

void CCGWorkView::OnAxisBoundxyz()
{
	m_nAxis = ID_AXIS_BOUNDXYZ;
}


void CCGWorkView::OnUpdateAxisBoundxyz(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_BOUNDXYZ);
}

void CCGWorkView::OnWorldSpace()
{
	m_transformationType = Space::WorldSpace;
}

void CCGWorkView::OnUpdateWorldSpace(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_transformationType == Space::WorldSpace);
}

void CCGWorkView::OnObjectSpace()
{
	m_transformationType = Space::ObjectSpace;
}

void CCGWorkView::OnUpdateObjectSpace(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_transformationType == Space::ObjectSpace);
}

void CCGWorkView::OnInvertCalculatedNormals()
{
	for (GameObject& gameObject : g_gameObjects)
	{
		if (gameObject.Selected || !IsAnyGameObjectSelected())
		{
			gameObject.InvertCalculatedNormals();
		}
	}

	Invalidate();
}

void CCGWorkView::OnUseCalculatedNormals()
{
	for (GameObject& gameObject : g_gameObjects)
	{
		if (gameObject.Selected || !IsAnyGameObjectSelected())
		{
			gameObject.UseCalculatedNormals();
		}
	}

	Invalidate();
}

void CCGWorkView::OnUseImportedNormals()
{
	for (GameObject& gameObject : g_gameObjects)
	{
		if (gameObject.Selected || !IsAnyGameObjectSelected())
		{
			gameObject.UseImportedNormals();
		}
	}

	Invalidate();
}

void CCGWorkView::OnRenderToScreen()
{
	m_bRender = true;
	Invalidate();
}

void CCGWorkView::OnRenderToFile()
{
	//Change to open dialog / render to file

	TCHAR szFilters[] = _T("All Files (*.*)|*.*||");

	CFileDialog fileDlg(FALSE, _T("PNG"), _T("*.png"),
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters);
	if (fileDlg.DoModal() == IDOK)
	{
		m_sOutputPath = fileDlg.GetPathName();
		m_sFileName = fileDlg.GetFileTitle();

		if (!m_bRenderVideo)
		{
			m_bRenderToFile = true;
			Invalidate();
		}
	}
}

void CCGWorkView::OnAlwaysRender()
{
	m_bAlwaysRender = !m_bAlwaysRender;
	Invalidate();
}

void CCGWorkView::OnUpdateAlwaysRender(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bAlwaysRender);
}

void CCGWorkView::OnUpdateRecordToggle(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_animator.IsRecording());
}

void CCGWorkView::OnRecordToggle()
{
	// Ignore in case it's a missclick
	if (m_animator.IsReplaying())
	{
		return;
	}

	if (!m_animator.IsRecording())
	{
		m_gameObjectsStateAtStartOfRecording.clear();
		for (GameObject& gameObject : g_gameObjects)
		{
			m_gameObjectsStateAtStartOfRecording.push_back(GameObject());
			GameObject& savedGameObject = m_gameObjectsStateAtStartOfRecording.back();
			savedGameObject.GetWorldTransformation(gameObject);
		}

		m_animator.StartRecording();
	}
	else
	{
		m_animator.StopRecording();
	}

	Invalidate();
}

void CCGWorkView::OnUpdateReplayToggle(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_animator.IsReplaying());
}

void CCGWorkView::SetGameObjectsStateLikeRecordStart()
{
	for (int i = 0; i < g_gameObjects.size(); i++)
	{
		g_gameObjects[i].GetWorldTransformation(m_gameObjectsStateAtStartOfRecording[i]);
	}
}

void CCGWorkView::OnReplayToggle()
{
	// Ignore in case it's a missclick
	if (m_animator.IsRecording())
	{
		return;
	}

	if (!m_animator.HasRecorded())
		return;

	if (!m_animator.IsReplaying())
	{
		m_nFrameCount = 0;
		SetGameObjectsStateLikeRecordStart();
		m_animator.StartReplaying();
	}
	else
	{
		m_animator.StopReplaying();
	}

	Invalidate();
}

void CCGWorkView::OnUpdateRepeatReplayToggle(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_animator.Repeat);
}

void CCGWorkView::OnRepeatReplayToggle()
{
	m_animator.Repeat = !m_animator.Repeat;
	Invalidate();
}

void CCGWorkView::OnUpdateRender(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(false);
}

void CCGWorkView::OnUpdateReplaySpeedHalf(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_animator.ReplaySpeed == ReplaySpeed::Half);
}

void CCGWorkView::OnReplaySpeedHalf()
{
	m_animator.ReplaySpeed = ReplaySpeed::Half;
}

void CCGWorkView::OnUpdateReplaySpeed1(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_animator.ReplaySpeed == ReplaySpeed::Normal);
}

void CCGWorkView::OnReplaySpeed1()
{
	m_animator.ReplaySpeed = ReplaySpeed::Normal;
}

void CCGWorkView::OnUpdateReplaySpeed2(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_animator.ReplaySpeed == ReplaySpeed::Double);
}

void CCGWorkView::OnReplaySpeed2()
{
	m_animator.ReplaySpeed = ReplaySpeed::Double;
}

void CCGWorkView::OnUpdateReplaySpeed4(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_animator.ReplaySpeed == ReplaySpeed::Quadruple);
}

void CCGWorkView::OnReplaySpeed4()
{
	m_animator.ReplaySpeed = ReplaySpeed::Quadruple;
}

void CCGWorkView::OnUpdateRenderVideoToggle(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bRenderVideo);
}

void CCGWorkView::OnRenderVideoToggle()
{
	m_bRenderVideo = !m_bRenderVideo;
}

// OPTIONS HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnPersOptions()
{
	CPersOptionsDlg presOptionsDlg(m_Camera.GetClose(), m_Camera.GetFar(), m_Camera.GetD(), m_Camera.GetOrthographicSize());

	if (presOptionsDlg.DoModal() == IDOK)
	{
		m_Camera.SetClose(presOptionsDlg.Close);
		m_Camera.SetFar(presOptionsDlg.Far);
		m_Camera.SetD(presOptionsDlg.D);
		m_Camera.SetOrthographicSize(presOptionsDlg.OrthographicSize);
		Invalidate();
	}
}

void CCGWorkView::OnGeneralOptions()
{
	CGeneralDlg generalOptionsDlg(
		m_nTesselation,
		m_bBGImage,
		m_bBGMode,
		m_bDefaultSize,
		m_sFilePath,
		m_nRenderHeight,
		m_nRenderWidth,
		m_nBackgroundColor,
		m_bUseCalculatedNormalsOnImport,
		m_bInvertNormalsOnImport);

	if (generalOptionsDlg.DoModal() == IDOK)
	{
		//Update Params
		m_nTesselation = generalOptionsDlg.m_tesselation;
		generalOptionsDlg.GetParams(
			m_bBGImage, 
			m_bBGMode, 
			m_bDefaultSize,
			m_sFilePath,
			m_nRenderHeight,
			m_nRenderWidth,
			m_nBackgroundColor,
			m_bUseCalculatedNormalsOnImport,
			m_bInvertNormalsOnImport);
		PngWrapper p(CT2A(m_sFilePath).m_psz);
		if (generalOptionsDlg.m_bBGImage == TRUE && p.ReadPng())
		{
			ImageMode mode = ImageMode::Stretch;
			if (m_bBGMode)
			{
				mode = ImageMode::Repeat;
			}

			m_viewBuffer.SetBackgroundImage(p, mode);
		}
		else
		{
			m_viewBuffer.SetBackgroundSolid(m_nBackgroundColor);
		}

		Invalidate();
	}
}

void CCGWorkView::OnColorsOptions()
{
	CColorsDlg colorsOptionsDlg(m_nWireframeColor, m_nNormalColor, m_nBoundryColor, m_nSilhouetteColor);

	if (colorsOptionsDlg.DoModal() == IDOK)
	{
		for (GameObject& gameObject : g_gameObjects)
		{
			if (!gameObject.Selected)
			{
				continue;
			}

			for (Polygon3D& polygon : gameObject.Polygons)
			{
				polygon.Color = colorsOptionsDlg.m_wireframeColor;
			}
		}

		//Update Params
		m_nWireframeColor = colorsOptionsDlg.m_wireframeColor;

		m_nNormalColor = colorsOptionsDlg.m_normalColor;
		m_nSilhouetteColor = colorsOptionsDlg.m_silhouetteColor;
		m_nBoundryColor = colorsOptionsDlg.m_boundryColor;
	}

	Invalidate();
}

void CCGWorkView::OnMouseSensitivityOptions()
{
	CDragSensitivityDlg dragSensitivityOptionsDlg(m_nScaleSensitivity, m_nRotateSensitivity, m_nTranslateSensitivity);

	if (dragSensitivityOptionsDlg.DoModal() == IDOK) {
		//Update Params
		m_nScaleSensitivity = dragSensitivityOptionsDlg.m_scaleSensitivity;
		m_nRotateSensitivity = dragSensitivityOptionsDlg.m_rotateSensitivity;
		m_nTranslateSensitivity = dragSensitivityOptionsDlg.m_translateSensitivity;
	}
}


void CCGWorkView::OnOptionsColours()
{
	// TODO: Add your command handler code here
}



// LIGHT SHADING HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnLightShadingFlat() 
{
	m_nShadingMode = ShadingMode::FLAT;
	Invalidate();
}

void CCGWorkView::OnUpdateLightShadingFlat(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nShadingMode == ShadingMode::FLAT);
}


void CCGWorkView::OnLightShadingGouraud() 
{
	m_nShadingMode = ShadingMode::GOURAUD;
	Invalidate();
}

void CCGWorkView::OnUpdateLightShadingGouraud(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nShadingMode == ShadingMode::GOURAUD);
}

void CCGWorkView::OnLightShadingPhong()
{
	m_nShadingMode = ShadingMode::PHONG;
	Invalidate();
}


void CCGWorkView::OnUpdateLightShadingPhong(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nShadingMode == ShadingMode::PHONG);
}


// LIGHT SETUP HANDLER ///////////////////////////////////////////

void CCGWorkView::SetLightsData()
{
	for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
	{
		Light::SetLight(id, Light(m_lights[id]));
	}
}

void CCGWorkView::OnLightConstants() 
{
	CLightDialog dlg;

	for (int id=LIGHT_ID_1;id<MAX_LIGHT;id++)
	{	    
	    dlg.SetDialogData((LightID)id,m_lights[id]);
	}
	dlg.SetDialogData(LIGHT_ID_AMBIENT,m_ambientLight);
	dlg.SetDialogGlobals(m_dAmbientIntensity, m_dDiffuseIntensity, m_dSpecularIntensity, m_nSpecularExponent);

	if (dlg.DoModal() == IDOK) 
	{
	    for (int id=LIGHT_ID_1;id<MAX_LIGHT;id++)
	    {
			m_lights[id] = dlg.GetDialogData((LightID)id);
	    }

		SetLightsData();

	    m_ambientLight = dlg.GetDialogData(LIGHT_ID_AMBIENT);
		dlg.GetDialogGlobals(m_dAmbientIntensity, m_dDiffuseIntensity, m_dSpecularIntensity, m_nSpecularExponent);
		Light::SetRatios(m_dAmbientIntensity, RGB(m_ambientLight.colorR, m_ambientLight.colorG, m_ambientLight.colorB), m_dDiffuseIntensity, m_dSpecularIntensity, m_nSpecularExponent);
	}	
	Invalidate();
}

void CCGWorkView::WriteViewBufferToFile(CString& path)
{
	PngWrapper pngWrapper(CT2A(path).m_psz, m_viewBuffer.Width(), m_viewBuffer.Height());
	pngWrapper.InitWritePng(); // Allocate buffer
	for (int y = 0; y < m_viewBuffer.Height(); y++)
	{
		for (int x = 0; x < m_viewBuffer.Width(); x++)
		{
			pngWrapper.SetValue(x, y, m_viewBuffer.GetPixelForPng(x, y));
		}
	}

	pngWrapper.WritePng();
}

int counter = 0;
void CCGWorkView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent != 1)
		return;

	if (!m_animator.IsReplaying())
		return;

	if (m_animator.Repeat && m_animator.ReplayFinished() && m_animator.HasRecorded())
	{
		SetGameObjectsStateLikeRecordStart();
		m_animator.StartReplaying();
		Invalidate();
		return;
	}

	if (m_bRenderVideo && m_sOutputPath.GetLength() > 4)
	{
		m_viewBufferLock.lock();
		CString path;
		path.Format(L"%s_%d.png", m_sOutputPath.Mid(0, m_sOutputPath.GetLength() - 4), m_nFrameCount++); // Remove .png, add frame count, re-add .png
		WriteViewBufferToFile(path);
		m_viewBufferLock.unlock();
	}

	std::vector<Action> actionsToReplay;
	if (m_animator.PopActionsToReplay(actionsToReplay))
	{
		for (Action& action : actionsToReplay)
		{
			DoAction(action);
		}

		Invalidate();
	}

	m_animator.AddFrameTime();
}






void CCGWorkView::OnOptionsSpecialeffect()
{
	CSpecialDlg generalOptionsDlg(
		m_dFogFar,
		m_dMotionBlur,
		m_fogColor,
		m_fogMode,
		m_viewBuffer.AAType(),
		m_viewBuffer.AASize());

	if (generalOptionsDlg.DoModal() == IDOK)
	{
		//Update Params
		int AASize, AAType;
		generalOptionsDlg.GetParams(
			m_dFogFar,
			m_dMotionBlur,
			m_fogColor,
			m_fogMode,
			AAType,
			AASize);
		m_viewBuffer.SetFogParams(m_dFogFar, m_fogColor, m_fogMode);
		m_viewBuffer.SetAAParams(AASize, AAType);
		Invalidate();
	}
}
