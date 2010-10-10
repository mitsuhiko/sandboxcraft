#include "glvolume.h"

extern VOLUME grid;
extern int debug;
extern double framerate;
extern int modeldirty;
extern COLOUR constcolour;  /* Need to change the way this is handled ! */
extern int failsafe;

void DrawTheAxes(void)
{
	XYZ v[500];
	int i,vlen = 0,nmin;
	XYZ right = {1.0,0.0,0.0},up = {0.0,1.0,0.0},p = {0.0,0.0,0.0};
	double scale = 2,border = 3;

	/* Text scale factor */
	nmin = MIN(grid.Nx,grid.Ny);
	nmin = MIN(nmin,grid.Nz);
	scale = nmin / 20; 

	glLineWidth(2.0);

	glBegin(GL_LINES);
	glColor4f(1.0,0.0,0.0,1.0);
	glVertex3f(-border,-border,-border);
	glVertex3f(grid.Dx*grid.Nx+border,-border,-border);
	glColor4f(0.0,1.0,0.0,1.0);
   glVertex3f(-border,-border,-border);
   glVertex3f(-border,grid.Dy*grid.Ny+border,-border);
	glColor4f(0.0,0.0,1.0,1.0);
   glVertex3f(-border,-border,-border);
   glVertex3f(-border,-border,grid.Dz*grid.Nz+border);
	glEnd();

	glBegin(GL_LINES);
	glColor4f(1.0,0.0,0.0,1.0);
	CreateLabelVector("x",p,right,up,v,&vlen);
	for (i=0;i<vlen;i+=2) {
		glVertex3f(
			scale*v[i].x+grid.Dx*grid.Nx+2*border,
			scale*v[i].y-border,
			scale*v[i].z-border);
		glVertex3f(
			scale*v[i+1].x+grid.Dx*grid.Nx+2*border,
			scale*v[i+1].y-border,
			scale*v[i+1].z-border);
	}
   glColor4f(0.0,1.0,0.0,1.0);
   CreateLabelVector("y",p,right,up,v,&vlen);
   for (i=0;i<vlen;i+=2) {
      glVertex3f(
         scale*v[i].x-border,
         scale*v[i].y+grid.Dx*grid.Ny+2*border,
         scale*v[i].z-border);
      glVertex3f(
         scale*v[i+1].x-border,
         scale*v[i+1].y+grid.Dx*grid.Ny+2*border,
         scale*v[i+1].z-border);
   }
   glColor4f(0.0,0.0,1.0,1.0);
   CreateLabelVector("z",p,right,up,v,&vlen);
   for (i=0;i<vlen;i+=2) {
      glVertex3f(
         scale*v[i].x-border,
         scale*v[i].y-border,
         scale*v[i].z+grid.Dz*grid.Nz+2*border);
      glVertex3f(
         scale*v[i+1].x-border,
         scale*v[i+1].y-border,
         scale*v[i+1].z+grid.Dz*grid.Nz+2*border);
   }
	glEnd();
	glLineWidth(1.0);
}

/*
   Create a regular wireframe grid along the 3 axis planes
	at the current grid resolutions
*/
void DrawGrid(void)
{
   int i,j,k;

   glBegin(GL_LINES);

	/* Draw the x-y plane */
   for (i=0;i<grid.Nx+grid.Rx;i+=grid.Rx) {
		SendGridVertex(i,0,0);
		SendGridVertex(i,grid.Ny-1,0);
   }
   for (j=0;j<grid.Ny+grid.Ry;j+=grid.Ry) {
		SendGridVertex(0,j,0);
		SendGridVertex(grid.Nx-1,j,0);
   }

   /* Draw the x-z plane */
   for (i=0;i<grid.Nx+grid.Rx;i+=grid.Rx) {
		SendGridVertex(i,0,0);
		SendGridVertex(i,0,grid.Nz-1);
   }
   for (k=0;k<grid.Nz+grid.Rz;k+=grid.Rz) {
      SendGridVertex(0,0,k);
      SendGridVertex(grid.Nx-1,0,k);
   }

   /* Draw the y-z plane */
   for (j=0;j<grid.Ny+grid.Ry;j+=grid.Ry) {
      SendGridVertex(0,j,0);
      SendGridVertex(0,j,grid.Nz-1);
   }
   for (k=0;k<grid.Nz+grid.Rz;k+=grid.Rz) {
      SendGridVertex(0,0,k);
      SendGridVertex(0,grid.Ny-1,k);
   }

   glEnd();
}

/*
	Draw a histogram
*/
void DrawHistogram(int cmap)
{
	int i,j,dgrid;
	int offset = 10,border = 5;
	int height,length;
	int themax = 0;
	double val,dd;
	COLOUR colour;

	length = HISTLENGTH;
	height = HISTLENGTH / 2;

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(0,glutGet(GLUT_WINDOW_WIDTH),0,glutGet(GLUT_WINDOW_HEIGHT),-1,1);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	/* Transparent patch */
	glColor4f(1.0,1.0,1.0,0.5);
	glBegin(GL_QUADS);
   glVertex3i(offset,offset,0);
   glVertex3i(offset,offset + height + 3*border,0);
   glVertex3i(offset + length + 2*border,offset + height + 3*border,0);
   glVertex3i(offset + length + 2*border,offset,0);
	glEnd();

	/* Graph */
	for (i=0;i<length;i++)
		themax = MAX(themax,grid.histogram[i]);
	dd = grid.themax - grid.themin;
	if (themax > 0) {
		glColor4f(0.0,1.0,0.0,1.0);
		glBegin(GL_LINES);
		for (i=0;i<length;i++) {
			j = rint(grid.histogram[i] * height / (double)themax);
		   glVertex3i(offset+border+i,offset+2*border,0);
   		glVertex3i(offset+border+i,offset+2*border+j,0);
		}

		/* Colour ramp */
      for (i=0;i<length;i++) {
			val = grid.themin + i * dd / length;
			if (cmap == CONSTANTCOLOUR)
				colour = constcolour;
			else
         	colour = GetColour(val,(double)grid.themin,(double)grid.themax,cmap);
			glColor4f(colour.r,colour.g,colour.b,1.0);
         glVertex3i(offset+border+i,offset+border,0);
         glVertex3i(offset+border+i,offset+2*border,0);
      }
		glEnd();
	}

   /* Draw the isolevel */
   dgrid = grid.themax - grid.themin;
   if (dgrid > 0) {
      glColor4f(0.0,0.0,1.0,1.0);
      glBegin(GL_LINES);
      i = length * (grid.isolevel - grid.themin) / dgrid;
      glVertex3i(offset + border + i,offset + 2*border,0);
      glVertex3i(offset + border + i,offset + height + 2*border,0);
      glEnd();
   }

   /* Axes */
   glColor4f(1.0,0.0,0.0,1.0);
   glBegin(GL_LINE_STRIP);
   glVertex3i(offset + border - 1,offset + height + 2*border,0);
   glVertex3i(offset + border - 1,offset + 2*border - 1,0);
   glVertex3i(offset + border + length,offset + 2*border - 1,0);
   glEnd();

	glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
}

/*
	Handle clicks in the histogram box
*/
void HistogramClick(int x,int y)
{
   int offset = 10,border = 5;
	int length,height;
	double d;

   length = HISTLENGTH;
   height = HISTLENGTH / 2;

	/* Are we in the region */
	if (x < offset+border || x > offset+length+border)
		return;
	if (y < offset || y > offset+height+2*border)
		return;

	/* Calculate the new isolevel */
	d = grid.themax - grid.themin;
	grid.isolevel = (x - (offset+border)) * d / length + grid.themin;
	modeldirty = GEOMDIRTY;

	if (debug)
		fprintf(stderr,"Clicked in histograph region\n");
}

/*
	Draw the points that lie either above or below the isolevel
*/
void DrawPoints(int mode,int size,int cmap)
{
	int index,i,j,k;
	int value;
	long NxNy;
	COLOUR colour;
	int npoints = 0;

   /* Set up the colour for constant colouring */
   if (cmap == CONSTANTCOLOUR)
      glColor4f(constcolour.r,constcolour.g,constcolour.b,1.0);

	NxNy = grid.Nx * grid.Ny;

	glPointSize(size);
	glBegin(GL_POINTS);

   for (k=0;k<grid.Nz+grid.Rz;k+=grid.Rz) {
      if (k >= grid.Nz)
          k = grid.Nz - 1;

      for (j=0;j<grid.Ny+grid.Ry;j+=grid.Ry) {
        	if (j >= grid.Ny)
           	j = grid.Ny - 1;

			for (i=0;i<grid.Nx+grid.Rx;i+=grid.Rx) {
				if (i >= grid.Nx)
					i = grid.Nx - 1;
		
				index = k*NxNy + j*grid.Nx + i;
				value = grid.data[index];

				/* Should we draw the point */
     			if (mode == POINTSABOVE && value <= grid.isolevel)
        			goto skip;;
     			if (mode == POINTSBELOW && value >= grid.isolevel)
        			goto skip;

				/* Choose the colour */
     			if (cmap != CONSTANTCOLOUR) 
     				colour = GetColour((double)value,
        				(double)grid.themin,(double)grid.themax,cmap);

				/* Draw it */
				glColor4f(colour.r,colour.g,colour.b,1.0);
				SendGridVertex(i,j,k);
				
				skip:
				if (i >= grid.Nx-1)
					break;
			}

			if (j >= grid.Ny-1)
				break;
		}

		/* Deal with the number of points */
		npoints++;
		if (npoints > MAXPOINTS) 
			break;

		if (k >= grid.Nz-1)
			break;
	}
   glEnd();
	glPointSize(1.0);

	/* Adjust the resolution */
	if (npoints > MAXPOINTS) 
		UpdateResolution(1);
	if (npoints < MAXPOINTS/5)
		UpdateResolution(-1);

	if (debug)
		fprintf(stderr,"%d points\n",npoints);
}

/*
   Draw the lines either above or below the isosurface
*/
void DrawLines(int mode,int size,int cmap)
{
   int i,j,k,ii,jj,kk;
	int value;
	int index;
	long NxNy;
   COLOUR colour1,colour2;
	int nlines = 0;

	NxNy = grid.Nx * grid.Ny;

	glLineWidth(size); 
	glBegin(GL_LINES);

	/* Constant colour? */
	if (cmap == CONSTANTCOLOUR) {
		colour1 = constcolour;
		colour2 = constcolour;
	}
	
   for (k=0;k<grid.Nz+grid.Rz;k+=grid.Rz) {
      if (k >= grid.Nz)
         k = grid.Nz - 1;
      for (j=0;j<grid.Ny+grid.Ry;j+=grid.Ry) {
         if (j >= grid.Ny)
            j = grid.Ny - 1;
         for (i=0;i<grid.Nx-1;i+=grid.Rx) {
            index = k*NxNy + j*grid.Nx + i;
            value = grid.data[index];

            if (mode == LINESABOVE && value <= grid.isolevel)
               continue;
            if (mode == LINESBELOW && value >= grid.isolevel)
               continue;
            if (cmap != CONSTANTCOLOUR)
               colour1 = GetColour((double)value,
                  (double)grid.themin,(double)grid.themax,cmap);

            if ((ii = i + grid.Rx) >= grid.Nx)
               ii = grid.Nx - 1;
            index = k*NxNy + j*grid.Nx + ii;
            value = grid.data[index];

            if (mode == LINESABOVE && value <= grid.isolevel)
               continue;
            if (mode == LINESBELOW && value >= grid.isolevel)
               continue;
            if (cmap != CONSTANTCOLOUR) 
               colour2 = GetColour((double)value,
                  (double)grid.themin,(double)grid.themax,cmap);

            glColor4f(colour1.r,colour1.g,colour1.b,1.0);
				SendGridVertex(i,j,k);
            glColor4f(colour2.r,colour2.g,colour2.b,1.0);
				SendGridVertex(ii,j,k);
	
            nlines++;
            if (nlines > MAXLINES)
               break;
         }
         if (j >= grid.Ny-1)
            break;
         if (nlines > MAXLINES)
            break;
      }
      if (k >= grid.Nz-1)
         break;
      if (nlines > MAXLINES)
         break;
   }

   for (k=0;k<grid.Nz+grid.Rz;k+=grid.Rz) {
		if (k >= grid.Nz)
			k = grid.Nz - 1;
      for (i=0;i<grid.Nx+grid.Rx;i+=grid.Rx) {
			if (i >= grid.Nx)
				i = grid.Nx - 1;
         for (j=0;j<grid.Ny-1;j+=grid.Ry) {
            index = k*NxNy + j*grid.Nx + i;
				value = grid.data[index];

            if (mode == LINESABOVE && value <= grid.isolevel)
               continue;
            if (mode == LINESBELOW && value >= grid.isolevel)
               continue;
            if (cmap != CONSTANTCOLOUR) 
               colour1 = GetColour((double)value,
                  (double)grid.themin,(double)grid.themax,cmap);

				if ((jj = j + grid.Ry) >= grid.Ny)
					jj = grid.Ny - 1;
            index = k*NxNy + jj*grid.Nx + i;
				value = grid.data[index];

            if (mode == LINESABOVE && value <= grid.isolevel)
               continue;
            if (mode == LINESBELOW && value >= grid.isolevel)
               continue;
            if (cmap != CONSTANTCOLOUR) 
               colour2 = GetColour((double)value,
                  (double)grid.themin,(double)grid.themax,cmap);

            glColor4f(colour1.r,colour1.g,colour1.b,1.0);
				SendGridVertex(i,j,k);
            glColor4f(colour2.r,colour2.g,colour2.b,1.0);
				SendGridVertex(i,jj,k);

				nlines++;
         	if (nlines > MAXLINES)
            	break;
         }
         if (i >= grid.Nx-1)
            break;
         if (nlines > MAXLINES)
            break;
      }
		if (k >= grid.Nz-1)
			break;
      if (nlines > MAXLINES) 
         break;
   }

   for (i=0;i<grid.Nx+grid.Rx;i+=grid.Rx) {
		if (i >= grid.Nx)
			i = grid.Nx - 1;
      for (j=0;j<grid.Ny+grid.Ry;j+=grid.Ry) {
         if (j >= grid.Ny)
            j = grid.Ny - 1;
         for (k=0;k<grid.Nz-1;k+=grid.Rz) {
            index = k*NxNy + j*grid.Nx + i;
				value = grid.data[index];

            if (mode == LINESABOVE && value <= grid.isolevel)
               continue;
            if (mode == LINESBELOW && value >= grid.isolevel)
               continue;
            if (cmap != CONSTANTCOLOUR) 
               colour1 = GetColour((double)value,
                  (double)grid.themin,(double)grid.themax,cmap);

				if ((kk = k + grid.Rz) >= grid.Nz)
					kk = grid.Nz - 1;
            index = kk*NxNy + j*grid.Nx + i;
				value = grid.data[index];

            if (mode == LINESABOVE && value <= grid.isolevel)
               continue;
            if (mode == LINESBELOW && value >= grid.isolevel)
               continue;
            if (cmap != CONSTANTCOLOUR) 
               colour2 = GetColour((double)value,
                  (double)grid.themin,(double)grid.themax,cmap);

            glColor4f(colour1.r,colour1.g,colour1.b,1.0);
				SendGridVertex(i,j,k);
            glColor4f(colour2.r,colour2.g,colour2.b,1.0);
				SendGridVertex(i,j,kk);

				nlines++;
         	if (nlines > MAXLINES)
            	break;
         }
         if (j >= grid.Ny-1)
            break;
     		if (nlines > MAXLINES)
         	break;
      }
		if (i >= grid.Nx-1)
			break;
      if (nlines > MAXLINES) 
         break;
   }

	glEnd();
	glLineWidth(1.0);

   /* Adjust the resolution */
   if (nlines > MAXLINES)
      UpdateResolution(1);
	if (nlines < MAXLINES/5)
		UpdateResolution(-1);

	if (debug)
		fprintf(stderr,"%d lines\n",nlines);
}

/*
	Draw the isosurface as points
	Find zero crossings and draw points at them
*/
void DrawIsopoints(int size,int cmap)
{
	int i,j,k,ip1,jp1,kp1;
	int sign0,sign1;
	int index,NxNy;
	COLOUR colour;
	int npoints = 0;

   /* Set up the colour for constant colouring */
   if (cmap == CONSTANTCOLOUR) {
      glColor4f(constcolour.r,constcolour.g,constcolour.b,1.0);
	} else {
		colour = GetColour((double)grid.isolevel,
			(double)grid.themin,(double)grid.themax,cmap);
		glColor4f(colour.r,colour.g,colour.b,1.0);
	}

	NxNy = grid.Nx * grid.Ny;
	glPointSize(size);
	glBegin(GL_POINTS);

	/* Find the zero crossings between lines running along j */
	for (k=0;k<grid.Nz+grid.Rz;k+=grid.Rz) {
		if (k >= grid.Nz)
			k = grid.Nz - 1;
		for (i=0;i<grid.Nx+grid.Rx;i+=grid.Rx) {
			if (i >= grid.Nx)
				i = grid.Nx - 1;
			index = k*NxNy + i;
			sign0 = SIGN(grid.data[index] - grid.isolevel);
			for (j=1;j<grid.Ny;j++) {
				index += grid.Nx; 
				sign1 = SIGN(grid.data[index] - grid.isolevel);
				if (sign1 != sign0) {
					sign0 = sign1;
					SendGridVertex(i,j,k);
					npoints++;
				}
			}
			if (i >= grid.Nx-1)
				break;
			if (npoints > MAXPOINTS)
				break;
		}
		if (k >= grid.Nz-1)
			break;
      if (npoints > MAXPOINTS)
         break;
	}

   /* Find the zero crossings between lines running along i */
   for (k=0;k<grid.Nz+grid.Rz;k+=grid.Rz) {
      if (k >= grid.Nz)
         k = grid.Nz - 1;
      for (j=0;j<grid.Ny+grid.Ry;j+=grid.Ry) {
			if (j >= grid.Ny)
				j = grid.Ny - 1;
         index = k*NxNy + j*grid.Nx;
         sign0 = SIGN(grid.data[index] - grid.isolevel);
         for (i=1;i<grid.Nx;i++) {
            index++; 
            sign1 = SIGN(grid.data[index] - grid.isolevel);
            if (sign1 != sign0) {
               sign0 = sign1;
					SendGridVertex(i,j,k);
					npoints++;
            }
         }
         if (j >= grid.Ny-1)
            break;
         if (npoints > MAXPOINTS)
            break;
      }
      if (k >= grid.Nz-1)
         break;
      if (npoints > MAXPOINTS)
         break;
   }

   /* Find the zero crossings between lines running along k */
   for (i=0;i<grid.Nx+grid.Rx;i+=grid.Rx) {
      if (i >= grid.Nx)
         i = grid.Nx - 1;
      for (j=0;j<grid.Ny+grid.Ry;j+=grid.Ry) {
         if (j >= grid.Ny)
            j = grid.Ny - 1;
         index = j*grid.Nx + i;
         sign0 = SIGN(grid.data[index] - grid.isolevel);
         for (k=1;k<grid.Nz;k++) {
            index += NxNy; 
            sign1 = SIGN(grid.data[index] - grid.isolevel);
            if (sign1 != sign0) {
               sign0 = sign1;
					SendGridVertex(i,j,k);
					npoints++;
            }
         }
         if (j >= grid.Ny-1)
            break;
         if (npoints > MAXPOINTS)
            break;
      }
      if (i >= grid.Nx-1)
         break;
      if (npoints > MAXPOINTS)
         break;
   }
   glEnd();
   glPointSize(1.0);

	if (npoints > MAXPOINTS)
		UpdateResolution(1);
	if (npoints < MAXPOINTS/5)
		UpdateResolution(-1);

	if (debug)
		fprintf(stderr,"%d points\n",npoints);
}

/*
   Display useful information
*/
void DrawInfo(int xstart,int ystart)
{
   int y;
   char s[256];

   y = ystart + 5;

	sprintf(s,"   Frame rate = %g\n",framerate);
	DrawGLText(xstart,y+=13,s);

   sprintf(s,"   Isolevel = %d\n",grid.isolevel);
   DrawGLText(xstart,y+=13,s);

   sprintf(s,"   Grid range = %d -> %d",grid.themin,grid.themax);
   DrawGLText(xstart,y+=13,s);

   sprintf(s,"   Rx = %d, Ry = %d, Rz = %d",grid.Rx,grid.Ry,grid.Rz);
   DrawGLText(xstart,y+=13,s);

   sprintf(s,"   Dx = %g, Dy = %g, Dz = %g",grid.Dx,grid.Dy,grid.Dz);
   DrawGLText(xstart,y+=13,s);

   sprintf(s,"   Nx = %d, Ny = %d, Nz = %d",grid.Nx,grid.Ny,grid.Nz);
   DrawGLText(xstart,y+=13,s);

   DrawGLText(xstart,y+=13,"Information");
}

/*
   Draw the isosurface facets
*/
void DrawIsoSurface(int cmap)
{
	int i,j,k,ii,jj,kk;
	int t,ntri,index;
	int npolygons = 0;
	GRIDCELL cell;
   TRIANGLE tri[10];
	long NxNy;
	COLOUR colour;
	XYZ normal;

   /* Set up the colour for constant colouring */
   if (cmap == CONSTANTCOLOUR) {
      glColor4f(constcolour.r,constcolour.g,constcolour.b,1.0);
   } else {
      colour = GetColour((double)grid.isolevel,
         (double)grid.themin,(double)grid.themax,cmap);
      glColor4f(colour.r,colour.g,colour.b,1.0);
   }

   NxNy = grid.Nx * grid.Ny;
   glBegin(GL_TRIANGLES);

	for (i=0;i<grid.Nx-1;i+=grid.Rx) {
		if ((ii = i + grid.Rx) >= grid.Nx)
			ii = grid.Nx - 1;

		for (j=0;j<grid.Ny-1;j+=grid.Ry) {
			if ((jj = j + grid.Ry) >= grid.Ny)
				jj = grid.Ny - 1;

			for (k=0;k<grid.Nz-1;k+=grid.Rz) {
				if ((kk = k + grid.Rz) >= grid.Nz)
					kk = grid.Nz - 1;

            cell.p[0].x = i*grid.Dx;
            cell.p[0].y = j*grid.Dy;
            cell.p[0].z = k*grid.Dz;
				index = k*NxNy + j*grid.Nx + i;
            cell.val[0] = grid.data[index];

            cell.p[1].x = ii*grid.Dx;
            cell.p[1].y = j*grid.Dy;
            cell.p[1].z = k*grid.Dz;
				index = k*NxNy + j*grid.Nx + ii;
            cell.val[1] = grid.data[index];

            cell.p[2].x = ii*grid.Dx;
            cell.p[2].y = j*grid.Dy;
            cell.p[2].z = kk*grid.Dz;
				index = kk*NxNy + j*grid.Nx + ii;
            cell.val[2] = grid.data[index];

            cell.p[3].x = i*grid.Dx;
            cell.p[3].y = j*grid.Dy;
            cell.p[3].z = kk*grid.Dz;
				index = kk*NxNy + j*grid.Nx + i;
            cell.val[3] = grid.data[index];

            cell.p[4].x = i*grid.Dx;
            cell.p[4].y = jj*grid.Dy;
            cell.p[4].z = k*grid.Dz;
				index = k*NxNy + jj*grid.Nx + i;
            cell.val[4] = grid.data[index];

            cell.p[5].x = ii*grid.Dx;
            cell.p[5].y = jj*grid.Dy;
            cell.p[5].z = k*grid.Dz;
				index = k*NxNy + jj*grid.Nx + ii;
            cell.val[5] = grid.data[index];

            cell.p[6].x = ii*grid.Dx;
            cell.p[6].y = jj*grid.Dy;
            cell.p[6].z = kk*grid.Dz;
				index = kk*NxNy + jj*grid.Nx + ii;
            cell.val[6] = grid.data[index];

            cell.p[7].x = i*grid.Dx;
            cell.p[7].y = jj*grid.Dy;
            cell.p[7].z = kk*grid.Dz;
				index = kk*NxNy + jj*grid.Nx + i;
            cell.val[7] = grid.data[index];

				ntri = PolygoniseCube(cell,grid.isolevel,tri);
				npolygons += ntri;
				for (t=0;t<ntri;t++) {
					normal = CalcNormal(tri[t].p[0],tri[t].p[1],tri[t].p[2]);
					glNormal3f(normal.x,normal.y,normal.z);
               glVertex3f(tri[t].p[0].x,tri[t].p[0].y,tri[t].p[0].z);
               glVertex3f(tri[t].p[1].x,tri[t].p[1].y,tri[t].p[1].z);
               glVertex3f(tri[t].p[2].x,tri[t].p[2].y,tri[t].p[2].z);
				}

				if (failsafe && npolygons > MAXPOLYGON)
					break;
			} /* k */
         if (failsafe && npolygons > MAXPOLYGON)
            break;
		} /* j */
      if (failsafe && npolygons > MAXPOLYGON)
			break;
	} /* i */
	glEnd();

   if (failsafe && npolygons > MAXPOLYGON)
      UpdateResolution(1);
   if (failsafe && npolygons < MAXPOLYGON/5)
      UpdateResolution(-1);
}

/*
	Estimate the normal for each point on the grid
*/
XYZ CalcGridNormal(int i,int j,int k)
{
	int im1,ip1,jp1,jm1,kp1,km1;
	int index1,index2,index;
	long NxNy;
	XYZ n = {0.0,0.0,0.0};

	im1 = i - 1;
	ip1 = i + 1;
	jm1 = j - 1;
	jp1 = j + 1;
	km1 = k - 1;
	kp1 = k + 1;

	NxNy = grid.Nx * grid.Ny;
	index = k*NxNy + j*grid.Ny + i;

	if (im1 > 0 && ip1 < grid.Nx) {
		index1 = index - 1;
		index2 = index + 1;
		n.x = (grid.data[index1] - grid.data[index2]) / 2.0;
	} else if (im1 > 0) {
      index1 = index - 1;
      n.x = grid.data[index1] - grid.data[index];
	} else {
      index2 = index + 1;
      n.x = grid.data[index] - grid.data[index2];
	}

   if (jm1 > 0 && jp1 < grid.Ny) {
      index1 = index - grid.Ny;
      index2 = index + grid.Ny;
      n.y = (grid.data[index1] - grid.data[index2]) / 2.0;
   } else if (jm1 > 0) {
      index1 = index - grid.Ny;
      n.y = grid.data[index1] - grid.data[index];
   } else {
      index2 = index + grid.Ny;
      n.y = grid.data[index] - grid.data[index2];
   }

   if (km1 > 0 && kp1 < grid.Nz) {
      index1 = index - NxNy;
      index2 = index + NxNy;
      n.z = (grid.data[index1] - grid.data[index2]) / 2.0;
   } else if (km1 > 0) {
      index1 = index - NxNy;
      n.z = grid.data[index1] - grid.data[index];
   } else {
      index2 = index + NxNy;
      n.z = grid.data[index] - grid.data[index2];
   }

	return(n);
}

void SendGridVertex(int i,int j,int k)
{
	if (i < 0)
		i = 0;
	if (i >= grid.Nx)
		i = grid.Nx-1;
   if (j < 0)
      j = 0;
   if (j >= grid.Ny)
      j = grid.Ny-1;
   if (k < 0)
      k = 0;
   if (k >= grid.Nz)
      k = grid.Nz-1;
	glVertex3f(i*grid.Dx,j*grid.Dy,k*grid.Dz);
}

