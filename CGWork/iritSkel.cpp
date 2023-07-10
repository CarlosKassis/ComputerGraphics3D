#include <vector>
#include "stdafx.h"
#include "iritSkel.h"
#include "Physics.h"
#include <tuple>

/*****************************************************************************
* Skeleton for an interface to a parser to read IRIT data files.			 *
******************************************************************************
* (C) Gershon Elber, Technion, Israel Institute of Technology                *
******************************************************************************
* Written by:  Gershon Elber				Ver 1.0, Feb 2002				 *
* Minimal changes made by Amit Mano			November 2008					 *
******************************************************************************/

IPFreeformConvStateStruct CGSkelFFCState = {
	FALSE,          /* Talkative */
	FALSE,          /* DumpObjsAsPolylines */
	TRUE,           /* DrawFFGeom */
	FALSE,          /* DrawFFMesh */
	{ 10, 10, 10 }, /* 10 isocurves peru/v/w direction. */
	100,            /* 100 point samples along a curve. */
	SYMB_CRV_APPROX_UNIFORM,  /* CrvApproxMethod */
	FALSE,   /* ShowIntrnal */
	FALSE,   /* CubicCrvsAprox */
	20,      /* Polygonal FineNess */
	FALSE,   /* ComputeUV */
	TRUE,    /* ComputeNrml */
	FALSE,   /* FourPerFlat */
	0,       /* OptimalPolygons */
	FALSE,   /* BBoxGrid */
	TRUE,    /* LinearOnePolyFlag */
	FALSE
};

//CGSkelProcessIritDataFiles(argv + 1, argc - 1);

std::vector<GameObject> g_gameObjects;
GameObject currentGameObject;

/*****************************************************************************
* DESCRIPTION:                                                               *
* Main module of skeleton - Read command line and do what is needed...	     *
*                                                                            *
* PARAMETERS:                                                                *
*   FileNames:  Files to open and read, as a vector of strings.              *
*   NumFiles:   Length of the FileNames vector.								 *
*                                                                            *
* RETURN VALUE:                                                              *
*   bool:		false - fail, true - success.                                *
*****************************************************************************/
bool CGSkelProcessIritDataFiles(CString &FileNames, int NumFiles, int fineNess, GameObject** gameObject)
{
	IPObjectStruct *PObjects;
	IrtHmgnMatType CrntViewMat;
	IPTraverseObjHierarchyStruct TraversState;
	/* Get the data files: */
	IPSetFlattenObjects(FALSE);
	CStringA CStr(FileNames);
	if ((PObjects = IPGetDataFiles((const char* const *)&CStr, 1/*NumFiles*/, TRUE, FALSE)) == NULL)
		return false;
	PObjects = IPResolveInstances(PObjects);

	if (IPWasPrspMat)
		MatMultTwo4by4(CrntViewMat, IPViewMat, IPPrspMat);
	else
		IRIT_GEN_COPY(CrntViewMat, IPViewMat, sizeof(IrtHmgnMatType));

	/* Here some useful parameters to play with in tesselating freeforms: */
	CGSkelFFCState.FineNess = fineNess;   /* Res. of tesselation, larger is finer. */
	CGSkelFFCState.ComputeUV = TRUE;   /* Wants UV coordinates for textures. */
	CGSkelFFCState.FourPerFlat = TRUE;/* 4 poly per ~flat patch, 2 otherwise.*/
	CGSkelFFCState.LinearOnePolyFlag = TRUE;    /* Linear srf gen. one poly. */

	currentGameObject = GameObject();

	/* Traverse ALL the parsed data, recursively. */
	IPTraverseObjHierarchyInitState(&TraversState);
	TraversState.ApplyFunc = (IPApplyObjFuncType) CGSkelDumpOneTraversedObject;
	IRIT_GEN_COPY(TraversState.Mat, CrntViewMat, sizeof(IrtHmgnMatType));
	IPTraverseObjListHierarchy(PObjects, &TraversState);

	g_gameObjects.push_back(currentGameObject);
	*gameObject = &g_gameObjects[g_gameObjects.size() - 1];
	return true;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Call back function of IPTraverseObjListHierarchy. Called on every non    *
* list object found in hierarchy.                                            *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:       Non list object to handle.                                   *
*   Mat:        Transformation matrix to apply to this object.               *
*   Data:       Additional data.                                             *
*                                                                            *
* RETURN VALUE:                                                              *
*   void									                                 *
*****************************************************************************/
void CGSkelDumpOneTraversedObject(IPObjectStruct *PObj,
                                  IrtHmgnMatType Mat,
                                  void *Data)
{
	IPObjectStruct *PObjs;

	if (IP_IS_FFGEOM_OBJ(PObj))
		PObjs = IPConvertFreeForm(PObj, &CGSkelFFCState);
	else
		PObjs = PObj;

	
	for (PObj = PObjs; PObj != NULL; PObj = PObj->Pnext)
	{
		if (PObj->ObjType != IP_OBJ_POLY) {
			continue;
		}

		if (!CGSkelStoreData(PObj, currentGameObject))
			exit(1);
	}
}

void TrySetMinMaxVertex(GameObject& gameObject, Vector3D& point)
{
	if (gameObject.MinMaxVertexCoordSet)
	{
		for (int i = 0; i < 3; i++)
		{
			gameObject.MinVertexCoord[i] = point[i] < gameObject.MinVertexCoord[i] ? point[i] : gameObject.MinVertexCoord[i];
			gameObject.MaxVertexCoord[i] = point[i] > gameObject.MaxVertexCoord[i] ? point[i] : gameObject.MaxVertexCoord[i];
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			gameObject.MinVertexCoord[i] = point[i];
			gameObject.MaxVertexCoord[i] = point[i];
		}

		gameObject.MinMaxVertexCoordSet = true;
	}
}

bool GetPolygonColor(IPPolygonStruct* polygon, COLORREF& color)
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
			color = RGB(colorArray[0], colorArray[1], colorArray[2]);
			return true;
		}
		attr = attr->Pnext;
	}

	return false;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Prints the data from given geometry object.								 *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:       Object to print.                                             *
*   Indent:     Column of indentation.                                       *
*                                                                            *
* RETURN VALUE:                                                              *
*   bool:		false - fail, true - success.                                *
*****************************************************************************/
bool CGSkelStoreData(IPObjectStruct *PObj, GameObject& gameObject)
{
	const char *Str;
	double RGB[3], Transp;
	bool missingVertexNormal = false;
	IPPolygonStruct *PPolygon;
	IPVertexStruct *PVertex;
	const IPAttributeStruct *Attrs =
        AttrTraceAttributes(PObj -> Attr, PObj -> Attr);

	if (PObj -> ObjType != IP_OBJ_POLY) {
		AfxMessageBox(_T("Non polygonal object detected and ignored"));
		return true;
	}

	/* You can use IP_IS_POLYGON_OBJ(PObj) and IP_IS_POINTLIST_OBJ(PObj) 
	   to identify the type of the object*/

	double opacity = 1.0;
	if (CGSkelGetObjectColor(PObj, RGB))
	{
		int z = 5;
		/* color code */
	}
	if (CGSkelGetObjectTransp(PObj, &Transp))
	{
		opacity = 1.0 - Transp;
	}
	if ((Str = CGSkelGetObjectTexture(PObj)) != NULL)
	{
		/* volumetric texture code */
	}
	if ((Str = CGSkelGetObjectPTexture(PObj)) != NULL)
	{
		/* parametric texture code */
	}
	if (Attrs != NULL) 
	{
		printf("[OBJECT\n");
		while (Attrs) {
			/* attributes code */
			Attrs = AttrTraceAttributes(Attrs, NULL);
		}
	}

	std::vector<std::tuple<Vector3D, std::vector<Vector3D>>> pointsNormals;

	for (PPolygon = PObj -> U.Pl; PPolygon != NULL;	PPolygon = PPolygon -> Pnext) 
	{
		if (PPolygon -> PVertex == NULL) {
			AfxMessageBox(_T("Dump: Attempt to dump empty polygon"));
			return false;
		}

		Polygon3D polygon;
		Vector3D polygonCenter;
		for (IPVertexStruct* ipVertex = PPolygon->PVertex; ipVertex != nullptr; ipVertex = ipVertex->Pnext)
		{
			Vector3D vertex(ipVertex->Coord);

			TrySetMinMaxVertex(gameObject, vertex);

			vertex.SetNormal(Vector3D(ipVertex->Normal), NormalType::Imported);

			polygonCenter += vertex;

			COLORREF polygonImportColor;
			polygon.Color = Vector3D(RGB(255, 255, 255));
			if (GetPolygonColor(PPolygon, polygonImportColor))
			{
				polygon.Color = Vector3D(polygonImportColor);
			}

			polygon.Vertices.push_back(vertex);
		}

		polygon.Center = polygonCenter / (polygon.Vertices.empty() ? 1 : polygon.Vertices.size());
		polygon.SetCalculatedNormal(Vector3D::PlaneNormal(polygon.Vertices));
		if (IP_HAS_PLANE_POLY(PPolygon))
		{
			polygon.SetImportedNormal(Vector3D(PPolygon->Plane));
		}

		polygon.Opacity = opacity;

		gameObject.Polygons.push_back(polygon);
	}

	printf("Object read\n");
	/* Close the object. */
	return true;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the color of an object.                                          *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its color.                                         *
*   RGB:    as 3 floats in the domain [0, 1].                                *
*                                                                            *
* RETURN VALUE:                                                              *
*   int:    TRUE if object has color, FALSE otherwise.                       *
*****************************************************************************/
int CGSkelGetObjectColor(IPObjectStruct *PObj, double RGB[3])
{
	static int TransColorTable[][4] = {
		{ /* BLACK	*/   0,    0,   0,   0 },
		{ /* BLUE	*/   1,    0,   0, 255 },
		{ /* GREEN	*/   2,    0, 255,   0 },
		{ /* CYAN	*/   3,    0, 255, 255 },
		{ /* RED	*/   4,  255,   0,   0 },
		{ /* MAGENTA 	*/   5,  255,   0, 255 },
		{ /* BROWN	*/   6,   50,   0,   0 },
		{ /* LIGHTGRAY	*/   7,  127, 127, 127 },
		{ /* DARKGRAY	*/   8,   63,  63,  63 },
		{ /* LIGHTBLUE	*/   9,    0,   0, 255 },
		{ /* LIGHTGREEN	*/   10,   0, 255,   0 },
		{ /* LIGHTCYAN	*/   11,   0, 255, 255 },
		{ /* LIGHTRED	*/   12, 255,   0,   0 },
		{ /* LIGHTMAGENTA */ 13, 255,   0, 255 },
		{ /* YELLOW	*/   14, 255, 255,   0 },
		{ /* WHITE	*/   15, 255, 255, 255 },
		{ /* BROWN	*/   20,  50,   0,   0 },
		{ /* DARKGRAY	*/   56,  63,  63,  63 },
		{ /* LIGHTBLUE	*/   57,   0,   0, 255 },
		{ /* LIGHTGREEN	*/   58,   0, 255,   0 },
		{ /* LIGHTCYAN	*/   59,   0, 255, 255 },
		{ /* LIGHTRED	*/   60, 255,   0,   0 },
		{ /* LIGHTMAGENTA */ 61, 255,   0, 255 },
		{ /* YELLOW	*/   62, 255, 255,   0 },
		{ /* WHITE	*/   63, 255, 255, 255 },
		{		     -1,   0,   0,   0 }
	};
	int i, j, Color, RGBIColor[3];

	if (AttrGetObjectRGBColor(PObj,
		&RGBIColor[0], &RGBIColor[1], &RGBIColor[2])) {
			for (i = 0; i < 3; i++)
				RGB[i] = RGBIColor[i] / 255.0;

			return TRUE;
	}
	else if ((Color = AttrGetObjectColor(PObj)) != IP_ATTR_NO_COLOR) {
		for (i = 0; TransColorTable[i][0] >= 0; i++) {
			if (TransColorTable[i][0] == Color) {
				for (j = 0; j < 3; j++)
					RGB[j] = TransColorTable[i][j+1] / 255.0;
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the volumetric texture of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its volumetric texture.                            *
*                                                                            *
* RETURN VALUE:                                                              *
*   char *:    Name of volumetric texture map to apply, NULL if none.        *
*****************************************************************************/
const char *CGSkelGetObjectTexture(IPObjectStruct *PObj)
{
	return AttrGetObjectStrAttrib(PObj, "texture");
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the parametric texture of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its parametric texture.                            *
*                                                                            *
* RETURN VALUE:                                                              *
*   char *:    Name of parametric texture map to apply, NULL if none.        *
*****************************************************************************/
const char *CGSkelGetObjectPTexture(IPObjectStruct *PObj)
{
	return AttrGetObjectStrAttrib(PObj, "ptexture");
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the transparency level of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its volumetric texture.                            *
*   Transp: Transparency level between zero and one.                         *
*                                                                            *
* RETURN VALUE:                                                              *
*   int:    TRUE if object has transparency, FALSE otherwise.                *
*****************************************************************************/
int CGSkelGetObjectTransp(IPObjectStruct *PObj, double *Transp)
{
	*Transp = AttrGetObjectRealAttrib(PObj, "transp");

	return !IP_ATTR_IS_BAD_REAL(*Transp);
}

