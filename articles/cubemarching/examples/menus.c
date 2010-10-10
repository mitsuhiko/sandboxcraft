#include "glvolume.h"

extern VOLUME grid;

extern int showhistogram;
extern int showgrid;
extern int showinfo;
extern int showusage;
extern int showaxes;
extern int showgrid;
extern int drawingmode;
extern int colourmap;
extern int pointlinesize;
extern int uselights;
extern int usespecular;
extern int nlights;
extern int drawwireframe;
extern int polyorder;
extern int dosmooth;
extern XYZ autospin;
extern int autorampiso;
extern int showbbox;
extern int modeldirty;
extern COLOUR constcolour;

extern int debug;

/* Create the menus */
void CreateMenus(void)
{
   int mainmenu,drawmenu,cameramenu;
	int datamenu,resolmenu,togglemenu;
	int colourmenu,pointmenu,rendermenu;
	int lightmenu;

   datamenu = glutCreateMenu(HandleDataMenu);
   glutAddMenuEntry("Read Volume",1);
	glutAddMenuEntry("Save Volume",2);
	glutAddMenuEntry("Save as Geom (Unimplemented)",3);
	glutAddMenuEntry("Save as PovRay (Unimplemented)",4);
	glutAddMenuEntry("-------------",9);
   glutAddMenuEntry("Sample data (sphere)",10);
	glutAddMenuEntry("Sample data (ramp)",11);
	glutAddMenuEntry("-------------",19);
   glutAddMenuEntry("Subsample x-y",20);
   glutAddMenuEntry("Subsample x-z",21);
   glutAddMenuEntry("Subsample y-z",22);
   glutAddMenuEntry("Subsample x-y-z",23);
	glutAddMenuEntry("Invert levels",30);

   cameramenu = glutCreateMenu(HandleCameraMenu);
   glutAddMenuEntry("Front",1);
   glutAddMenuEntry("Top",2);
   glutAddMenuEntry("Left",3);
   glutAddMenuEntry("Axonometric",4);

   lightmenu = glutCreateMenu(HandleLightMenu);
   glutAddMenuEntry("1",1);
   glutAddMenuEntry("2",2);
   glutAddMenuEntry("3",3);

   rendermenu = glutCreateMenu(HandleRenderMenu);
   glutAddMenuEntry("Toggle Wireframe/Solid",1);
   glutAddMenuEntry("Toggle Flat/Smooth shading",2);
   glutAddMenuEntry("Toggle Lights",3);
	glutAddSubMenu("Number of lights",lightmenu);
	glutAddMenuEntry("Toggle specular hilights",5);
	glutAddMenuEntry("Toggle CW/CCW",4);

   colourmenu = glutCreateMenu(HandleColourMenu);
   glutAddMenuEntry("blue-cyan-green-magenta-red",1);
   glutAddMenuEntry("blue-red",2);
   glutAddMenuEntry("grey scale",3);
   glutAddMenuEntry("red-yellow-green-cyan-blue-magenta-red",4);
   glutAddMenuEntry("green-yellow",5);
   glutAddMenuEntry("green-magenta",6);
   glutAddMenuEntry("blue-green-red-green-blue",7);
   glutAddMenuEntry("white-black-white",8);
   glutAddMenuEntry("red-blue-cyan-magenta",9);
   glutAddMenuEntry("blue-cyan-green-yellow-white",10);
   glutAddMenuEntry("landscape",13);
   glutAddMenuEntry("constant",100);

	pointmenu = glutCreateMenu(HandlePointMenu);
   glutAddMenuEntry("1",1);
   glutAddMenuEntry("2",2);
   glutAddMenuEntry("3",3);
   glutAddMenuEntry("4",4);
	glutAddMenuEntry("5",5);

   resolmenu = glutCreateMenu(HandleResolMenu);
   glutAddMenuEntry("1",1);
   glutAddMenuEntry("2",2);
	glutAddMenuEntry("3",3);
   glutAddMenuEntry("4",4);
	glutAddMenuEntry("5",5);
	glutAddMenuEntry("6",6);
	glutAddMenuEntry("7",7);
	glutAddMenuEntry("8",8);
   glutAddMenuEntry("9",9);
	glutAddMenuEntry("10",10);

   togglemenu = glutCreateMenu(HandleToggleMenu);
	glutAddMenuEntry("Usage information (f1)",1);
	glutAddMenuEntry("Histogram (f2)",2);
	glutAddMenuEntry("Model information (f3)",3);
   glutAddMenuEntry("Bounding box (f4)",4);
	glutAddMenuEntry("Axes (f5)",5);
   glutAddMenuEntry("Grid (f6)",6);
   glutAddMenuEntry("Auto rotate (a)",7);
	glutAddMenuEntry("Auto ramp isolevel (b)",8);

   drawmenu = glutCreateMenu(HandleDrawMenu);
   glutAddMenuEntry("Points Above",1);
	glutAddMenuEntry("Points Below",2);
	glutAddMenuEntry("Lines Above",3);
	glutAddMenuEntry("Lines Below",4);
	glutAddMenuEntry("Isopoints",5);
   glutAddMenuEntry("Isosurface",6);
   glutAddMenuEntry("Volume rendering (Not implemented)",7);

   /* Set up the main menu */
   mainmenu = glutCreateMenu(HandleMainMenu);
	glutAddSubMenu("Data",datamenu);
   glutAddSubMenu("Drawing mode",drawmenu);
	glutAddSubMenu("Colour map",colourmenu);
	glutAddSubMenu("Point and line size",pointmenu);
   glutAddSubMenu("Camera",cameramenu);
   glutAddSubMenu("Grid resolution",resolmenu);
	glutAddSubMenu("Rendering options",rendermenu);
   glutAddSubMenu("Toggle",togglemenu);
	glutAddMenuEntry("Help (f1)",8);
   glutAddMenuEntry("Quit (q, ESC)",9);
   glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/* Handle the rendering menu */
void HandleRenderMenu(int whichone)
{
   switch (whichone) {
   case 1:
 	   drawwireframe = !drawwireframe;
      break;
   case 2:
		dosmooth = !dosmooth;
      break;
	case 3:
   	uselights = !uselights;
		break;
   case 4:
		if (polyorder == CLOCKWISE)
      	polyorder = ANTICLOCKWISE;
		else
			polyorder = CLOCKWISE;
      break;
	case 5:
		usespecular = !usespecular;
		break;
   }
	modeldirty = ENVDIRTY;
}

/* Handle the main menu */
void HandleMainMenu(int whichone)
{
   switch (whichone) {
   case 8:
      showusage = !showusage;
      break;
   case 9:
      exit(0);
      break;
   }
}

/* Handle the colour menu */
void HandleColourMenu(int whichone)
{
   int i,r,g,b;

   colourmap = whichone;
   if (colourmap == 100) {
      i = fl_show_colormap(2);
      fl_getmcolor(i,&r,&g,&b);
      constcolour.r = r / 255.0;
      constcolour.g = g / 255.0;
      constcolour.b = b / 255.0;
   }
	modeldirty = GEOMDIRTY;
}

/* Handle the resolution menu */
void HandleResolMenu(int whichone)
{
	grid.resolution = whichone;
	UpdateResolution(0);
}

/* Handle the point menu */
void HandlePointMenu(int whichone)
{
   pointlinesize = whichone;
   modeldirty = GEOMDIRTY;
}

/* Handle the light menu */
void HandleLightMenu(int whichone)
{
   nlights = whichone;
   modeldirty = ENVDIRTY;
}

/* Handle the drawing menu */
void HandleDrawMenu(int whichone)
{
   drawingmode = whichone;
	modeldirty = GEOMDIRTY;
}

/* Handle the toggle menu */
void HandleToggleMenu(int whichone)
{
   switch (whichone) {
   case 1:
		showusage = !showusage;
		break;
	case 2:
		showhistogram = !showhistogram;
		break;
	case 3:
		showinfo = !showinfo;
		break;
	case 4:
      showbbox = !showbbox;
      break;
	case 5:
		showaxes = !showaxes;
		break;
   case 6:
      showgrid = !showgrid;
      break;
   case 7:
      if (autospin.x != 0 || autospin.y != 0 || autospin.z != 0) {
         autospin.x = 0; autospin.y = 0; autospin.z = 0;
      } else {
         autospin.x = 1; autospin.y = 0; autospin.z = 0;
      }
      break;
	case 8:
		autorampiso = !autorampiso;
		break;
   }
}

/* Handle the camera menu */
void HandleCameraMenu(int whichone)
{
   CameraHome(whichone);
}

/* Handle the data menu */
void HandleDataMenu(int whichone)
{
	int i,j,k;
	double dx,dy,dz;
	int nx,ny,nz;
	long index,value;
	short int *tmp;

	switch (whichone) {
	case 1: /* Open / read a file */
		if (!ReadVolume("")) {
			grid.Nx = 50; grid.Ny = 50; grid.Nz = 50;
			grid.Dx = 1;  grid.Dy = 1;  grid.Dz = 1;
			MallocGrid(grid.Nx,grid.Ny,grid.Nz);
      	for (k=0;k<grid.Nz;k++) {
         	for (j=0;j<grid.Ny;j++) {
         	   for (i=0;i<grid.Nx;i++) {
         	      grid.data[index] = rand() % 1000;
         	      index++;
         	   }
         	}
      	}
		}
		break;
	case 2:
		SaveVolume("");
		return;
		break;
	case 3: /* Save as Geom */
		return;
		break;
	case 4: /* Save as PovRay */
		return;
		break;
	case 10: /* Create spherical data set */
		grid.Nx = 100; grid.Ny = 75; grid.Nz = 50;
		grid.Dx = 1; grid.Dy = 1; grid.Dz = 2;
		MallocGrid(grid.Nx,grid.Ny,grid.Nz);
		index = 0;
		for (k=0;k<grid.Nz;k++) {
			dz = 2 * (k - grid.Nz / 2) / (double)grid.Nz;
			for (j=0;j<grid.Ny;j++) {
				dy = 2 * (j - grid.Ny / 2) / (double)grid.Ny;
				for (i=0;i<grid.Nx;i++) {
					dx = 2 * (i - grid.Nx / 2) / (double)grid.Nx;
					grid.data[index] = 1000 * sqrt(dx*dx + dy*dy + dz*dz);
					index++;
				}
			}
		}
		break;
	case 11: /* Create ramp data set */
      grid.Nx = 75; grid.Ny = 75; grid.Nz = 75;
		grid.Dx = 1; grid.Dy = 1; grid.Dz = 1.5;
      MallocGrid(grid.Nx,grid.Ny,grid.Nz);
      index = 0;
      for (k=0;k<grid.Nz;k++) {
			dz = k / (double)grid.Nz;
         for (j=0;j<grid.Ny;j++) {
            dy = j / (double)grid.Ny;
            for (i=0;i<grid.Nx;i++) {
               dx = i / (double)grid.Nx;
               grid.data[index] = 1000 * (dx + dy + dz)/3;
               index++;
            }
         }
      }
		break;
	case 20: /* Subsample x-y */
		nx = grid.Nx / 2;
		ny = grid.Ny / 2;
		nz = grid.Nz;
		if ((tmp = malloc(nx*ny*nz*sizeof(int))) == NULL) {
			fprintf(stderr,"Failed to allocate temporary storage\n");
			break;
		}
		for (k=0;k<nz;k++) {
			for (i=0;i<nx;i++) {
				for (j=0;j<ny;j++) {
					index = k*grid.Nx*grid.Ny + 2*j*grid.Nx + 2*i;
					value = grid.data[index];
               index = k*grid.Nx*grid.Ny + (2*j+1)*grid.Nx + 2*i;
               value += grid.data[index];
               index = k*grid.Nx*grid.Ny + (2*j+1)*grid.Nx + 2*i + 1;
               value += grid.data[index];
               index = k*grid.Nx*grid.Ny + 2*j*grid.Nx + 2*i + 1;
               value += grid.data[index];
					tmp[k*nx*ny + j*nx + i] = value / 4;
				}
			}
		}
		MallocGrid(nx,ny,nz);
      for (i=0;i<nx*ny*nz;i++)
         grid.data[i] = tmp[i];
		grid.Nx = nx;
		grid.Ny = ny;
		grid.Dz /= 2;
		free(tmp);
		break;
   case 21: /* Subsample x-z */
      nx = grid.Nx / 2;
      ny = grid.Ny;
      nz = grid.Nz / 2;
      if ((tmp = malloc(nx*ny*nz*sizeof(int))) == NULL) {
         fprintf(stderr,"Failed to allocate temporary storage\n");
         break;
      }
      for (k=0;k<nz;k++) {
         for (i=0;i<nx;i++) {
            for (j=0;j<ny;j++) {
               index = 2*k*grid.Nx*grid.Ny + j*grid.Nx + 2*i;
               value = grid.data[index];
               index = 2*k*grid.Nx*grid.Ny + j*grid.Nx + 2*i + 1;
               value += grid.data[index];
               index = (2*k+1)*grid.Nx*grid.Ny + j*grid.Nx + 2*i + 1;
               value += grid.data[index];
               index = (2*k+1)*grid.Nx*grid.Ny + j*grid.Nx + 2*i;
               value += grid.data[index];
               tmp[k*nx*ny + j*nx + i] = value / 4;
            }
         }
      }
      MallocGrid(nx,ny,nz);
      for (i=0;i<nx*ny*nz;i++)
         grid.data[i] = tmp[i];
      grid.Nx = nx;
      grid.Nz = nz;
      grid.Dy /= 2;
      free(tmp);
      break;
   case 22: /* Subsample y-z */
      nx = grid.Nx;
      ny = grid.Ny / 2;
      nz = grid.Nz / 2;
      if ((tmp = malloc(nx*ny*nz*sizeof(int))) == NULL) {
         fprintf(stderr,"Failed to allocate temporary storage\n");
         break;
      }
      for (k=0;k<nz;k++) {
         for (i=0;i<nx;i++) {
            for (j=0;j<ny;j++) {
               index = 2*k*grid.Nx*grid.Ny + j*grid.Nx + i;
               value = grid.data[index];
               index = 2*k*grid.Nx*grid.Ny + (j+1)*grid.Nx + i;
               value += grid.data[index];
               index = (2*k+1)*grid.Nx*grid.Ny + (j+1)*grid.Nx + i;
               value += grid.data[index];
               index = (2*k+1)*grid.Nx*grid.Ny + j*grid.Nx + i;
               value += grid.data[index];
               tmp[k*nx*ny + j*nx + i] = value / 4;
            }
         }
      }
      MallocGrid(nx,ny,nz);
      for (i=0;i<nx*ny*nz;i++)
         grid.data[i] = tmp[i];
      grid.Ny = ny;
      grid.Nz = nz;
      grid.Dx /= 2;
      free(tmp);
      break;
   case 23: /* Subsample x-y-z */
      nx = grid.Nx / 2;
      ny = grid.Ny / 2;
      nz = grid.Nz / 2;
      if ((tmp = malloc(nx*ny*nz*sizeof(int))) == NULL) {
         fprintf(stderr,"Failed to allocate temporary storage\n");
         break;
      }
      for (k=0;k<nz;k++) {
         for (i=0;i<nx;i++) {
            for (j=0;j<ny;j++) {
               index = 2*k*grid.Nx*grid.Ny + 2*j*grid.Nx + 2*i;
               value = grid.data[index];
               index = 2*k*grid.Nx*grid.Ny + 2*j*grid.Nx + 2*i + 1;
               value += grid.data[index];
               index = (2*k+1)*grid.Nx*grid.Ny + 2*j*grid.Nx + 2*i + 1;
               value += grid.data[index];
               index = (2*k+1)*grid.Nx*grid.Ny + 2*j*grid.Nx + 2*i;
               value += grid.data[index];

               index = 2*k*grid.Nx*grid.Ny + (2*j+1)*grid.Nx + 2*i;
               value = grid.data[index];
               index = 2*k*grid.Nx*grid.Ny + (2*j+1)*grid.Nx + 2*i + 1;
               value += grid.data[index];
               index = (2*k+1)*grid.Nx*grid.Ny + (2*j+1)*grid.Nx + 2*i + 1;
               value += grid.data[index];
               index = (2*k+1)*grid.Nx*grid.Ny + (2*j+1)*grid.Nx + 2*i;
               value += grid.data[index];

               tmp[k*nx*ny + j*nx + i] = value / 8;
            }
         }
      }
      MallocGrid(nx,ny,nz);
      for (i=0;i<nx*ny*nz;i++)
         grid.data[i] = tmp[i];
      grid.Nx = nx;
		grid.Ny = ny;
      grid.Nz = nz;
      free(tmp);
      break;
	case 30:
      for (i=0;i<grid.Nx*grid.Ny*grid.Nz;i++) 
			grid.data[i] = grid.themax - grid.data[i];
		break;
	}
	if (debug)
		fprintf(stderr,"Dataset created\n");

	CalcBounds();
	UpdateResolution(0);
	CameraHome(0);
}


