/*!  
** 
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as 
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.  
** It teaches strong moral principles, as well as leadership skills and 
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
**
**
** The MIT license:
**
** Permission is hereby granted, free of charge, to any person obtaining a copy 
** of this software and associated documentation files (the "Software"), to deal 
** in the Software without restriction, including without limitation the rights 
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
** copies of the Software, and to permit persons to whom the Software is furnished 
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all 
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#include <assert.h>

#include "frustum.h"


enum PlaneBit
{
	PLANE_X = (1<<0),
	PLANE_Y = (1<<1),
	PLANE_Z = (1<<2),
};


class FrustumMatrix
{
public:
	float mElement[4][4];
};

class FrustumPlane
{
public:

	void ComputeIndex(void)
	{
		int index = 0;

		if ( N[0] >= 0.0f )
		{
			index|=PLANE_X;
		}

		if ( N[1] >= 0.0f )
		{
			index|=PLANE_Y;
		}

		if ( N[2] >= 0.0f )
		{
			index|=PLANE_Z;
		}

		switch ( index )
		{
			// x < | y < | z <
			case 0:

				mIndex[0] = 3+0;
				mIndex[1] = 0+0;

				mIndex[2] = 3+1;
				mIndex[3] = 0+1;

				mIndex[4] = 3+2;
				mIndex[5] = 0+2;

				break;
			// x >= | y < |  z <
			case 1:

				mIndex[0]    = 0+0;
				mIndex[1]    = 3+0;

				mIndex[2]    = 3+1;
				mIndex[3]    = 0+1;

				mIndex[4]    = 3+2;
				mIndex[5]    = 0+2;

				break;
			// x < | y > | z <
			case 2:

				mIndex[0]    = 3+0;
				mIndex[1]    = 0+0;

				mIndex[2]    = 0+1;
				mIndex[3]    = 3+1;

				mIndex[4]    = 3+2;
				mIndex[5]    = 0+2;

				break;
			// x > | y > | z <
			case 3:

				mIndex[0]    = 0+0;
				mIndex[1]    = 3+0;

				mIndex[2]    = 0+1;
				mIndex[3]    = 3+1;

				mIndex[4]    = 3+2;
				mIndex[5]    = 0+2;

				break;
			// x < | y < | z >
			case 4:

				mIndex[0]    = 3+0;
				mIndex[1]    = 0+0;

				mIndex[2]    = 3+1;
				mIndex[3]    = 0+1;

				mIndex[4]    = 0+2;
				mIndex[5]    = 3+2;

				break;
			// x > | y < | z >
			case 5:

				mIndex[0]    = 0+0;
				mIndex[1]    = 3+0;

				mIndex[2]    = 3+1;
				mIndex[3]    = 0+1;

				mIndex[4]    = 0+2;
				mIndex[5]    = 3+2;

				break;
			// x < | y > | z >
			case 6:

				mIndex[0]    = 3+0;
				mIndex[1]    = 0+0;

				mIndex[2]    = 0+1;
				mIndex[3]    = 3+1;

				mIndex[4]    = 0+2;
				mIndex[5]    = 3+2;

				break;
			// x > | y > | z >
			case 7:
				mIndex[0]  = 0+0;
				mIndex[1]  = 3+0;

				mIndex[2]  = 0+1;
				mIndex[3]  = 3+1;

				mIndex[4]  = 0+2;
				mIndex[5]  = 3+2;
				break;
		}

	}

	inline void ComputeExtremes(const float *source,float *minExtreme,float *maxExtreme) const
	{
		const int *idx = mIndex;

		minExtreme[0] = source[ *idx++ ];
		maxExtreme[0] = source[ *idx++ ];

		minExtreme[1] = source[ *idx++ ];
		maxExtreme[1] = source[ *idx++ ];

		minExtreme[2] = source[ *idx++ ];
		maxExtreme[2] = source[ *idx   ];
	}

	int             mIndex[6];
	float           N[3];
	float           D;
};


Frustum::Frustum(void)
{
	mViewProjectionMatrix = 0;
	m_frustumPlanes = new FrustumPlane[6];
}

Frustum::~Frustum(void)
{
	delete []m_frustumPlanes;
}
//** Plane Extraction method by Klaus 'Niki' Hartman
void Frustum::Set(const float *viewproj)
{
	mViewProjectionMatrix = viewproj;
	const FrustumMatrix &vp = *(const FrustumMatrix *) viewproj;
	// Left clipping plane
	m_frustumPlanes[0].N[0] = -(vp.mElement[0][3] + vp.mElement[0][0]);
	m_frustumPlanes[0].N[1] = -(vp.mElement[1][3] + vp.mElement[1][0]);
	m_frustumPlanes[0].N[2] = -(vp.mElement[2][3] + vp.mElement[2][0]);
	m_frustumPlanes[0].D   = -(vp.mElement[3][3] + vp.mElement[3][0]);
	m_frustumPlanes[0].ComputeIndex();

	// Right clipping plane
	m_frustumPlanes[1].N[0] = -(vp.mElement[0][3] - vp.mElement[0][0]);
	m_frustumPlanes[1].N[1] = -(vp.mElement[1][3] - vp.mElement[1][0]);
	m_frustumPlanes[1].N[2] = -(vp.mElement[2][3] - vp.mElement[2][0]);
	m_frustumPlanes[1].D   = -(vp.mElement[3][3] - vp.mElement[3][0]);
	m_frustumPlanes[1].ComputeIndex();

	// Top clipping plane
	m_frustumPlanes[2].N[0] = -(vp.mElement[0][3] - vp.mElement[0][1]);
	m_frustumPlanes[2].N[1] = -(vp.mElement[1][3] - vp.mElement[1][1]);
	m_frustumPlanes[2].N[2] = -(vp.mElement[2][3] - vp.mElement[2][1]);
	m_frustumPlanes[2].D   = -(vp.mElement[3][3] - vp.mElement[3][1]);
	m_frustumPlanes[2].ComputeIndex();

	// Bottom clipping plane
	m_frustumPlanes[3].N[0] = -(vp.mElement[0][3] + vp.mElement[0][1]);
	m_frustumPlanes[3].N[1] = -(vp.mElement[1][3] + vp.mElement[1][1]);
	m_frustumPlanes[3].N[2] = -(vp.mElement[2][3] + vp.mElement[2][1]);
	m_frustumPlanes[3].D   = -(vp.mElement[3][3] + vp.mElement[3][1]);
	m_frustumPlanes[3].ComputeIndex();
	// Near clipping plane
//** Comments from Klaus Hartman regarding a bug in the initial release
//** now fixed below.
//  Hello, John,
//I downloaded nxodf.zip last night and wanted to have a quick look at it
//before I go to bed. Unfortunately, I don't have the latest DirectX 9
//installed, yet, so it didn't work for me (will do that today). Instead I
//took at quick look at the source code and detected a problem in your frustum
//plane extraction code. It is partially my fault that people use wrong
//extraction code for Direct3D, because I once provided an incorrect sample. I
//tried to undo this error by writing a paper about the extraction algorithm
//(together with Gil Gribb), even posted about it on the GD Algorithms list.
//But there are people who missed that, so sometimes I find incorrect versions
//of the plane extraction. So let me fix my bug in your code...
//
//Transforming a point P=(x, y, z, w=1) with the perspective projection matrix
//results in a point P'=(x', y', z', w'). In OpenGL, this point is within the
//viewing frustum, if all of the following inequalities are true:
//
//-w' < x' < w'
//-w' < y' < w'
//-w' < z' < w'
//
//The use of w' here already hints that the fourth column of the projection
//matrix is involved in extracting the view frustum planes. However, these
//inequalities are different for Direct3D:
//
//-w' < x' < w'
//-w' < y' < w'
//0 < z' < w' (different near plane)
//
//This means that the plane extraction is identical for OpenGL and Direct3D,
//except for the near plane. When using Direct3D, the 4th column is not part
//of the near plane extraction.
//
//This is correct for OpenGL, but not for Direct3D. For Direct3D the code
//becomes (eliminate the use of the 4th column):

#if 0 // this version for OpenGL
	m_frustumPlanes[4].N[0] = -(vp.mElement[0][3] + vp.mElement[0][2]);
	m_frustumPlanes[4].N[1] = -(vp.mElement[1][3] + vp.mElement[1][2]);
	m_frustumPlanes[4].N[2] = -(vp.mElement[2][3] + vp.mElement[2][2]);
	m_frustumPlanes[4].D = -(vp.mElement[3][3] + vp.mElement[3][2]);
#else
	m_frustumPlanes[4].N[0] = -vp.mElement[0][2];
	m_frustumPlanes[4].N[1] = -vp.mElement[1][2];
	m_frustumPlanes[4].N[2] = -vp.mElement[2][2];
	m_frustumPlanes[4].D   =  -vp.mElement[3][2];
	m_frustumPlanes[4].ComputeIndex();
#endif
	//  Far clipping plane
	m_frustumPlanes[5].N[0] = -(vp.mElement[0][3] - vp.mElement[0][2]);
	m_frustumPlanes[5].N[1] = -(vp.mElement[1][3] - vp.mElement[1][2]);
	m_frustumPlanes[5].N[2] = -(vp.mElement[2][3] - vp.mElement[2][2]);
	m_frustumPlanes[5].D   = -(vp.mElement[3][3] - vp.mElement[3][2]);
	m_frustumPlanes[5].ComputeIndex();

}

void Frustum::ComputeExtreme(const float *bound,const FrustumPlane &plane,unsigned int &istate,ViewTest flag) const
{
	float minExtreme[3];
	float maxExtreme[3];

	plane.ComputeExtremes(bound,minExtreme,maxExtreme);
	float d1 = plane.N[0] * minExtreme[0] + plane.N[1] * minExtreme[1] + plane.N[2] * minExtreme[2] + plane.D;
	if ( d1 > 0.0f)
	{
		istate|=(VT_INSIDE_0 | VT_INSIDE_1 | VT_INSIDE_2 | VT_INSIDE_3 | VT_INSIDE_4 | VT_INSIDE_5 | VT_OUTSIDE | VT_PARTIAL);
	}
	else
	{
		float d2 = plane.N[0] * maxExtreme[0] + plane.N[1] * maxExtreme[1] +  plane.N[2] * maxExtreme[2] + plane.D;
		if ( d2 >= 0.0f )
			istate|=VT_PARTIAL;
		else
			istate|=flag;  // inside this plane...
	}
}



ViewTest Frustum::ViewTestAABB(const float *bound,ViewTest state)
{
	unsigned int istate = state;
	istate&=~VT_PARTIAL; // turn off the partial bit...

	if ( !(istate & VT_INSIDE_0) ) ComputeExtreme(bound,m_frustumPlanes[0],istate,VT_INSIDE_0);
	if ( !(istate & VT_INSIDE_1) ) ComputeExtreme(bound,m_frustumPlanes[1],istate,VT_INSIDE_1);
	if ( !(istate & VT_INSIDE_2) ) ComputeExtreme(bound,m_frustumPlanes[2],istate,VT_INSIDE_2);
	if ( !(istate & VT_INSIDE_3) ) ComputeExtreme(bound,m_frustumPlanes[3],istate,VT_INSIDE_3);
	if ( !(istate & VT_INSIDE_4) ) ComputeExtreme(bound,m_frustumPlanes[4],istate,VT_INSIDE_4);
	if ( !(istate & VT_INSIDE_5) ) ComputeExtreme(bound,m_frustumPlanes[5],istate,VT_INSIDE_5);

	if ( !(istate & VT_PARTIAL) )
			istate = VT_INSIDE;

	return (ViewTest) istate;
}


void Frustum::GetPlane(unsigned int index,float *plane) const // retrieve the plane equation as XYZD
{
	assert( index >= 0 && index < 6 );
	plane[0] = m_frustumPlanes[index].N[0];
	plane[1] = m_frustumPlanes[index].N[1];
	plane[2] = m_frustumPlanes[index].N[2];
	plane[3] = m_frustumPlanes[index].D;
}
