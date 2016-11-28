// Application5.cpp: implementation of the Application5 class.
//
//////////////////////////////////////////////////////////////////////

/*
 * application test code for homework assignment #5
*/

#include "stdafx.h"
#include "CS580HW.h"
#include "Application5.h"
#include "Gz.h"
#include "disp.h"
#include "rend.h"
#include "MyRaycast.h"
#include "MyHelperFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INFILE  "ppot.asc"
#define OUTFILE "output.ppm"

#define	M_PI	(3.14159265359)

extern int tex_fun(float u, float v, GzColor color); /* image texture function */
extern int ptex_fun(float u, float v, GzColor color); /* procedural texture function */
extern int bokehtex_fun(float u, float v, GzColor color); /* bokeh texture function */

void shade(GzCoord norm, GzCoord color);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Application5::Application5()
{

}

Application5::~Application5()
{
	Clean();
}

int Application5::Initialize()
{
	GzCamera	camera;  
	int		    xRes, yRes;	/* display parameters */ 

	GzToken		nameListShader[9]; 	    /* shader attribute names */
	GzPointer   valueListShader[9];		/* shader attribute pointers */
	GzToken     nameListLights[10];		/* light info */
	GzPointer   valueListLights[10];
	int			shaderType, interpStyle;
	float		specpower;
	int		status; 
 
	status = 0; 

	/* 
	 * Allocate memory for user input
	 */
	m_pUserInput = new GzInput;

	/* 
	 * initialize the display and the renderer 
	 */ 
 	m_nWidth = 512;		// frame buffer and display width
	m_nHeight = 512;    // frame buffer and display height

	status |= GzNewFrameBuffer(&m_pFrameBuffer, m_nWidth, m_nHeight);

	status |= GzNewDisplay(&m_pDisplay, m_nWidth, m_nHeight);
	status |= GzGetDisplayParams(m_pDisplay, &xRes, &yRes);   

	status |= GzNewDisplay(&m_pDisplay1, m_nWidth, m_nHeight);
	status |= GzGetDisplayParams(m_pDisplay1, &xRes, &yRes);
	status |= GzNewRender(&m_pRender1, m_pDisplay1);

	status |= GzNewDisplay(&m_pDisplay2, m_nWidth, m_nHeight);
	status |= GzGetDisplayParams(m_pDisplay2, &xRes, &yRes);
	status |= GzNewRender(&m_pRender2, m_pDisplay2);

	status |= GzNewDisplay(&m_pDisplay3, m_nWidth, m_nHeight);
	status |= GzGetDisplayParams(m_pDisplay3, &xRes, &yRes);
	status |= GzNewRender(&m_pRender3, m_pDisplay3);

	status |= GzNewDisplay(&m_pDisplay4, m_nWidth, m_nHeight);
	status |= GzGetDisplayParams(m_pDisplay4, &xRes, &yRes);
	status |= GzNewRender(&m_pRender4, m_pDisplay4);

	status |= GzNewDisplay(&m_pDisplay5, m_nWidth, m_nHeight);
	status |= GzGetDisplayParams(m_pDisplay5, &xRes, &yRes);
	status |= GzNewRender(&m_pRender5, m_pDisplay5);

	status |= GzNewDisplay(&m_pDisplay6, m_nWidth, m_nHeight);
	status |= GzGetDisplayParams(m_pDisplay6, &xRes, &yRes);
	status |= GzNewRender(&m_pRender6, m_pDisplay6);

/* Translation matrix */
GzMatrix	scale = 
{ 
	3.25,	0.0,	0.0,	-3.0, 
	0.0,	3.25,	0.0,	-3.0, 
	0.0,	0.0,	3.25,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 
 
GzMatrix	rotateX = 
{ 
	1.0,	0.0,	0.0,	0.0, 
	0.0,	1.0,	0.0,	0.0, 
	0.0,	0.0,	1.0,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 
 
GzMatrix	rotateY = 
{ 
	1.0,	0.0,	0.0,	0.0,
	0.0,	1.0,	0.0,	0.0,
	0.0,	0.0,	1.0,	0.0,
	0.0,	0.0,	0.0,	1.0
}; 

#if 1 	/* set up app-defined camera if desired, else use camera defaults */
    camera.position[X] = 10.0;
    camera.position[Y] = 10.0;
    camera.position[Z] = -12.5;

    camera.lookat[X] = 1.0;
    camera.lookat[Y] = 0.0;
    camera.lookat[Z] = 1.0;

    camera.worldup[X] = 0.0;
    camera.worldup[Y] = 1.0;
    camera.worldup[Z] = 0.0;

    camera.FOV = 63.7;              /* degrees *              /* degrees */

	status |= GzPutCamera(m_pRender1, &camera);
	status |= GzPutCamera(m_pRender2, &camera);
	status |= GzPutCamera(m_pRender3, &camera);
	status |= GzPutCamera(m_pRender4, &camera);
	status |= GzPutCamera(m_pRender5, &camera);
	status |= GzPutCamera(m_pRender6, &camera);
#endif 

	/* Start Renderer */
	status |= GzBeginRender(m_pRender1);
	status |= GzBeginRender(m_pRender2);
	status |= GzBeginRender(m_pRender3);
	status |= GzBeginRender(m_pRender4);
	status |= GzBeginRender(m_pRender5);
	status |= GzBeginRender(m_pRender6);

	/* Light */
	GzLight	light1 = { {-0.7071, 0.7071, 0}, {0.5, 0.3, 0.3} };
	GzLight	light2 = { {0, -0.7071, -0.7071}, {0.5, 0.5, 0.3} };
	GzLight	light3 = { {0.7071, 0.0, -0.7071}, {0.7, 0.7, 0.3} };
	GzLight	ambientlight = { {0, 0, 0}, {0.3, 0.3, 0.3} };

	/* Material property */
	GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
	GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
	GzColor diffuseCoefficient = {0.7, 0.7, 0.7};

/* 
  renderer is ready for frame --- define lights and shader at start of frame 
*/

        /*
         * Tokens associated with light parameters
         */
        nameListLights[0] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[0] = (GzPointer)&light1;
        nameListLights[1] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[1] = (GzPointer)&light2;
        nameListLights[2] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[2] = (GzPointer)&light3;

		status |= GzPutAttribute(m_pRender1, 3, nameListLights, valueListLights);
		status |= GzPutAttribute(m_pRender2, 3, nameListLights, valueListLights);
		status |= GzPutAttribute(m_pRender3, 3, nameListLights, valueListLights);
		status |= GzPutAttribute(m_pRender4, 3, nameListLights, valueListLights);
		status |= GzPutAttribute(m_pRender5, 3, nameListLights, valueListLights);
		status |= GzPutAttribute(m_pRender6, 3, nameListLights, valueListLights);

        nameListLights[0] = GZ_AMBIENT_LIGHT;
        valueListLights[0] = (GzPointer)&ambientlight;

		status |= GzPutAttribute(m_pRender1, 1, nameListLights, valueListLights);
		status |= GzPutAttribute(m_pRender2, 1, nameListLights, valueListLights);
		status |= GzPutAttribute(m_pRender3, 1, nameListLights, valueListLights);
		status |= GzPutAttribute(m_pRender4, 1, nameListLights, valueListLights);
		status |= GzPutAttribute(m_pRender5, 1, nameListLights, valueListLights);
		status |= GzPutAttribute(m_pRender6, 1, nameListLights, valueListLights);

        /*
         * Tokens associated with shading 
         */
        nameListShader[0]  = GZ_DIFFUSE_COEFFICIENT;
        valueListShader[0] = (GzPointer)diffuseCoefficient;

	/* 
	* Select either GZ_COLOR or GZ_NORMALS as interpolation mode  
	*/
        nameListShader[1]  = GZ_INTERPOLATE;
        interpStyle = GZ_NORMALS;         /* Phong shading */
        valueListShader[1] = (GzPointer)&interpStyle;

        nameListShader[2]  = GZ_AMBIENT_COEFFICIENT;
        valueListShader[2] = (GzPointer)ambientCoefficient;
        nameListShader[3]  = GZ_SPECULAR_COEFFICIENT;
        valueListShader[3] = (GzPointer)specularCoefficient;
        nameListShader[4]  = GZ_DISTRIBUTION_COEFFICIENT;
        specpower = 128;
        valueListShader[4] = (GzPointer)&specpower;

        nameListShader[5]  = GZ_TEXTURE_MAP;
#if 0   /* set up null texture function or valid pointer */
        valueListShader[5] = (GzPointer)0;
#else
        valueListShader[5] = (GzPointer)(tex_fun);	/* or use ptex_fun */
#endif
		nameListShader[6] = GZ_BOKEH_TEXTURE_MAP;
		valueListShader[6] = (GzPointer)(bokehtex_fun);

		status |= GzPutAttribute(m_pRender1, 7, nameListShader, valueListShader);
		status |= GzPutAttribute(m_pRender2, 7, nameListShader, valueListShader);
		status |= GzPutAttribute(m_pRender3, 7, nameListShader, valueListShader);
		status |= GzPutAttribute(m_pRender4, 7, nameListShader, valueListShader);
		status |= GzPutAttribute(m_pRender5, 7, nameListShader, valueListShader);
		status |= GzPutAttribute(m_pRender6, 7, nameListShader, valueListShader);

		/*
		* Tokens associated with anti-aliasing
		*/
		nameListShader[0] = GZ_AASHIFTX;
		nameListShader[1] = GZ_AASHIFTY;

		double AATruncFilter[2];

		AATruncFilter[0] = AAFilter[0][0]; AATruncFilter[1] = AAFilter[0][1];
		valueListShader[0] = (GzPointer)AATruncFilter;
		valueListShader[1] = (GzPointer)AATruncFilter;
		status |= GzPutAttribute(m_pRender1, 2, nameListShader, valueListShader);

		AATruncFilter[0] = AAFilter[1][0]; AATruncFilter[1] = AAFilter[1][1];
		valueListShader[0] = (GzPointer)AATruncFilter;
		valueListShader[1] = (GzPointer)AATruncFilter;
		status |= GzPutAttribute(m_pRender2, 2, nameListShader, valueListShader);

		AATruncFilter[0] = AAFilter[2][0]; AATruncFilter[1] = AAFilter[2][1];
		valueListShader[0] = (GzPointer)AATruncFilter;
		valueListShader[1] = (GzPointer)AATruncFilter;
		status |= GzPutAttribute(m_pRender3, 2, nameListShader, valueListShader);

		AATruncFilter[0] = AAFilter[3][0]; AATruncFilter[1] = AAFilter[3][1];
		valueListShader[0] = (GzPointer)AATruncFilter;
		valueListShader[1] = (GzPointer)AATruncFilter;
		status |= GzPutAttribute(m_pRender4, 2, nameListShader, valueListShader);

		AATruncFilter[0] = AAFilter[4][0]; AATruncFilter[1] = AAFilter[4][1];
		valueListShader[0] = (GzPointer)AATruncFilter;
		valueListShader[1] = (GzPointer)AATruncFilter;
		status |= GzPutAttribute(m_pRender5, 2, nameListShader, valueListShader);

		AATruncFilter[0] = AAFilter[5][0]; AATruncFilter[1] = AAFilter[5][1];
		valueListShader[0] = (GzPointer)AATruncFilter;
		valueListShader[1] = (GzPointer)AATruncFilter;
		status |= GzPutAttribute(m_pRender6, 2, nameListShader, valueListShader);


	status |= GzPushMatrix(m_pRender1, scale);
	status |= GzPushMatrix(m_pRender1, rotateY);
	status |= GzPushMatrix(m_pRender1, rotateX);

	status |= GzPushMatrix(m_pRender2, scale);
	status |= GzPushMatrix(m_pRender2, rotateY);
	status |= GzPushMatrix(m_pRender2, rotateX);

	status |= GzPushMatrix(m_pRender3, scale);
	status |= GzPushMatrix(m_pRender3, rotateY);
	status |= GzPushMatrix(m_pRender3, rotateX);

	status |= GzPushMatrix(m_pRender4, scale);
	status |= GzPushMatrix(m_pRender4, rotateY);
	status |= GzPushMatrix(m_pRender4, rotateX);

	status |= GzPushMatrix(m_pRender5, scale);
	status |= GzPushMatrix(m_pRender5, rotateY);
	status |= GzPushMatrix(m_pRender5, rotateX);

	status |= GzPushMatrix(m_pRender6, scale);
	status |= GzPushMatrix(m_pRender6, rotateY);
	status |= GzPushMatrix(m_pRender6, rotateX);

	if (status) exit(GZ_FAILURE); 

	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

int Application5::Render() 
{
	GzToken		nameListTriangle[3]; 	/* vertex attribute names */
	GzPointer	valueListTriangle[3]; 	/* vertex attribute pointers */
	GzCoord		vertexList[3];	/* vertex position coordinates */ 
	GzCoord		normalList[3];	/* vertex normals */ 
	GzTextureIndex  	uvList[3];		/* vertex texture map indices */ 
	char		dummy[256]; 
	int			status; 


	/* Initialize Displays */
	status |= GzInitDisplay(m_pDisplay); 

	status |= GzInitDisplay(m_pDisplay1);
	status |= GzInitDisplay(m_pDisplay2);
	status |= GzInitDisplay(m_pDisplay3);
	status |= GzInitDisplay(m_pDisplay4);
	status |= GzInitDisplay(m_pDisplay5);
	status |= GzInitDisplay(m_pDisplay6);	
	/* 
	* Tokens associated with triangle vertex values 
	*/ 
	nameListTriangle[0] = GZ_POSITION; 
	nameListTriangle[1] = GZ_NORMAL; 
	nameListTriangle[2] = GZ_TEXTURE_INDEX;  

	// I/O File open
	FILE *infile;
	if( (infile  = fopen( INFILE , "r" )) == NULL )
	{
         AfxMessageBox( "The input file was not opened\n" );
		 return GZ_FAILURE;
	}

	FILE *outfile;
	if( (outfile  = fopen( OUTFILE , "wb" )) == NULL )
	{
         AfxMessageBox( "The output file was not opened\n" );
		 return GZ_FAILURE;
	}

	/* 
	* Walk through the list of triangles, set color 
	* and render each triangle 
	*/ 
	while( fscanf(infile, "%s", dummy) == 1) { 	/* read in tri word */
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[0][0]), &(vertexList[0][1]),  
		&(vertexList[0][2]), 
		&(normalList[0][0]), &(normalList[0][1]), 	
		&(normalList[0][2]), 
		&(uvList[0][0]), &(uvList[0][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[1][0]), &(vertexList[1][1]), 	
		&(vertexList[1][2]), 
		&(normalList[1][0]), &(normalList[1][1]), 	
		&(normalList[1][2]), 
		&(uvList[1][0]), &(uvList[1][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[2][0]), &(vertexList[2][1]), 	
		&(vertexList[2][2]), 
		&(normalList[2][0]), &(normalList[2][1]), 	
		&(normalList[2][2]), 
		&(uvList[2][0]), &(uvList[2][1]) ); 

	    /* 
	     * Set the value pointers to the first vertex of the 	
	     * triangle, then feed it to the renderer 
	     * NOTE: this sequence matches the nameList token sequence
	     */ 
	     valueListTriangle[0] = (GzPointer)vertexList; 
		 valueListTriangle[1] = (GzPointer)normalList; 
		 valueListTriangle[2] = (GzPointer)uvList; 

		 GzPutTriangle(m_pRender1, 3, nameListTriangle, valueListTriangle);
		 GzPutTriangle(m_pRender2, 3, nameListTriangle, valueListTriangle);
		 GzPutTriangle(m_pRender3, 3, nameListTriangle, valueListTriangle);
		 GzPutTriangle(m_pRender4, 3, nameListTriangle, valueListTriangle);
		 GzPutTriangle(m_pRender5, 3, nameListTriangle, valueListTriangle);
		 GzPutTriangle(m_pRender6, 3, nameListTriangle, valueListTriangle);
	}

	for (int j = 0; j < m_pDisplay->yres; j++) {
		for (int i = 0; i < m_pDisplay->xres; i++) {
			GzIntensity r(0), g(0), b(0);

			r += m_pDisplay1->fbuf[(i + (j * m_pDisplay1->xres))].red * AAFilter[0][2];
			g += m_pDisplay1->fbuf[(i + (j * m_pDisplay1->xres))].green * AAFilter[0][2];
			b += m_pDisplay1->fbuf[(i + (j * m_pDisplay1->xres))].blue * AAFilter[0][2];

			r += m_pDisplay2->fbuf[(i + (j * m_pDisplay2->xres))].red * AAFilter[1][2];
			g += m_pDisplay2->fbuf[(i + (j * m_pDisplay2->xres))].green * AAFilter[1][2];
			b += m_pDisplay2->fbuf[(i + (j * m_pDisplay2->xres))].blue * AAFilter[1][2];

			r += m_pDisplay3->fbuf[(i + (j * m_pDisplay3->xres))].red * AAFilter[2][2];
			g += m_pDisplay3->fbuf[(i + (j * m_pDisplay3->xres))].green * AAFilter[2][2];
			b += m_pDisplay3->fbuf[(i + (j * m_pDisplay3->xres))].blue * AAFilter[2][2];

			r += m_pDisplay4->fbuf[(i + (j * m_pDisplay4->xres))].red * AAFilter[3][2];
			g += m_pDisplay4->fbuf[(i + (j * m_pDisplay4->xres))].green * AAFilter[3][2];
			b += m_pDisplay4->fbuf[(i + (j * m_pDisplay4->xres))].blue * AAFilter[3][2];

			r += m_pDisplay5->fbuf[(i + (j * m_pDisplay5->xres))].red * AAFilter[4][2];
			g += m_pDisplay5->fbuf[(i + (j * m_pDisplay5->xres))].green * AAFilter[4][2];
			b += m_pDisplay5->fbuf[(i + (j * m_pDisplay5->xres))].blue * AAFilter[4][2];

			r += m_pDisplay6->fbuf[(i + (j * m_pDisplay6->xres))].red * AAFilter[5][2];
			g += m_pDisplay6->fbuf[(i + (j * m_pDisplay6->xres))].green * AAFilter[5][2];
			b += m_pDisplay6->fbuf[(i + (j * m_pDisplay6->xres))].blue * AAFilter[5][2];

			m_pDisplay->fbuf[(i + (j * m_pDisplay->xres))].red = max(0, min(r, 4095));
			m_pDisplay->fbuf[(i + (j * m_pDisplay->xres))].green = max(0, min(g, 4095));
			m_pDisplay->fbuf[(i + (j * m_pDisplay->xres))].blue = max(0, min(b, 4095));
		}
	}

	// Depth of field
	GzCoord cameraPos;
	cameraPos[X] = m_pDisplay->xres / 2.0f;
	cameraPos[Y] = m_pDisplay->yres / 2.0f;

	float radian = m_pRender1->camera.FOV * (M_PI / 180);
	float d = (float)m_pDisplay->xres / 2.0f / tan(radian / 2);
	cameraPos[Z] = -d;

	// Set camera parameters here
	float focalDist = d * 1.2f;
	float apertureSize = 45.0f;
	float blurWeight = 0.95f;

	for (int j = 0; j < m_pDisplay->yres; j++) {
		for (int i = 0; i < m_pDisplay->xres; i++) {
			GzPixel pixel = m_pDisplay->fbuf[i + j * m_pDisplay->xres];
			GzCoord pixelCoord{ i, j, pixel.z / (float)INT_MAX }; // Scale z between [0,1] 

			MyRay fray;
			GzInitRay(&fray, cameraPos, pixelCoord);
			MyRaycast* focal = new MyRaycast();
			GzInitRaycastWithFocal(focal, &fray, focalDist, m_pRender1);
			GzCoord focalPoint;
			coordCopy(focalPoint, focal->focalPoint);

			int rayCount = GZ_RAYCAST_COUNT;
			int blurCount = 0;
			GzColor blurColors = { 0,0,0 };

			for (int a = 0; a != rayCount; ++a) {
				MyRay ray;
				GzCoord aperturePos;
				coordCopy(aperturePos, cameraPos);

				// Randomize aperture raycasting position
				if (rand() % 2) {
					aperturePos[X] += (((double)rand() / (double)RAND_MAX) * apertureSize) * sin(((double)rand() / (double)RAND_MAX) * 2.0f * M_PI);
					aperturePos[Y] += (((double)rand() / (double)RAND_MAX) * apertureSize) * cos(((double)rand() / (double)RAND_MAX) * 2.0f * M_PI);
				}
				else {
					aperturePos[X] -= (((double)rand() / (double)RAND_MAX) * apertureSize) * sin(((double)rand() / (double)RAND_MAX) * 2.0f * M_PI);
					aperturePos[Y] -= (((double)rand() / (double)RAND_MAX) * apertureSize) * cos(((double)rand() / (double)RAND_MAX) * 2.0f * M_PI);
				}

				GzInitRay(&ray, aperturePos, focalPoint);
				MyRaycast *raycast = new MyRaycast();
				GzInitRaycast(raycast, &ray, m_pRender1);
				GzPixel hitPixel = m_pDisplay->fbuf[(int)roundf(raycast->nearestHit[X]) +
					((int)roundf(raycast->nearestHit[Y])) * m_pDisplay->xres];

				if (hitPixel.red < 0 || hitPixel.green < 0 || hitPixel.blue < 0)
					continue;

				if (hitPixel.red != m_pDisplay->fbuf[i + j * m_pDisplay->xres].red
					&& hitPixel.green != m_pDisplay->fbuf[i + j * m_pDisplay->xres].green
					&& hitPixel.blue != m_pDisplay->fbuf[i + j * m_pDisplay->xres].blue) {

					// Accumulate blur colors
					blurColors[RED] += hitPixel.red;
					blurColors[GREEN] += hitPixel.green;
					blurColors[BLUE] += hitPixel.blue;
					blurCount++;
				}
				else {
					blurColors[RED] = hitPixel.red;
					blurColors[GREEN] = hitPixel.green;
					blurColors[BLUE] = hitPixel.blue;
					blurCount = 1;
				}
				delete raycast;
			}
			// Average blur colors
			blurColors[RED] /= blurCount;
			blurColors[GREEN] /= blurCount;
			blurColors[BLUE] /= blurCount;

			delete focal;

			m_pDisplay->fbuf[i + j * m_pDisplay->xres].red = m_pDisplay->fbuf[i + j * m_pDisplay->xres].red *
				(1.0f - blurWeight) + blurWeight * blurColors[RED];
			m_pDisplay->fbuf[i + j * m_pDisplay->xres].green = m_pDisplay->fbuf[i + j * m_pDisplay->xres].green *
				(1.0f - blurWeight) + blurWeight * blurColors[GREEN];
			m_pDisplay->fbuf[i + j * m_pDisplay->xres].blue = m_pDisplay->fbuf[i + j * m_pDisplay->xres].blue *
				(1.0f - blurWeight) + blurWeight * blurColors[BLUE];

			// Draw bokeh texture
			if (m_pDisplay1->fbuf[i + j * m_pDisplay->xres].z != MAXINT) {
				int bokehScale = (((float)blurCount / (float)GZ_RAYCAST_COUNT) * 20); // controlled by distance to focal plane
				float bokehAlpha = (4.0f / (float)bokehScale); // controlled by distance to focal plane

				if (bokehScale < 19)
					continue;

				for (int k = 0; k < bokehScale; k++) {
					for (int l = 0; l < bokehScale; l++) {
						GzColor bokehColor;
						float u = (float)l / bokehScale;
						float v = (float)k / bokehScale;
						(GzTexture(m_pRender1->bokehtex_fun))(u, v, bokehColor);

						bokehColor[RED] = (short)((int)(bokehColor[RED] * ((1 << 12) - 1)));
						bokehColor[GREEN] = (short)((int)(bokehColor[GREEN] * ((1 << 12) - 1)));
						bokehColor[BLUE] = (short)((int)(bokehColor[BLUE] * ((1 << 12) - 1)));

						bokehColor[RED] = max(0, min(bokehColor[RED], 4095));
						bokehColor[GREEN] = max(0, min(bokehColor[GREEN], 4095));
						bokehColor[BLUE] = max(0, min(bokehColor[BLUE], 4095));

						// Only draw bokeh mask
						if (bokehColor[RED] == 0
							&& bokehColor[GREEN] == 0
							&& bokehColor[BLUE] == 0)
							continue;


						if ((i - (bokehScale / 2) + l) + (j - (bokehScale / 2) + k) * m_pDisplay->xres < 0)
							continue;

						bokehColor[RED] = m_pDisplay->fbuf[i + j * m_pDisplay->xres].red;
						bokehColor[GREEN] = m_pDisplay->fbuf[i + j * m_pDisplay->xres].green;
						bokehColor[BLUE] = m_pDisplay->fbuf[i + j * m_pDisplay->xres].blue;

						if(bokehColor[RED])
						if ((i - (bokehScale / 2) + l)  < 0 || (i - (bokehScale / 2) + l) > m_nWidth
							|| (j - (bokehScale / 2) + k) < 0 || (j - (bokehScale / 2) + k) > m_nHeight)
							continue;

						m_pDisplay->fbuf[(i - (bokehScale / 2) + l) + (j - (bokehScale / 2) + k) * m_pDisplay->xres].red =
							m_pDisplay->fbuf[(i - (bokehScale / 2) + l) + (j - (bokehScale / 2) + k) * m_pDisplay->xres].red * (1.0f - bokehAlpha) + (bokehAlpha * bokehColor[RED]);
						m_pDisplay->fbuf[(i - (bokehScale / 2) + l) + (j - (bokehScale / 2) + k) * m_pDisplay->xres].green =
							m_pDisplay->fbuf[(i - (bokehScale / 2) + l) + (j - (bokehScale / 2) + k) * m_pDisplay->xres].green * (1.0f - bokehAlpha) + (bokehAlpha * bokehColor[GREEN]);
						m_pDisplay->fbuf[(i - (bokehScale / 2) + l) + (j - (bokehScale / 2) + k) * m_pDisplay->xres].blue =
							m_pDisplay->fbuf[(i - (bokehScale / 2) + l) + (j - (bokehScale / 2) + k) * m_pDisplay->xres].blue * (1.0f - bokehAlpha) + (bokehAlpha * bokehColor[BLUE]);
					}
				}
			}
		}
	}
	// End depth of field

	GzFlushDisplay2File(outfile, m_pDisplay); 	/* write out or update display to file*/
	GzFlushDisplay2FrameBuffer(m_pFrameBuffer, m_pDisplay);	// write out or update display to frame buffer

	/* 
	 * Close file
	 */ 

	if( fclose( infile ) )
      AfxMessageBox( "The input file was not closed\n" );

	if( fclose( outfile ) )
      AfxMessageBox( "The output file was not closed\n" );
 
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

int Application5::Clean()
{
	/* 
	 * Clean up and exit 
	 */ 
	int	status = 0; 
 
	status |= GzFreeDisplay(m_pDisplay);

	status |= GzFreeRender(m_pRender1);
	status |= GzFreeDisplay(m_pDisplay1);

	status |= GzFreeRender(m_pRender2);
	status |= GzFreeDisplay(m_pDisplay2);

	status |= GzFreeRender(m_pRender3);
	status |= GzFreeDisplay(m_pDisplay3);

	status |= GzFreeRender(m_pRender4);
	status |= GzFreeDisplay(m_pDisplay4);

	status |= GzFreeRender(m_pRender5);
	status |= GzFreeDisplay(m_pDisplay5);

	status |= GzFreeRender(m_pRender6);
	status |= GzFreeDisplay(m_pDisplay6);

	status |= GzFreeTexture();
	
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS);
}



