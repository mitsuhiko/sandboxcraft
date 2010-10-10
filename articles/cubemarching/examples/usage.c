#include "glvolume.h"

/*
   Display the program usage information
*/
void GiveUsage(char *cmd)
{
   fprintf(stderr,"Usage: %s [command line options]\n",cmd);
   fprintf(stderr,"       -h this text\n");
   fprintf(stderr,"       -f full screen mode\n");
   fprintf(stderr,"       -s stereo mode\n");
	fprintf(stderr,"      -as start in rotate mode\n");
   fprintf(stderr,"       -d debug mode\n");
   exit(-1);
}

/*
	Display usage information
*/
void DrawUsage(int xstart,int ystart)
{
   int y;

   y = ystart - 13;

   /* Usage information */
   DrawGLText(xstart,y-=13,"Keyboard / mouse");
   DrawGLText(xstart,y-=13,"       arrows   rotate left/right/up/down");
   DrawGLText(xstart,y-=13,"   left mouse   rotate left/right");
   DrawGLText(xstart,y-=13," middle mouse   roll clockwise/anticlockwise");
   DrawGLText(xstart,y-=13,"  right mouse   activates menus");
   DrawGLText(xstart,y-=13,"          i,k   translate up or down");
   DrawGLText(xstart,y-=13,"          j,l   translate left or right");
   DrawGLText(xstart,y-=13,"          [,]   roll clockwise or anticlockwise");
   DrawGLText(xstart,y-=13,"          -,+   move forward or backward");
	DrawGLText(xstart,y-=13,"          <,>   decrement, increment isolevel");
   DrawGLText(xstart,y-=13,"            w   write a frame as a TIFF file");
   DrawGLText(xstart,y-=13,"            r   toggle movie recording");
   DrawGLText(xstart,y-=13,"           f1   toggle this help information");
   DrawGLText(xstart,y-=13,"           f2   toggle histogram display");
	DrawGLText(xstart,y-=13,"           f3   toggle general information");
	DrawGLText(xstart,y-=13,"           f4   toggle bounding box");
	DrawGLText(xstart,y-=13,"           f5   toggle axes");
	DrawGLText(xstart,y-=13,"           f6   toggle grid");
   DrawGLText(xstart,y-=13,"            q   quit (ESC)");
}


