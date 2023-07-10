#pragma once

#define IDENTITY4X4 { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} }
#define MAX_POLYGON_VERTICES 8

#define MIN_CLOSE_CLIP 4
#define MAX_CLOSE_CLIP 20

#define MIN_FAR_CLIP 50
#define MAX_FAR_CLIP 200

#define MIN_D 1
#define DEFAULT_D 4
#define MAX_D 30
// e.g: X = X / Z * (D / D_DIVIDER)
#define D_DIVIDER 5

#define MIN_ORTHOGRAPHIC_SIZE 1
#define DEFAULT_ORTHOGRAPHIC_SIZE 5
#define MAX_ORTHOGRAPHIC_SIZE 20

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

#define MIN_SCALE_SENSITIVITY 200
#define MAX_SCALE_SENSITIVITY 500

#define MIN_ROTATE_SENSITIVITY 150
#define MAX_ROTATE_SENSITIVITY 500

#define MIN_TRANSLATE_SENSITIVITY 120
#define MAX_TRANSLATE_SENSITIVITY 500

#define DEFAULT_FOG_FAR 30
#define MIN_FOG_FAR 20
#define MAX_FOG_FAR 100

#define DEFAULT_MOTION_BLUR 0.25
#define MIN_MOTION_BLUR 0    //Amplified by 100
#define MAX_MOTION_BLUR 90  //Amplified by 100

#define DEFAULT_TESSELATION 2
#define MIN_TESSELATION 2
#define MAX_TESSELATION 20

#define CLICK_THRESHOLD_DISTANCE 15.0



enum class Space
{
	WorldSpace,
	ObjectSpace
};

enum class ImageMode
{
	Repeat,
	Stretch
};

enum class BackgroundType
{
	SolidColor,
	Image
};

enum class NormalType
{
	Calculated,
	Imported
};

enum class FogMode
{
	Color,
	Transparent,
	Off
};

enum class KernelType
{
	Box = 0,
	Triangle = 1,
	Sinc = 2,
	Gaussian = 3,
	None = 4,
};

enum class KernelSize
{
	Size3 = 0,
	Size5 = 1,
};