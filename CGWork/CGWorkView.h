// CGWorkView.h : interface of the CCGWorkView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)
#define AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "gl\gl.h"    // Include the standard CGWork  headers
#include "gl\glu.h"   // Add the utility library

#include <mutex>

#include "Physics.h"
#include "Light.h"
#include "BresenhamDraw.h"
#include "GameObject.h"
#include "Globals.h"
#include "ViewBuffer.h"
#include "ScanConversion.h"
#include "Benchmark.h"
#include "CGWorkDoc.h"
#include "Camera.h"
#include "Animation.h"

extern std::vector<GameObject> g_gameObjects;
extern GameObject currentGameObject;

class CCGWorkView : public CView
{
protected: // create from serialization only
	CCGWorkView();
	void SetMousePosition(Vector3D& positionInPixels);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseDragL(Vector3D& offset);
	void OnMouseDragM(Vector3D& offset);
	void OnMouseDragR(Vector3D& offset);
	afx_msg void OnKeyUp(UINT nChar,UINT nRepCnt, UINT nFlags);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnMButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnPersOptions();
	void OnGeneralOptions();
	void OnColorsOptions();
	void OnWorldSpace();
	void OnUpdateWorldSpace(CCmdUI* pCmdUI);
	void OnObjectSpace();
	void OnUpdateObjectSpace(CCmdUI* pCmdUI);
	void OnInvertCalculatedNormals();
	void OnUseCalculatedNormals();
	void OnUseImportedNormals();
	DECLARE_DYNCREATE(CCGWorkView)

		// Attributes
public:
	CCGWorkDoc* GetDocument();
	void DrawBoundingBox(BresenhamDraw& pen, GameObject& gameObject);
	void DrawPolygonNormals(BresenhamDraw& pen, GameObject& gameObject);
	void DrawVertexNormals(BresenhamDraw& pen, GameObject& gameObject);
	void DrawWireframe(BresenhamDraw& pen, GameObject& gameObject, COLORREF color, bool force = false);
	void DrawPolygons(ScanConversion::ScanConversion& scan, GameObject& gameObject);
	void SetLightsData();
	// Operations
public:

private:
	int m_nAxis;						// Axis of Action, X Y or Z
	int m_nAction;						// Rotate, Translate, Scale
	int m_nView = ID_VIEW_PERSPECTIVE;	// Orthographic, perspective
	bool m_bShowVertexNormals = false;
	bool m_bShowPolygonNormals = false;
	bool m_bShowBoundingBox = false;
	bool m_bShowSilhouette = false;


	bool m_bBGImage = false;
	bool m_bBGMode = false;

	bool m_bUseCalculatedNormalsOnImport = true;
	bool m_bInvertNormalsOnImport = false;

	// Render settings
	CString m_sFilePath = "";
	int m_nRenderHeight = 1080;
	int m_nRenderWidth = 1920;
	bool m_bDefaultSize = true;

	int m_nLightShading;				// shading: Flat, Gouraud.

	double m_lMaterialAmbient;			// The Ambient in the scene
	double m_lMaterialDiffuse;			// The Diffuse in the scene
	double m_lMaterialSpecular;			// The Specular in the scene
	int m_nMaterialCosineFactor;		// The cosine factor for the specular
	int m_nLastWindowSize = -1;
	double m_nScaleSensitivity = MIN_SCALE_SENSITIVITY;
	double m_nRotateSensitivity = MIN_ROTATE_SENSITIVITY;
	double m_nTranslateSensitivity = MIN_TRANSLATE_SENSITIVITY;
	COLORREF m_nWireframeColor = RGB(255, 255, 255);//White
	COLORREF m_nNormalColor = RGB(0, 255, 0);		// Green
	COLORREF m_nBoundryColor = RGB(255, 0, 0);	// Red
	COLORREF m_nSilhouetteColor = RGB(0, 255, 255); // Yellow
	COLORREF m_nBackgroundColor = RGB(0, 0, 0);		// Black
	Space m_transformationType = Space::WorldSpace;

	ShadingMode m_nShadingMode = ShadingMode::FLAT;
	Camera m_Camera;
	Benchmark m_frameTime;

	int m_nFrameCount = 0;
	bool m_bRenderVideo = false;
	std::vector<GameObject> m_gameObjectsStateAtStartOfRecording;
	Animator m_animator;

	int m_nTesselation = DEFAULT_TESSELATION;
	std::mutex m_viewBufferLock;

	double m_dAmbientIntensity = 0.2;
	double m_dDiffuseIntensity = 0.8;
	double m_dSpecularIntensity = 1.0;
	int m_nSpecularExponent = 32;

	bool m_bRender = false;
	bool m_bRenderToFile = false;
	bool m_bAlwaysRender = false;

	// Mouse
	bool m_clicking = false;
	CPoint m_downPositionL;

	LightParams m_lights[MAX_LIGHT];	//configurable lights array
	LightParams m_ambientLight;		//ambient light (only RGB is used)

	ViewBuffer m_viewBuffer;

	bool m_bBackFaceCulling = false;

	CString m_sOutputPath;
	CString m_sFileName;

	double m_dFogFar = DEFAULT_FOG_FAR;

	double m_dMotionBlur = DEFAULT_MOTION_BLUR;
	COLORREF m_fogColor = RGB(100,100,100);
	FogMode m_fogMode = FogMode::Transparent;

	bool IsAnyGameObjectSelected()
	{
		for (GameObject& gameObject : g_gameObjects)
		{
			if (gameObject.Selected)
			{
				return true;
			}
		}

		return false;
	}

	bool ActionsApplyToGameObject(GameObject& gameObject)
	{
		return !IsAnyGameObjectSelected() || gameObject.Selected;
	}

	void DoAction(Action& action);

public:

	// Polygon normal goes inside the GameObject
	// Dot product between [polygon normal in world space] and [camera to polygon position offset]
	bool IsPolygonBackFacing(GameObject& gameObject, Polygon3D& polygon, Camera& camera);

	void RenderGameObjects(CRect& rect);

	bool IsRenderingVideoToFileAndNotDefaultResolution();

	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

	// Implementation
public:
	virtual ~CCGWorkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL InitializeCGWork();
	BOOL SetupViewingFrustum(void);
	BOOL SetupViewingOrthoConstAspect(void);

	virtual void RenderScene();

	void LoadGameObjectFromFile(CString& path);


	HGLRC    m_hRC;			// holds the Rendering Context
	CDC* m_pDC;			// holds the Device Context
	int m_WindowWidth;		// hold the windows width
	int m_WindowHeight;		// hold the windows height
	double m_AspectRatio;		// hold the fixed Aspect Ration

	HBITMAP m_pDbBitMap;
	CDC* m_pDbDC;

	// Generated message map functions
protected:
	//{{AFX_MSG(CCGWorkView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileLoad();
	afx_msg void OnViewOrthographic();
	afx_msg void OnUpdateViewOrthographic(CCmdUI* pCmdUI);
	afx_msg void OnViewPerspective();
	afx_msg	void OnUpdateShowSilhouette(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPerspective(CCmdUI* pCmdUI);
	afx_msg void OnActionRotate();
	afx_msg void OnUpdateActionRotate(CCmdUI* pCmdUI);
	afx_msg void OnActionScale();
	afx_msg void OnUpdateActionScale(CCmdUI* pCmdUI);
	afx_msg void OnActionTranslate();
	afx_msg void OnUpdateActionTranslate(CCmdUI* pCmdUI);
	afx_msg void OnAxisX();
	afx_msg void OnUpdateAxisX(CCmdUI* pCmdUI);
	afx_msg void OnAxisY();
	afx_msg void OnUpdateAxisY(CCmdUI* pCmdUI);
	afx_msg void OnAxisZ();
	afx_msg void OnUpdateAxisZ(CCmdUI* pCmdUI);
	afx_msg void OnAxisXY();
	afx_msg void OnUpdateAxisXY(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingFlat();
	afx_msg void OnUpdateLightShadingFlat(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingGouraud();
	afx_msg void OnUpdateLightShadingGouraud(CCmdUI* pCmdUI);
	afx_msg void OnLightConstants();
	void WriteViewBufferToFile(CString& path);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnAxisBoundxyz();
	afx_msg void OnUpdateAxisBoundxyz(CCmdUI* pCmdUI);
	afx_msg void OnMouseSensitivityOptions();
	afx_msg void OnOptionsColours();
	afx_msg void OnViewShowpolygonnormals();
	afx_msg void OnUpdateViewShowpolygonnormals(CCmdUI* pCmdUI);
	afx_msg void OnViewShownormals();
	afx_msg void OnUpdateViewShownormals(CCmdUI* pCmdUI);
	afx_msg void OnShowBoundingbox();
	afx_msg void OnUpdateShowBoundingbox(CCmdUI* pCmdUI);
	afx_msg void OnShowSilhouette();
	afx_msg void OnRenderToScreen();
	afx_msg void OnRenderToFile();
	void OnAlwaysRender();
	void OnUpdateAlwaysRender(CCmdUI* pCmdUI);
	void OnUpdateRecordToggle(CCmdUI* pCmdUI);
	void OnRecordToggle();
	void OnUpdateReplayToggle(CCmdUI* pCmdUI);
	void SetGameObjectsStateLikeRecordStart();
	void OnReplayToggle();
	void OnUpdateRepeatReplayToggle(CCmdUI* pCmdUI);
	void OnUpdateRender(CCmdUI* pCmdUI);

	void OnUpdateReplaySpeedHalf(CCmdUI* pCmdUI);

	void OnReplaySpeedHalf();

	void OnUpdateReplaySpeed1(CCmdUI* pCmdUI);

	void OnReplaySpeed1();

	void OnUpdateReplaySpeed2(CCmdUI* pCmdUI);

	void OnReplaySpeed2();

	void OnUpdateReplaySpeed4(CCmdUI* pCmdUI);

	void OnReplaySpeed4();

	void OnUpdateRenderVideoToggle(CCmdUI* pCmdUI);

	void OnRenderVideoToggle();

	afx_msg void OnRepeatReplayToggle();

	afx_msg void OnLightShadingPhong();
	afx_msg void OnUpdateLightShadingPhong(CCmdUI* pCmdUI);
	afx_msg void OnViewEnablebackfaceculling();
	afx_msg void OnUpdateViewEnablebackfaceculling(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSpecialeffect();

};

#ifndef _DEBUG  // debug version in CGWorkView.cpp
inline CCGWorkDoc* CCGWorkView::GetDocument()
{
	return (CCGWorkDoc*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)
