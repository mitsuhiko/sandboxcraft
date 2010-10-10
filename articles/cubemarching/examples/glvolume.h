#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "paulslib.h"
#include "bitmaplib.h"
#include "opengllib.h"
#include "forms.h"
#include <GL/glut.h>

typedef struct {
   XYZ vp;              /* View position           */
   XYZ vd;              /* View direction vector   */
   XYZ vu;              /* View up direction       */
   XYZ pr;              /* Point to rotate about   */
   double focallength;  /* Focal Length along vd   */
   double aperture;     /* Camera aperture         */
   double eyesep;       /* Eye separation          */
	int screenheight,screenwidth;
} CAMERA;

/* Number of bins in the historgram */
#define HISTLENGTH 256

typedef struct {
	short int *data;
	int Nx,Ny,Nz;
	double Dx,Dy,Dz;
	int Rx,Ry,Rz;
	int themin,themax;
	int resolution;
	int histogram[HISTLENGTH];
	int isolevel;
} VOLUME;

#define KEYBOARDCONTROL 0
#define MOUSECONTROL    1

/* See colour ramp menu */
#define CONSTANTCOLOUR 100

/* Maximum drawing elements */
#define MAXPOINTS  500000
#define MAXLINES   500000
#define MAXPOLYGON 500000

/* Degrees of dirty */
#define NOTDIRTY     0
#define ENVDIRTY     1
#define GEOMDIRTY    2

/* Rendering modes  - these numbers match the menu numbers! */
#define POINTSABOVE  1
#define POINTSBELOW  2
#define LINESABOVE   3
#define LINESBELOW   4
#define ISOPOINTS    5
#define ISOSURFACE   6
#define RENDER       7

void HandleDisplay(void);
void CreateEnvironment(void);
void CreateWorld(void);
void MakeGeometry(void);
void MakeLighting(void);
void DrawExtras(void);
void MakeEnv(void);
int  ReadVolume(char *);
int  SaveVolume(char *);
void HandleKeyboard(unsigned char key,int x, int y);
void HandleSpecialKeyboard(int key,int x, int y);
void HandleMouse(int,int,int,int);
void HandleVisibility(int vis);
void HandleReshape(int,int);
void HandleMouseMotion(int,int);
void HandlePassiveMotion(int,int);
void HandleTimer(int);
void RotateCamera(double,double,double,int);
void TranslateCamera(int,int);
void CameraHome(int);
void FlyCamera(int);
void MallocGrid(int,int,int);
void CalcBounds(void);
void UpdateResolution(int);

/* Drawing */
void DrawTheAxes(void);
void DrawGrid(void);
void DrawHistogram(int);
void HistogramClick(int,int);
void DrawPoints(int,int,int);
void DrawLines(int,int,int);
void DrawIsopoints(int,int);
void DrawInfo(int,int);
void DrawIsoSurface(int); 
XYZ  CalcGridNormal(int,int,int);
void SendGridVertex(int,int,int);

/* Menus */
void CreateMenus(void);
void HandleCameraMenu(int);
void HandleDataMenu(int);
void HandleColourMenu(int);
void HandleRenderMenu(int);
void HandlePointMenu(int);
void HandleLightMenu(int);
void HandleToggleMenu(int);
void HandleDrawMenu(int);
void HandleResolMenu(int);
void HandleMainMenu(int);

/* Usage */
void GiveUsage(char *);
void DrawUsage(int,int);
