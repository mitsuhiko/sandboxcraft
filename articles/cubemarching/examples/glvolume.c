#include "glvolume.h"

/*
	Volumetric visualisation
	A place to test out ideas.
*/

/* Global Flags */
int debug = FALSE;
int fullscreen = FALSE;
int modeldirty = GEOMDIRTY;
int stereo = FALSE;
int showgrid = TRUE;
int uselights = TRUE;
int nlights = 2;
int drawwireframe = FALSE;
int dosmooth = TRUE;
int polyorder = ANTICLOCKWISE;
int usespecular = FALSE;
int showbbox = TRUE;
int showaxes = TRUE;
int showlightpos = FALSE;
int showhistogram = TRUE;
int showinfo = TRUE;
int showusage = FALSE;
int autorampiso = 0;
int failsafe = FALSE;			/* Whether or not to restruct polygon numbers */
XYZ autospin = {0.0,0.0,0.0}; /* 0, 1, or -1 for still, left, right */
int currentbutton = -1;
int windowdump = FALSE;
int movierecord = FALSE;

/* Camera variables */
CAMERA camera;

/* Generally useful things */
XYZ origin = {0.0,0.0,0.0};
XYZ xaxis = {1.0,0.0,0.0};
GLfloat white[4] = {1.0,1.0,1.0,1.0};
GLfloat grey[4] = {0.5,0.5,0.5,1.0};
GLfloat dkgrey[4] = {0.25,0.25,0.25,1.0};
GLfloat black[4] = {0.0,0.0,0.0,1.0};

/* Grid globals */
VOLUME grid;

/* Drawing mode */
int drawingmode = ISOPOINTS;
int colourmap = 1;
int pointlinesize = 1;
COLOUR constcolour = {1.0,0.0,0.0};
double framerate = 0;
XYZ lightpositions[3];

int main(int argc,char **argv)
{
	int i;
	char volname[64];

	/* Initialise things */
	volname[0] = '\0';
	grid.data = NULL;
	grid.Nx = 50; grid.Ny = 50; grid.Nz = 50;
	grid.Rx = 2; grid.Ry = 2; grid.Rz = 2;
	grid.resolution = 2;
	grid.Dx = 1; grid.Dy = 1; grid.Dz = 1;
	grid.isolevel = 256;
	HandleDataMenu(10);

	/* Parse the command line arguments */
   for (i=1;i<argc;i++) {
      if (strcmp(argv[i],"-h") == 0) 
         GiveUsage(argv[0]);
		if (strcmp(argv[i],"-d") == 0)
			debug = TRUE;
		if (strcmp(argv[i],"-s") == 0)
			stereo = TRUE;
      if (strcmp(argv[i],"-f") == 0)
         fullscreen = TRUE;
		
      if (strcmp(argv[i],"-as") == 0) {
         autospin.x = 1;
         autospin.y = 0;
         autospin.z = 0;
      }
		if (strcmp(argv[i],"-ab") == 0)
			autorampiso = 1;
		if (strcmp(argv[i],"-aB") == 0)
			autorampiso = -1;
      if (strcmp(argv[i],"-v") == 0) {
         i++;
         if (i >= argc) {
            GiveUsage(argv[0]);
            continue;
         }
         strcpy(volname,argv[i]);
      }
   }

	/* Set up the form library */
	fl_initialize(&argc,argv,"Volume Visualisation",0,0);

   /* Set things (glut) up */
   glutInit(&argc,argv);
   if (!stereo)
      glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   else
      glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);

	/* Create the window and handlers */
   glutCreateWindow("Volume Visualisation");
	camera.screenwidth = 800;
	camera.screenheight = 600;
	glutReshapeWindow(camera.screenwidth,camera.screenheight);
   if (fullscreen)
      glutFullScreen();
   glutDisplayFunc(HandleDisplay);
	glutReshapeFunc(HandleReshape);
   glutVisibilityFunc(HandleVisibility);
   glutKeyboardFunc(HandleKeyboard);
   glutSpecialFunc(HandleSpecialKeyboard);
   glutMouseFunc(HandleMouse);
	glutMotionFunc(HandleMouseMotion);
   CreateEnvironment();
	/* glutSetCursor(GLUT_CURSOR_NONE); */

	CreateMenus();

	/* Read data */
	if (strlen(volname) > 0) {
		ReadVolume(volname);
   	CalcBounds();
   	UpdateResolution(0);
	}

	/* Ready to go! */
	if (debug)
		fprintf(stderr,"Starting\n");
	CameraHome(0);
   glutMainLoop();
   return(0);
}

/*
   This is where global OpenGL/GLUT settings are made, 
	that is, things that will not change in time 
*/
void CreateEnvironment(void)
{
   glEnable(GL_DEPTH_TEST);
   glDisable(GL_LINE_SMOOTH);
   glDisable(GL_POINT_SMOOTH);
   glDisable(GL_POLYGON_SMOOTH); 
   glDisable(GL_DITHER);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glLineWidth(1.0);
   glPointSize(1.0);
   glClearColor(0.0,0.0,0.0,0.0);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
}

/*
   This is the basic display callback routine
   It creates the geometry, lighting, and viewing position
*/
void HandleDisplay(void)
{
	int i,j;
   XYZ r,eyepos;
   double dist,ratio,radians,scale,wd2,ndfl;
	double near,far;
   double left,right,top,bottom;
   static int framecount = -1;
   static double tstart,tstop;         /* Frame rate calculations */

   /* Set the time the first time around */
   if (framecount < 0) {
      framecount = 0;
      tstart = GetRunTime();
   }
   framecount++;

   /* Clip to avoid extreme stereo */
   near = 0.1;
   far = 2*(grid.Nx*grid.Dx+grid.Ny*grid.Dy+grid.Nz*grid.Dz);
   if (stereo)
      near = camera.focallength / 5;

   /* Misc stuff */
   ratio  = camera.screenwidth / (double)camera.screenheight;
   radians = DTOR * camera.aperture / 2;
   wd2     = near * tan(radians);
   ndfl    = near / camera.focallength;

   /* Are we in autospin mode */
   if ((int)autospin.x != 0)
      RotateCamera(autospin.x/2,0.0,0.0,KEYBOARDCONTROL);
   if ((int)autospin.y != 0)
      RotateCamera(0.0,autospin.y/2,0.0,KEYBOARDCONTROL);
   if ((int)autospin.z != 0)
      RotateCamera(0.0,0.0,autospin.z/2,KEYBOARDCONTROL);

	/* Are we in autospin isolevel mode */
	if (autorampiso != 0) {
		grid.isolevel += autorampiso;
		if (grid.isolevel > grid.themax)
			grid.isolevel = grid.themin;
		if (grid.isolevel < grid.themin)
			grid.isolevel = grid.themax;
		modeldirty = GEOMDIRTY;
	}

   /* Derive the the "right" vector */
   CROSSPROD(camera.vd,camera.vu,r);
   Normalise(&r);
   r.x *= camera.eyesep / 2.0;
   r.y *= camera.eyesep / 2.0;
   r.z *= camera.eyesep / 2.0;

	/* Do we need to create the geometry */
	MakeGeometry();

   /* Clear the buffers */
   glDrawBuffer(GL_BACK_LEFT);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   if (stereo) {
      glDrawBuffer(GL_BACK_RIGHT);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   if (stereo) {

      /* Derive the two eye positions */
      CROSSPROD(camera.vd,camera.vu,r);
      Normalise(&r);
      r.x *= camera.eyesep / 2.0;
      r.y *= camera.eyesep / 2.0;
      r.z *= camera.eyesep / 2.0;
		eyepos = VectorAdd(camera.vp,r);

      glMatrixMode(GL_MODELVIEW);
      glDrawBuffer(GL_BACK_RIGHT);
      glLoadIdentity();
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      left  = - ratio * wd2 - 0.5 * camera.eyesep * ndfl;
      right =   ratio * wd2 - 0.5 * camera.eyesep * ndfl;
      top    =   wd2;
      bottom = - wd2;
      glFrustum(left,right,bottom,top,near,far);
      gluLookAt(eyepos.x,eyepos.y,eyepos.z,
                eyepos.x + camera.vd.x,
                eyepos.y + camera.vd.y,
                eyepos.z + camera.vd.z,
                camera.vu.x,camera.vu.y,camera.vu.z);
		MakeEnv();
      glCallList(1);
		DrawExtras();

      glMatrixMode(GL_MODELVIEW);
      glDrawBuffer(GL_BACK_LEFT);
      glLoadIdentity();
      eyepos = VectorSub(r,camera.vp);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      left  = - ratio * wd2 + 0.5 * camera.eyesep * ndfl;
      right =   ratio * wd2 + 0.5 * camera.eyesep * ndfl;
      top    =   wd2;
      bottom = - wd2;
      glFrustum(left,right,bottom,top,near,far);
      gluLookAt(eyepos.x,eyepos.y,eyepos.z,
                eyepos.x + camera.vd.x,
                eyepos.y + camera.vd.y,
                eyepos.z + camera.vd.z,
                camera.vu.x,camera.vu.y,camera.vu.z);
		MakeEnv();
      glCallList(1);
		DrawExtras();

   } else {

      glMatrixMode(GL_MODELVIEW);
      glDrawBuffer(GL_BACK_LEFT);
      glLoadIdentity();
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      left  = - ratio * wd2;
      right =   ratio * wd2;
      top    =   wd2;
      bottom = - wd2;
      glFrustum(left,right,bottom,top,near,far);
      gluLookAt(camera.vp.x,camera.vp.y,camera.vp.z,
                camera.vp.x + camera.vd.x,
                camera.vp.y + camera.vd.y,
                camera.vp.z + camera.vd.z,
                camera.vu.x,camera.vu.y,camera.vu.z);
		MakeEnv();
		glCallList(1);
		DrawExtras();
   }
	
	/* Optionally dump image */
	if (windowdump || movierecord) {
		WindowDump("",camera.screenwidth,camera.screenheight,stereo,-5);
		windowdump = FALSE;
	}

	/* Let's look at it */
   glutSwapBuffers(); 

	/* Frame rate calculations */
   tstop = GetRunTime();
   if (tstop - tstart > 5) {
      framerate = framecount / (tstop - tstart);
      if (debug)
         fprintf(stderr,"Frame rate = %.1f frames/second\n",framerate);
      framecount = 0;
      tstart = tstop;
	}
}

void MakeEnv(void)
{
   GLfloat shin1[1] = {0.0},shin2[1] = {40.0};
   GLfloat white[4] = {1.0,1.0,1.0,1.0};

   MakeLighting();

   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   if (!usespecular)
      glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shin1);
   else
      glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shin2);

   if (polyorder == CLOCKWISE)
      glFrontFace(GL_CW);
   else
      glFrontFace(GL_CCW);

   if (drawwireframe)
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
   else
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

   if (dosmooth)
      glShadeModel(GL_SMOOTH);
   else
      glShadeModel(GL_FLAT);

   if (uselights)
      glEnable(GL_LIGHTING);
   else
      glDisable(GL_LIGHTING);
}

/*
	Create the geometry
	Use a display list so this is only called whenever
	some changes the displayed geometry.
*/
void MakeGeometry(void)
{
	if (modeldirty == GEOMDIRTY) {
   	glNewList(1,GL_COMPILE);
		switch (drawingmode) {
		case POINTSABOVE:
			DrawPoints(POINTSABOVE,pointlinesize,colourmap);
			break;
		case POINTSBELOW:
			DrawPoints(POINTSBELOW,pointlinesize,colourmap);
			break;
   	case LINESABOVE:
   	   DrawLines(LINESABOVE,pointlinesize,colourmap);
   	   break;
   	case LINESBELOW:
   	   DrawLines(LINESBELOW,pointlinesize,colourmap);
   	   break;
		case ISOPOINTS:
			DrawIsopoints(pointlinesize,colourmap);
			break;
		case ISOSURFACE:
			DrawIsoSurface(colourmap);
			break;
		case RENDER:
			break;
		}
		glEndList();
	}

	modeldirty = NOTDIRTY;
}

/*
   Set up the lighting environment
*/
void MakeLighting(void)
{
	int i;
   int deflightlist[8] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,
                          GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};
	GLfloat globalambient[4] = {0.3,0.3,0.3,1.0};
   GLfloat white[4] = {1.0,1.0,1.0,1.0};
   GLfloat black[4] = {0.0,0.0,0.0,1.0};
	GLfloat p[4] = {0.0,0.0,0.0,1.0};

   /* Turn off all the lights */
   for (i=0;i<8;i++) {
      glDisable(deflightlist[i]);
      glLightfv(deflightlist[i],GL_AMBIENT,black);
		glLightfv(deflightlist[i],GL_DIFFUSE,white);
		if (usespecular)
			glLightfv(deflightlist[i],GL_SPECULAR,white);
		else
			glLightfv(deflightlist[i],GL_SPECULAR,black);
	}

   lightpositions[0].x =  grid.Nx * grid.Dx * 1.5;
   lightpositions[0].y =  grid.Ny * grid.Dy / 2.0;
   lightpositions[0].z =  grid.Nz * grid.Dz / 2.0;
   lightpositions[1].x = -grid.Nx * grid.Dx / 2.0;
   lightpositions[1].y = -grid.Ny * grid.Dy / 2.0;
   lightpositions[1].z = -grid.Nz * grid.Dz / 2.0;
   lightpositions[2].x = -grid.Nx * grid.Dx / 2.0;
   lightpositions[2].y =  grid.Ny * grid.Dy * 1.5;
   lightpositions[2].z =  grid.Nz * grid.Dz * 1.5;

   /* Turn on the appropriate lights */
	for (i=0;i<nlights;i++) {
		p[0] = lightpositions[i].x;
		p[1] = lightpositions[i].y;
		p[2] = lightpositions[i].z;
		p[3] = 1;
   	glLightfv(deflightlist[i],GL_POSITION,p);
		glLightfv(deflightlist[i],GL_DIFFUSE,white);
   	glEnable(deflightlist[i]);
	}

	/* Ambient light */
   switch (drawingmode) {
   case POINTSABOVE:
   case POINTSBELOW:
   case LINESABOVE:
   case LINESBELOW:
   case ISOPOINTS:
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,white);
      break;
   case ISOSURFACE:
   case RENDER:
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,globalambient);
      break;
   }
}

/*
   Create the extra bits of fluff
   Generally things that don't take long to create
   Leave the graphics state in the same state as when we entered
*/
void DrawExtras(void)
{
	int i;
	XYZ c;
   XYZ pmin={0.0,0.0,0.0},pmax;

   glDisable(GL_LIGHTING);

   /* Show the grid */
   if (showgrid) {
      glColor4f(0.5,0.5,0.5,1.0);
      DrawGrid();
   }

	/* Draw the axes */
	if (showaxes) {
		glColor4f(1.0,0.0,0.0,1.0);
		DrawTheAxes();
	}

	/* Optionally show the light source positions */
	if (showlightpos || debug) {
		for (i=0;i<nlights;i++)
			CreateSimpleSphere(lightpositions[i],2,16,0);
	}

   /* Show the bounding box ? */
   if (showbbox) {
      pmax.x = grid.Dx * (grid.Nx-1); 
		pmax.y = grid.Dy * (grid.Ny-1); 
		pmax.z = grid.Dz * (grid.Nz-1);
      glColor4f(1.0,0.0,0.0,1.0);
      glLineWidth(2.0);
      CreateBox(pmin,pmax,0);
      glLineWidth(1.0);
   }

   /* Draw the histogram */
   if (showhistogram) {
		glDisable(GL_DEPTH_TEST); 
      DrawHistogram(colourmap);
		glEnable(GL_DEPTH_TEST);
   }

   if (showusage) {
		glDisable(GL_DEPTH_TEST); 
      glColor4f(0.5,1.0,0.5,1.0);
      DrawUsage(10,camera.screenheight);
		glEnable(GL_DEPTH_TEST);
   }
	
	if (showinfo) {
		glDisable(GL_DEPTH_TEST); 
      glColor4f(1.0,0.5,0.5,1.0);
      DrawInfo(HISTLENGTH+50,0);
		glEnable(GL_DEPTH_TEST); 
   }
}

/*
	Read the volume data file
	The format is given 
		http://www.swin.edu.au/astronomy/pbourke/dataformats
	Only handle 8 and 16 bit at the moment (short int)
*/
int ReadVolume(char *fname)
{
	int i,j,k,index;
	int c;
	char c8;
	short int c16;
	int nx,ny,nz;
	double dx,dy,dz;
	double xmin,ymin,zmin,dmin;
	int datasize,endian;
	int status = TRUE;
   const char *fn;
   FILE *fptr;

	/* Open the file, prompt if necessary */
   if (strlen(fname) < 1) {
      fn = fl_show_file_selector("Volume file","","","");
      if (fn == NULL || strlen(fn) <= 0)
         return(TRUE);
      if ((fptr = fopen(fn,"r")) == NULL) {
         fl_show_message("File Error","Unable to open volume data file",fn);
         return(FALSE);
      }
   } else {
      if ((fptr = fopen(fname,"r")) == NULL) {
         fl_show_message("File Error","Unable to open volume data file",fname);
         status = FALSE;
			goto skip;
      }
   }

	/* Read the header */
	while ((c = fgetc(fptr)) != '\n' && c != EOF)
		;
	if (fscanf(fptr,"%d %d %d",&nx,&ny,&nz) != 3) {
		fl_show_message("File Read Error","Failed to read grid dimensions","");
		status = FALSE;
      goto skip;
	}
   if (fscanf(fptr,"%lf %lf %lf",&dx,&dy,&dz) != 3) {
      fl_show_message("File Read Error","Failed to read grid size","");
      status = FALSE;
      goto skip;
   }
	dmin = MIN(dx,dy);
	dmin = MIN(dmin,dz);
	if (dmin <= 0) {
      fl_show_message("File Read Error","Read illegal grid size","");
      status = FALSE;
      goto skip;
	}
	dx /= dmin;
	dy /= dmin;	
	dz /= dmin;
   if (fscanf(fptr,"%lf %lf %lf",&xmin,&ymin,&zmin) != 3) {
      fl_show_message("File Read Error","Failed to read grid lower corner","");
      status = FALSE;
      goto skip;
   }
   if (fscanf(fptr,"%d %d",&datasize,&endian) != 2) {
      fl_show_message("File Read Error","Failed to read grid data type","");
      status = FALSE;
      goto skip;
   }
	if (datasize != 8 && datasize != 16) {
		fl_show_message("File Read Error","Unsupported cell size","");
		status = FALSE;
		goto skip;
	}

	/* Read to the end of the line */
   while ((c = fgetc(fptr)) != '\n' && c != EOF)
      ;

	/* Read the data */
	grid.Nx = nx; grid.Ny = ny; grid.Nz = nz;
	grid.Dx = dx; grid.Dy = dy; grid.Dz = dz;
	MallocGrid(nx,ny,nz);
	index = 0;
	for (k=0;k<nz;k++) {
		for (j=0;j<ny;j++) {
			for (i=0;i<nx;i++) {
				if (datasize == 8) {
					fread(&c8,sizeof(char),1,fptr);
					grid.data[index] = c8;
				} else {
					fread(&c16,sizeof(short int),1,fptr);
					grid.data[index] = c16;
				}
				index++;
			}
		}
	}

	skip:
   fclose(fptr);
	return(status);
}

/*
	Write a volume
*/
int SaveVolume(char *s)
{
   int i,j,k;
	long index;
   const char *fn;
   char fname[256];
   FILE *fptr;

   /* Get the file name */
	if (strlen(s) < 1) {
   	fn = fl_show_file_selector("Volume file","","","");
   	if (fn == NULL || strlen(fn) <= 0)
   	   return(FALSE);
   	strcpy(fname,fn);
	} else {
		strcpy(fname,s);
	}

   /* Try to open the file */
   if ((fptr = fopen(fname,"w")) == NULL) {
		fl_show_message("File Save Error","fname","");
      return(FALSE);
	}

	fprintf(fptr,"Volume by Paul Bourke\n");
	fprintf(fptr,"%d %d %d\n",grid.Nx,grid.Ny,grid.Nz);
	fprintf(fptr,"%g %g %g\n",grid.Dx,grid.Dy,grid.Dz);
	fprintf(fptr,"0.0 0.0 0.0\n");
	fprintf(fptr,"16 1\n");

	for (k=0;k<grid.Nz;k++) {
		for (j=0;j<grid.Ny;j++) {
			for (i=0;i<grid.Nx;i++) {
				index = k*grid.Nx*grid.Ny + j*grid.Nx + i;
				fwrite(&(grid.data[index]),sizeof(short int),1,fptr);
			}
		}
	}

	fclose(fptr);
	return(TRUE);
}

/*
   Deal with plain key strokes
*/
void HandleKeyboard(unsigned char key,int x,int y)
{
	switch (key) {
   case ESC: 									/* Quit */
   case 'Q':
   case 'q': 
		exit(0); 
		break;
   case 'h':                           /* Go home     */
   case 'H':
      CameraHome(0);
      break;
	case 'w':                           /* Save one image */
	case 'W':
		windowdump = TRUE;
		break;
   case 'r':                           /* Toggle image recording */
   case 'R':
      movierecord = !movierecord;
      break;
   case '[':                           /* Roll anti clockwise */
      RotateCamera(0.0,0.0,-1.0,MOUSECONTROL);
      break;
   case ']':                           /* Roll clockwise */
      RotateCamera(0.0,0.0,1.0,MOUSECONTROL);
      break;
   case 'i':                           /* Translate camera up */
	case 'I':
      TranslateCamera(0,1);
      break;
   case 'k':                           /* Translate camera down */
	case 'K':
      TranslateCamera(0,-1);
      break;
   case 'j':                           /* Translate camera left */
	case 'J':
      TranslateCamera(-1,0);
      break;
   case 'l':                           /* Translate camera right */
	case 'L':
      TranslateCamera(1,0);
      break;
	case '+':									/* Move forward */
	case '=':
		FlyCamera(1);
		break;
   case '-':                           /* Move backward */
   case '_':
		FlyCamera(-1);
      break;
   case 'a':                           /* Start/stop autospin */
   case 'A':
      if (autospin.x != 0 || autospin.y != 0 || autospin.z != 0) 
         autospin = origin;
      else 
         autospin = xaxis;
      break;
	case 'b':
	case 'B':
		if (autorampiso == 0)
			autorampiso = 1;
		else if (autorampiso == 1)
			autorampiso = -1;
		else if (autorampiso == -1)
			autorampiso = 0;
		break;
	case '<':									/* Decrease isolevel */
	case ',':
      grid.isolevel--;
      if (grid.isolevel < grid.themin)
         grid.isolevel = grid.themax;
      modeldirty = GEOMDIRTY;
		break;
	case '>':									/* Increase isolevel */
	case '.':
      grid.isolevel++;
      if (grid.isolevel > grid.themax) 
         grid.isolevel = grid.themin;  
      modeldirty = GEOMDIRTY;   
		break;
	}
}

/*
   Deal with special key strokes
*/
void HandleSpecialKeyboard(int key,int x, int y)
{
   switch (key) {
   case GLUT_KEY_LEFT:
      RotateCamera(-1.0,0.0,0.0,MOUSECONTROL);
      if (autospin.x != 0 || autospin.y != 0 || autospin.z != 0) 
         autospin = xaxis;
      break;
   case GLUT_KEY_RIGHT:
      RotateCamera(1.0,0.0,0.0,MOUSECONTROL);
      if (autospin.x != 0 || autospin.y != 0 || autospin.z != 0) {
         autospin = origin;
         autospin.x = -1;
      }
      break;
   case GLUT_KEY_UP:
      RotateCamera(0.0,1.0,0.0,MOUSECONTROL);
      if (autospin.x != 0 || autospin.y != 0 || autospin.z != 0) {
         autospin = origin;
         autospin.y = 1;
      }
      break;
   case GLUT_KEY_DOWN:
      RotateCamera(0.0,-1.0,0.0,MOUSECONTROL);
      if (autospin.x != 0 || autospin.y != 0 || autospin.z != 0) {
         autospin = origin;
         autospin.y = -1;
      }
      break;
   case GLUT_KEY_F1:
      showusage = !showusage;
      break;
   case GLUT_KEY_F2:
      showhistogram = !showhistogram;
      break;
   case GLUT_KEY_F3:
      showinfo = !showinfo;
      break;
	case GLUT_KEY_F4:
		showbbox = !showbbox;
		break;
	case GLUT_KEY_F5:
		showaxes = !showaxes;
		break;
   case GLUT_KEY_F6:
      showgrid = !showgrid;
      break;
   }
}

/*
   Rotate (ix,iy) or roll (iz) the camera about the focal point
   ix,iy,iz are flags, 0 do nothing, +- 1 rotates in opposite directions
   Correctly updating all camera attributes
*/
void RotateCamera(double ix,double iy,double iz,int mode)
{
   XYZ vp,vu,vd;
   XYZ right;
   XYZ newvp,newr;
   double radius,dd,radians,dtheta = 1;
   double dx,dy,dz;

   vu = camera.vu;
   Normalise(&vu);
   vp = camera.vp;
   vd = camera.vd;
   Normalise(&vd);
   CROSSPROD(vd,vu,right);
   Normalise(&right);
   radians = dtheta * PI / 180.0;

   /* Handle the roll */
   if (iz != 0) {
      camera.vu.x += iz * right.x * radians;
      camera.vu.y += iz * right.y * radians;
      camera.vu.z += iz * right.z * radians;
      Normalise(&camera.vu);
      return;
   }

   /* Distance from the rotate point */
   dx = camera.vp.x - camera.pr.x;
   dy = camera.vp.y - camera.pr.y;
   dz = camera.vp.z - camera.pr.z;
   radius = sqrt(dx*dx + dy*dy + dz*dz);

   /* Determine the new view point */
   dd = radius * radians;
   newvp.x = vp.x + dd * ix * right.x + dd * iy * vu.x - camera.pr.x;
   newvp.y = vp.y + dd * ix * right.y + dd * iy * vu.y - camera.pr.y;
   newvp.z = vp.z + dd * ix * right.z + dd * iy * vu.z - camera.pr.z;
   Normalise(&newvp);
   camera.vp.x = camera.pr.x + radius * newvp.x;
   camera.vp.y = camera.pr.y + radius * newvp.y;
   camera.vp.z = camera.pr.z + radius * newvp.z;

   /* Determine the new right vector */
   newr.x = camera.vp.x + right.x - camera.pr.x;
   newr.y = camera.vp.y + right.y - camera.pr.y;
   newr.z = camera.vp.z + right.z - camera.pr.z;
   Normalise(&newr);
   newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
   newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
   newr.z = camera.pr.z + radius * newr.z - camera.vp.z;

   camera.vd.x = camera.pr.x - camera.vp.x;
   camera.vd.y = camera.pr.y - camera.vp.y;
   camera.vd.z = camera.pr.z - camera.vp.z;
   Normalise(&camera.vd);

   /* Determine the new up vector */
   CROSSPROD(newr,camera.vd,camera.vu);
   Normalise(&camera.vu);
}

/*
   Translate (pan) the camera view point
   In response to i,j,k,l keys
   Also move the camera rotate location in parallel
*/
void TranslateCamera(int ix,int iy)
{
   XYZ vp,vu,vd;
   XYZ right;
   XYZ newvp,newr;
   double radians,delta,dtheta = 1;

   vu = camera.vu;
   Normalise(&vu);
   vp = camera.vp;
   vd = camera.vd;
   Normalise(&vd);
   CROSSPROD(vd,vu,right);
   Normalise(&right);
   radians = dtheta * PI / 180.0;
   delta = dtheta * camera.focallength / 90.0;

   camera.vp.x += iy * vu.x * delta;
   camera.vp.y += iy * vu.y * delta;
   camera.vp.z += iy * vu.z * delta;
   camera.pr.x += iy * vu.x * delta;
   camera.pr.y += iy * vu.y * delta;
   camera.pr.z += iy * vu.z * delta;

   camera.vp.x += ix * right.x * delta;
   camera.vp.y += ix * right.y * delta;
   camera.vp.z += ix * right.z * delta;
   camera.pr.x += ix * right.x * delta;
   camera.pr.y += ix * right.y * delta;
   camera.pr.z += ix * right.z * delta;
}

/*
   Handle mouse events
	Right button events are passed to menu handlers
*/
void HandleMouse(int button,int state,int x,int y)
{
	if (debug)
		fprintf(stderr,"%d %d\n",x,y);

	if (showhistogram)
		HistogramClick(x,camera.screenheight-y);

   if (state == GLUT_DOWN) {
      if (button == GLUT_LEFT_BUTTON) {
			currentbutton = GLUT_LEFT_BUTTON;
      } else if (button == GLUT_MIDDLE_BUTTON) {
			currentbutton = GLUT_MIDDLE_BUTTON;
      } 
   }
}

/*
   How to handle visibility
*/
void HandleVisibility(int visible)
{
   if (visible == GLUT_VISIBLE)
      HandleTimer(0);
   else
      ;
}

/*
   What to do on an timer event
*/
void HandleTimer(int value)
{
   glutPostRedisplay();
	glutTimerFunc(30,HandleTimer,0);
}

/*
   Handle a window reshape/resize
*/
void HandleReshape(int w,int h)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glViewport(0,0,(GLsizei)w,(GLsizei)h);
   camera.screenwidth = w;
   camera.screenheight = h;
}

/*
	Move the camera to the home position 
	The center is the center of the volume box
*/
void CameraHome(int mode)
{
	camera.aperture = 60;
	camera.pr.x = grid.Dx * grid.Nx / 2;
   camera.pr.y = grid.Dy * grid.Ny / 2;
   camera.pr.z = grid.Dz * grid.Nz / 2;

	switch (mode) {
	case 2:	/* Top */
      camera.vu.x = 1; camera.vu.y = 0; camera.vu.z = 0;
      camera.vp.x = 0; camera.vp.y = 1; camera.vp.z = 0;
		break;
	case 3:	/* Side */
      camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
      camera.vp.x = 0; camera.vp.y = 0; camera.vp.z = 1;
		break;
	case 4:	/* axo */
      camera.vu.x = 0;   camera.vu.y = 1;   camera.vu.z = 0;
      camera.vp.x = 0.5; camera.vp.y = 0.5; camera.vp.z = 0.5;
		break;
	case 0:
	case 1:	/* Front */
	default:
   	camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
   	camera.vp.x = 1; camera.vp.y = 0; camera.vp.z = 0;
		break;
	}
	camera.vd.x = -camera.vp.x;
   camera.vd.y = -camera.vp.y;
   camera.vd.z = -camera.vp.z;
	Normalise(&camera.vd);
	camera.vp.x *= (grid.Nx*grid.Dx*1.5);
   camera.vp.y *= (grid.Ny*grid.Dy*1.5);
   camera.vp.z *= (grid.Nz*grid.Dz*1.5);
	camera.vp.x += camera.pr.x;
   camera.vp.y += camera.pr.y;
   camera.vp.z += camera.pr.z;

	/* Non stressful stereo setting */
	camera.focallength = VectorLength(camera.vp,camera.pr);
	camera.eyesep = camera.focallength / 30.0;
}

/*
   Handle mouse motion
*/
void HandleMouseMotion(int x,int y)
{
   static int xlast=-1,ylast=-1;
   int dx,dy;

   dx = x - xlast;
   dy = y - ylast;
   if (dx < 0)      dx = -1;
   else if (dx > 0) dx =  1;
   if (dy < 0)      dy = -1;
   else if (dy > 0) dy =  1;

   if (currentbutton == GLUT_LEFT_BUTTON)
      RotateCamera(-(double)dx,(double)dy,0.0,MOUSECONTROL);
   else if (currentbutton == GLUT_MIDDLE_BUTTON)
      RotateCamera(0.0,0.0,(double)dx,MOUSECONTROL);

   xlast = x;
   ylast = y;
}

/*
   Fly the camera forwards or backwards
*/
void FlyCamera(int dir)
{
   double delta = 1.0;

   camera.vp.x = camera.vp.x + dir * camera.vd.x * delta;
   camera.vp.y = camera.vp.y + dir * camera.vd.y * delta;
   camera.vp.z = camera.vp.z + dir * camera.vd.z * delta;
}

/*
	Malloc the 3D grid array
	At this stage don't bother recovering if it fails
*/
void MallocGrid(int nx,int ny,int nz)
{
	if (grid.data != NULL)
		free(grid.data);
	if ((grid.data = malloc(nx*ny*nz*sizeof(short int))) == NULL) {
		fprintf(stderr,"Error mallocing grid - fatal\n");
		exit(-1);
	}
}

/*
	Calculate the grid bounds
	And form the histogram
	Set a default isolevel
	Set an initial resolution
*/
void CalcBounds(void)
{
	int bin,i,index,nmax;
	long NxNyNz;
	long sum = 0;
	double dd;

	if (grid.data == NULL) 
		return;
	NxNyNz = grid.Nx*grid.Ny*grid.Nz;

	/* Find the range */
	grid.themin = 32656;	
	grid.themax = -65636;
	for (index=0;index<NxNyNz;index++) {
		grid.themax = MAX(grid.themax,grid.data[index]);
		grid.themin = MIN(grid.themin,grid.data[index]);
	}
	if (grid.themin >= grid.themax)
		grid.themax++;
	if (debug)
		fprintf(stderr,"Grid bounds: %d -> %d\n",grid.themin,grid.themax);

	/* Compute the histogram */
	for (i=0;i<HISTLENGTH;i++)
		grid.histogram[i] = 0;
	dd = grid.themax - grid.themin;
	for (index=0;index<NxNyNz;index++) {
		bin = 255 * (grid.data[index] - grid.themin) / dd;
		sum += grid.data[index];
		grid.histogram[bin]++;
	}
	if (debug) 
		for (i=0;i<HISTLENGTH;i++)
			fprintf(stderr,"%5d %d\n",i,grid.histogram[i]);

	/* Reset the isolevel */
	grid.isolevel = sum / (double)(NxNyNz);
	if (grid.isolevel < grid.themin || grid.isolevel > grid.themax)
		grid.isolevel = (grid.themax + grid.themin) / 2;

	/* Set an appropriate resolution */
	nmax = MAX(grid.Nx,grid.Ny);
	nmax = MAX(nmax,grid.Nz);
	grid.resolution = nmax / 20 + 1;
	if (grid.resolution > 10)
		grid.resolution = 10;
}

/*
	Update the resolution for each dimension
	Is an attempt to even the resolution for non square grids
	If mode is 0 update Nx, Ny, Nz
	If mode is 1 or -1, adjust the Nx, Ny, Nz appropriately
*/
void UpdateResolution(int mode)
{
	int nmin;

	/* Do any corrections */
	if (grid.resolution < 1)
		grid.resolution = 1;
	if (grid.resolution > grid.Nx)
		grid.resolution = grid.Nx / 2;
   if (grid.resolution > grid.Ny)
      grid.resolution = grid.Ny / 2;
   if (grid.resolution > grid.Nz)
      grid.resolution = grid.Nz / 2;

	/* Find the smallest length */
   nmin = MIN(grid.Nx,grid.Ny);
   nmin = MIN(nmin,grid.Nz);

	if (mode == 0) {
		/*
   	grid.Rx = grid.resolution * grid.Nx / nmin;
   	grid.Ry = grid.resolution * grid.Ny / nmin;
   	grid.Rz = grid.resolution * grid.Nz / nmin;
		*/
      grid.Rx = grid.resolution;
      grid.Ry = grid.resolution;
      grid.Rz = grid.resolution;
		modeldirty = GEOMDIRTY;
	}

	if (mode == 1) {
		if (grid.Rx < grid.Nx) {
			modeldirty = GEOMDIRTY;
			grid.Rx++;
		}
      if (grid.Ry < grid.Ny) {
			modeldirty = GEOMDIRTY;
			grid.Ry++;
		}
      if (grid.Rz < grid.Nz) {
			modeldirty = GEOMDIRTY;
			grid.Rz++;
		}
	}

	if (mode == -1) {
		if (grid.Rx > 1 && grid.Rx > grid.resolution) {
			modeldirty = GEOMDIRTY;
			grid.Rx--;
		}
      if (grid.Ry > 1 && grid.Ry > grid.resolution) {
			modeldirty = GEOMDIRTY;
			grid.Ry--;
		}
      if (grid.Rz > 1 && grid.Rz > grid.resolution) {
			modeldirty = GEOMDIRTY;
			grid.Rz--;
		}
	}

	if (debug)
		fprintf(stderr,"Resolution: %d x %d x %d\n",grid.Rx,grid.Ry,grid.Rz);
}



