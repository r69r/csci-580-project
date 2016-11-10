/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#define		M_PI	(3.14159265359)


int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value

	float radian = (M_PI / 180) * degree;
	mat[1][1] = cos(radian);
	mat[1][2] = -sin(radian);
	mat[2][1] = sin(radian);
	mat[2][2] = cos(radian);

	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value

	float radian = (M_PI / 180) * degree;
	mat[0][0] = cos(radian);
	mat[0][2] = sin(radian);
	mat[2][0] = -sin(radian);
	mat[2][2] = cos(radian);

	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value

	float radian = (M_PI / 180) * degree;
	mat[0][0] = cos(radian);
	mat[0][1] = -sin(radian);
	mat[1][0] = sin(radian);
	mat[1][1] = cos(radian);

	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value

	mat[0][3] = translate[X];
	mat[1][3] = translate[Y];
	mat[2][3] = translate[Z];

	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
	// Create scaling matrix
	// Pass back the matrix using mat value

	mat[0][0] = scale[X];
	mat[1][1] = scale[Y];
	mat[2][2] = scale[Z];

	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- setup Xsp and anything only done once 
- save the pointer to display 
- init default camera 
*/ 
	if (display == NULL)
		return GZ_FAILURE;

	/* Initialize the default camera */
	GzCamera camera;
	
	camera.position[X] = DEFAULT_IM_X; camera.position[Y] = DEFAULT_IM_Y; camera.position[Z] = DEFAULT_IM_Z;
	camera.lookat[X] = 0; camera.lookat[Y] = 0; camera.lookat[Z] = 0;
	camera.worldup[X] = DEFAULT_WU_X; camera.worldup[Y] = DEFAULT_WU_Y; camera.worldup[Z] = DEFAULT_WU_Z;
	camera.FOV = DEFAULT_FOV;

	*render = new GzRender{ display, camera, short(0), short(0) };

	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	if (render != NULL)
		delete render;

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
/*  
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 
	if (render == NULL)
		return GZ_FAILURE;

	GzInitDisplay(render->display);
	render->numlights = 0;

	/* Set up Xsp matrix */
	GzMatrix	Xsp =
	{
		render->display->xres / 2,	0.0,	0.0,	render->display->xres / 2,
		0.0,	-(render->display->yres / 2),	0.0,	render->display->yres / 2,
		0.0,	0.0,	MAXINT,	0.0,
		0.0,	0.0,	0.0,	1.0
	};

	memcpy(render->Xsp, Xsp, sizeof(GzMatrix));

	/* Compute Xpi matrix */
	float radian = (M_PI / 180) * render->camera.FOV;
	float d = 1 / tan(radian / 2);
	GzMatrix Xpi =
	{
		1.0,	0.0,	0.0,	0.0,
		0.0,	1.0,	0.0,	0.0,
		0.0,	0.0,	1/d,	0.0,
		0.0,	0.0,	1/d,	1.0
	};

	memcpy(render->camera.Xpi, Xpi, sizeof(GzMatrix));

	/* Compute Xiw matrix */
	GzCoord C = { render->camera.position[X], render->camera.position[Y], render->camera.position[Z] };
	GzCoord L = { render->camera.lookat[X], render->camera.lookat[Y], render->camera.lookat[Z] };

	GzCoord zVector = { L[X] - C[X], L[Y] - C[Y], L[Z] - C[Z] };
	float zMagnitude = sqrt((zVector[X] * zVector[X]) + (zVector[Y] * zVector[Y]) + (zVector[Z] * zVector[Z]));
	zVector[X] /= zMagnitude; zVector[Y] /= zMagnitude; zVector[Z] /= zMagnitude;

	GzCoord up = { render->camera.worldup[X], render->camera.worldup[Y], render->camera.worldup[Z] };
	float upDotZ = (up[X] * zVector[X]) + (up[Y] * zVector[Y]) + (up[Z] * zVector[Z]);
	GzCoord up_ = { up[X] - (upDotZ * zVector[X]), up[Y] - (upDotZ * zVector[Y]), up[Z] - (upDotZ * zVector[Z]) };

	float up_Magnitude = sqrt((up_[X] * up_[X]) + (up_[Y] * up_[Y]) + (up_[Z] * up_[Z]));
	GzCoord yVector = { up_[X] / up_Magnitude, up_[Y] / up_Magnitude, up_[Z] / up_Magnitude };

	float Xi = yVector[Y] * zVector[Z] - zVector[Y] * yVector[Z];
	float Xj = zVector[X] * yVector[Z] - yVector[X] * zVector[Z];
	float Xk = yVector[X] * zVector[Y] - zVector[X] * yVector[Y];
	GzCoord	xVector = { Xi, Xj, Xk };

	float Cx = (xVector[X] * C[X]) + (xVector[Y] * C[Y]) + (xVector[Z] * C[Z]);
	float Cy = (yVector[X] * C[X]) + (yVector[Y] * C[Y]) + (yVector[Z] * C[Z]);
	float Cz = (zVector[X] * C[X]) + (zVector[Y] * C[Y]) + (zVector[Z] * C[Z]);

	GzMatrix Xiw =
	{
		xVector[X],	xVector[Y], xVector[Z], -Cx,
		yVector[X],	yVector[Y], yVector[Z], -Cy,
		zVector[X],	zVector[Y], zVector[Z], -Cz,
		0.0,	0.0,	0.0,	1.0
	};

	memcpy(render->camera.Xiw, Xiw, sizeof(GzMatrix));

	/* Initialize Ximage matrix */
	memcpy(render->Ximage[render->matlevelImage], render->Xsp, sizeof(GzMatrix));

	/* Initialize the Xnorm matrix */
	GzMatrix normMatrix =
	{
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	1.0
	};

	/* Only push unitary rotations to the normals stack */
	float scaleFactor = sqrt((render->camera.Xiw[0][0] * render->camera.Xiw[0][0])
		+ (render->camera.Xiw[0][1] * render->camera.Xiw[0][1])
		+ (render->camera.Xiw[0][2] * render->camera.Xiw[0][2]));

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
				normMatrix[i][j] = (render->camera.Xiw[i][j] / scaleFactor);
		}
	}

	memcpy(render->Xnorm[render->matlevelNormal], normMatrix, sizeof(GzMatrix));

	/* Push Xpi onto Ximage stack */
	GzMatrix concatMatrix =
	{
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++)
				concatMatrix[i][j] += (render->Ximage[render->matlevelImage])[i][k] * render->camera.Xpi[k][j];
		}
	}
	render->matlevelImage++;

	memcpy(render->Ximage[render->matlevelImage], concatMatrix, sizeof(GzMatrix));

	/* Push Xiw onto Ximage stack */
	GzMatrix concatMatrix1 =
	{
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++)
				concatMatrix1[i][j] += (render->Ximage[render->matlevelImage])[i][k] * render->camera.Xiw[k][j];
		}
	}
	render->matlevelImage++;

	memcpy(render->Ximage[render->matlevelImage], concatMatrix1, sizeof(GzMatrix));

	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	if (render == NULL || camera == NULL)
		return GZ_FAILURE;

	render->camera = *camera;

	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if (render == NULL || render->matlevelImage == MATLEVELS)
		return GZ_FAILURE;

	GzMatrix concatMatrix =
	{
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++)
				concatMatrix[i][j] += (render->Ximage[render->matlevelImage])[i][k] * matrix[k][j];
		}
	}
	render->matlevelImage++;

	memcpy(render->Ximage[render->matlevelImage], concatMatrix, sizeof(GzMatrix));

/*
- push a matrix onto the Xnorm stack
- check for stack overflow
*/
	if (render->matlevelNormal == MATLEVELS)
		return GZ_FAILURE;
	
	GzMatrix normMatrix =
	{
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0
	};

	/* Only push unitary rotations to the normals stack */
	float scaleFactor = sqrt((matrix[0][0] * matrix[0][0])
		+ (matrix[0][1] * matrix[0][1])
		+ (matrix[0][2] * matrix[0][2]));

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++)
				normMatrix[i][j] += (render->Xnorm[render->matlevelNormal])[i][k] * (matrix[k][j] / scaleFactor);
		}
	}

	/* Eliminate translation from the normal matrix */
	normMatrix[0][3] = 0; normMatrix[1][3] = 0; normMatrix[2][3] = 0;
	render->matlevelNormal++;

	memcpy(render->Xnorm[render->matlevelNormal], normMatrix, sizeof(GzMatrix));

	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage and Xnorm stack
- check for stack underflow
*/
	if (render == NULL || render->matlevelImage == 0)
		return GZ_FAILURE;

	render->matlevelImage--;

	if(render->matlevelNormal == 0)
		return GZ_FAILURE;

	render->matlevelNormal--;

	return GZ_SUCCESS;
}

int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	
	if (render == NULL)
		return GZ_FAILURE;

	for (int i = 0; i < numAttributes; i++) {
		switch (nameList[i]) {
		case GZ_RGB_COLOR:	render->flatcolor[RED] = ((float*)*valueList)[RED];
							render->flatcolor[GREEN] = ((float*)*valueList)[GREEN];
							render->flatcolor[BLUE] = ((float*)*valueList)[BLUE];
							valueList++;
							break;

		case GZ_INTERPOLATE:	render->interp_mode = (*(int*)*valueList);
								valueList++;
								break;

		case GZ_DIRECTIONAL_LIGHT:	render->lights[render->numlights++] = (*(GzLight*)*valueList);
									valueList++;
									break;

		case GZ_AMBIENT_LIGHT:	render->ambientlight = (*(GzLight*)*valueList);
								valueList++;
								break;

		case GZ_AMBIENT_COEFFICIENT:	render->Ka[RED] = ((float*)*valueList)[RED];
										render->Ka[GREEN] = ((float*)*valueList)[GREEN];
										render->Ka[BLUE] = ((float*)*valueList)[BLUE];
										valueList++;
										break;

		case GZ_DIFFUSE_COEFFICIENT:	render->Kd[RED] = ((float*)*valueList)[RED];
										render->Kd[GREEN] = ((float*)*valueList)[GREEN];
										render->Kd[BLUE] = ((float*)*valueList)[BLUE];
										valueList++;
										break;

		case GZ_SPECULAR_COEFFICIENT:	render->Ks[RED] = ((float*)*valueList)[RED];
										render->Ks[GREEN] = ((float*)*valueList)[GREEN];
										render->Ks[BLUE] = ((float*)*valueList)[BLUE];
										valueList++;
										break;

		case GZ_DISTRIBUTION_COEFFICIENT:	render->spec = (*(float*)*valueList);
											valueList++;
											break;

		case GZ_TEXTURE_MAP:	render->tex_fun = (*(GzTexture*)valueList);
								valueList++;
								break;

		case GZ_BOKEH_TEXTURE_MAP:	render->bokehtex_fun = (*(GzTexture*)valueList);
									valueList++;
									break;

		case GZ_AASHIFTX:	render->aaShiftX = ((double*)*valueList)[X];
							valueList++;
							break;

		case GZ_AASHIFTY:	render->aaShiftY = ((double*)*valueList)[Y];
							valueList++;
							break;
		}
	}

	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts using matrix on top of stack 
- Clip - just discard any triangle with any vert(s) behind view plane 
       - optional: test for triangles with all three verts off-screen (trivial frustum cull)
- invoke triangle rasterizer  
*/ 
	if (render == NULL)
		return GZ_FAILURE;

	GzCoord vertexList[3], normalList[3];
	GzTextureIndex uvList[3];
	int valIndex = 0, status = 0;

	for (int i = 0; i < numParts; i++) {
		switch (nameList[i]) {
		case GZ_POSITION:	valIndex = 0;
							for (int vlIndexi = 0; vlIndexi < 3; vlIndexi++)
								for (int vlIndexj = 0; vlIndexj < 3; vlIndexj++)
									vertexList[vlIndexi][vlIndexj] = ((float*)*valueList)[valIndex++];
							valueList++;
							break;

		case GZ_NORMAL:	valIndex = 0;
						for (int vlIndexi = 0; vlIndexi < 3; vlIndexi++)
							for (int vlIndexj = 0; vlIndexj < 3; vlIndexj++)
								normalList[vlIndexi][vlIndexj] = ((float*)*valueList)[valIndex++];
						valueList++;
						break;

		case GZ_TEXTURE_INDEX:	valIndex = 0;
								for (int vlIndexi = 0; vlIndexi < 3; vlIndexi++)
									for (int vlIndexj = 0; vlIndexj < 2; vlIndexj++)
										uvList[vlIndexi][vlIndexj] = ((float*)*valueList)[valIndex++];
								valueList++;
								break;
		}
	}

	/* Convert vertices to 4D homogenious coordinates */
	GzMatrix vertices =
	{
		vertexList[0][0],	vertexList[1][0],	vertexList[2][0],	0.0,
		vertexList[0][1],	vertexList[1][1],	vertexList[2][1],	0.0,
		vertexList[0][2],	vertexList[1][2],	vertexList[2][2],	0.0,
		1.0,	1.0,	1.0,	0.0
	};

	GzMatrix concatMatrix =
	{
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++)
				concatMatrix[i][j] += (render->Ximage[render->matlevelImage])[i][k] * vertices[k][j];
		}
	}

	/* Convert transformed matrix back to 3D vertices */
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			vertexList[j][i] = concatMatrix[i][j] / concatMatrix[3][j];

	/* Convert normals to 4D homogenious coordinates */
	GzMatrix normals =
	{
		normalList[0][0],	normalList[1][0],	normalList[2][0],	0.0,
		normalList[0][1],	normalList[1][1],	normalList[2][1],	0.0,
		normalList[0][2],	normalList[1][2],	normalList[2][2],	0.0,
		1.0,	1.0,	1.0,	0.0
	};

	GzMatrix normMatrix =
	{
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0,
		0.0,	0.0,	0.0,	0.0
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++)
				normMatrix[i][j] += (render->Xnorm[render->matlevelNormal])[i][k] * normals[k][j];
		}
	}

	/* Convert transformed matrix back to 3D normals */
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			normalList[j][i] = normMatrix[i][j] / normMatrix[3][j];

	/* Skip any triangle with any vertex behind the image plane */
	if (vertexList[0][2] < 0 || vertexList[1][2] < 0 || vertexList[2][2] < 0)
		return GZ_SUCCESS;

	/* Trivial frustum culling */
	if ((vertexList[0][0] < 0 || vertexList[0][0] > render->display->xres
		|| (vertexList[0][1] < 0 || vertexList[0][1] > render->display->yres))
	&& ((vertexList[1][0] < 0 || vertexList[1][0] > render->display->xres)
		|| (vertexList[1][1] < 0 || vertexList[1][1] > render->display->yres))
	&& ((vertexList[2][0] < 0 || vertexList[2][0] > render->display->xres)
		|| (vertexList[2][1] < 0 || vertexList[2][1] > render->display->yres)))
		return GZ_SUCCESS;

	/* Perform AA shifts */
	for (int i = 0; i < 3; i++) {
		vertexList[i][X] += render->aaShiftX;
		vertexList[i][Y] += render->aaShiftY;
	}

	status |= GzRasterizeTriangle(render, vertexList, normalList, uvList);

	if (status)
		return GZ_FAILURE;

	return GZ_SUCCESS;
}

int GzRasterizeTriangle(GzRender *render, GzCoord *vertexList, GzCoord *normalList, GzTextureIndex *uvList)
{
	if (render == NULL)
		return GZ_FAILURE;

	GzCoord v1, v2, v3, n1, n2, n3;
	GzTextureIndex uv1, uv2, uv3;

	/* Bounding box parameters */
	float xMin = min(min(vertexList[0][0], vertexList[1][0]), vertexList[2][0]);
	float xMax = max(max(vertexList[0][0], vertexList[1][0]), vertexList[2][0]);
	float yMin = min(min(vertexList[0][1], vertexList[1][1]), vertexList[2][1]);
	float yMax = max(max(vertexList[0][1], vertexList[1][1]), vertexList[2][1]);

	/* Sort vertices, normals and UV coordinates by Y */
	for (int i = 0; i < 3; i++) {
		if (vertexList[i][1] == yMax) {
			v1[0] = vertexList[i][0]; v1[1] = vertexList[i][1]; v1[2] = vertexList[i][2];
			n1[0] = normalList[i][0]; n1[1] = normalList[i][1]; n1[2] = normalList[i][2];
			uv1[0] = uvList[i][0]; uv1[1] = uvList[i][1];
		}
		if (vertexList[i][1] == yMin) {
			v3[0] = vertexList[i][0]; v3[1] = vertexList[i][1]; v3[2] = vertexList[i][2];
			n3[0] = normalList[i][0]; n3[1] = normalList[i][1]; n3[2] = normalList[i][2];
			uv3[0] = uvList[i][0]; uv3[1] = uvList[i][1];
		}
		if (vertexList[i][1] != yMax && vertexList[i][1] != yMin) {
			v2[0] = vertexList[i][0]; v2[1] = vertexList[i][1]; v2[2] = vertexList[i][2];
			n2[0] = normalList[i][0]; n2[1] = normalList[i][1]; n2[2] = normalList[i][2];
			uv2[0] = uvList[i][0]; uv2[1] = uvList[i][1];
		}
	}

	/* Sort vertices, normals and UV coordinates by X */
	if (v3[0] > v1[0] && v2[0] < v1[0]) {
		GzCoord temp = { v3[0], v3[1], v3[2] };
		v3[0] = v2[0]; v3[1] = v2[1]; v3[2] = v2[2];
		v2[0] = temp[0]; v2[1] = temp[1]; v2[2] = temp[2];

		GzCoord tempNorm = { n3[0], n3[1], n3[2] };
		n3[0] = n2[0]; n3[1] = n2[1]; n3[2] = n2[2];
		n2[0] = tempNorm[0]; n2[1] = tempNorm[1]; n2[2] = tempNorm[2];

		GzTextureIndex tempUV = { uv3[0], uv3[1] };
		uv3[0] = uv2[0]; uv3[1] = uv2[1];
		uv2[0] = tempUV[0]; uv2[1] = tempUV[1];
;	}

	/* Normalize normals */
	float normScale = sqrt(n1[0] * n1[0] + n1[1] * n1[1] + n1[2] * n1[2]);
	n1[0] /= normScale; n1[1] /= normScale; n1[2] /= normScale;

	normScale = sqrt(n2[0] * n2[0] + n2[1] * n2[1] + n2[2] * n2[2]);
	n2[0] /= normScale; n2[1] /= normScale; n2[2] /= normScale;

	normScale = sqrt(n3[0] * n3[0] + n3[1] * n3[1] + n3[2] * n3[2]);
	n3[0] /= normScale; n3[1] /= normScale; n3[2] /= normScale;

	/* LEE evaluation - clockwise convention for E1, E2, E3 */
	float E1[3] = { (v2[1] - v1[1]), (v1[0] - v2[0]), (v2[0] - v1[0])*v1[1] - (v2[1] - v1[1])*v1[0] };
	float E2[3] = { (v3[1] - v2[1]), (v2[0] - v3[0]), (v3[0] - v2[0])*v2[1] - (v3[1] - v2[1])*v2[0] };
	float E3[3] = { (v1[1] - v3[1]), (v3[0] - v1[0]), (v1[0] - v3[0])*v3[1] - (v1[1] - v3[1])*v3[0] };

	/* Create edge vectors for interpolating z */
	float eV1[3] = { v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
	float eV2[3] = { v3[0] - v2[0], v3[1] - v2[1], v3[2] - v2[2] };

	/* Calculate cross product of edge vectors to determine normal */
	float A = eV1[1] * eV2[2] - eV2[1] * eV1[2];
	float B = eV2[0] * eV1[2] - eV1[0] * eV2[2];
	float C = eV1[0] * eV2[1] - eV2[0] * eV1[1];
	float D = -(A*v1[0] + B*v1[1] + C*v1[2]);
	float z = 0.0f;

	/* Transform U,V coordinates to perspective space */
	float v1ZInt = v1[2] / (MAXINT - v1[2]);
	float v2ZInt = v2[2] / (MAXINT - v2[2]);
	float v3ZInt = v3[2] / (MAXINT - v3[2]);

	GzTextureIndex uv1s = { uv1[0] / (v1ZInt + 1), uv1[1] / (v1ZInt + 1) };
	GzTextureIndex uv2s = { uv2[0] / (v2ZInt + 1), uv2[1] / (v2ZInt + 1) };
	GzTextureIndex uv3s = { uv3[0] / (v3ZInt + 1), uv3[1] / (v3ZInt + 1) };

	/* Create edge vectors for interpolating Us */
	float uV1[3] = { v2[0] - v1[0], v2[1] - v1[1], uv2s[0] - uv1s[0] };
	float uV2[3] = { v3[0] - v2[0], v3[1] - v2[1], uv3s[0] - uv2s[0] };

	float uA = uV1[1] * uV2[2] - uV2[1] * uV1[2];
	float uB = uV2[0] * uV1[2] - uV1[0] * uV2[2];
	float uC = uV1[0] * uV2[1] - uV2[0] * uV1[1];
	float uD = -(uA*v1[0] + uB*v1[1] + uC*uv1s[0]);

	/* Create edge vectors for interpolating Vs */
	float vV1[3] = { v2[0] - v1[0], v2[1] - v1[1], uv2s[1] - uv1s[1] };
	float vV2[3] = { v3[0] - v2[0], v3[1] - v2[1], uv3s[1] - uv2s[1] };

	float vA = vV1[1] * vV2[2] - vV2[1] * vV1[2];
	float vB = vV2[0] * vV1[2] - vV1[0] * vV2[2];
	float vC = vV1[0] * vV2[1] - vV2[0] * vV1[1];
	float vD = -(vA*v1[0] + vB*v1[1] + vC*uv1s[1]);

	if (render->interp_mode == GZ_FLAT) {
		/* Only right edge renders edge pixels */
		for (int y = (int)yMin; y <= (int)yMax; y++) {
			for (int x = (int)xMin; x <= (int)xMax; x++) {
				if (((E1[0] * x + E1[1] * y + E1[2]) > 0 && (E2[0] * x + E2[1] * y + E2[2]) > 0 && (E3[0] * x + E3[1] * y + E3[2]) > 0)
					|| ((E1[0] * x + E1[1] * y + E1[2]) < 0 && (E2[0] * x + E2[1] * y + E2[2]) < 0 && (E3[0] * x + E3[1] * y + E3[2]) < 0)
					|| (E1[0] * x + E1[1] * y + E1[2]) == 0) {
					if (x >= 0 && x <= render->display->xres && y >= 0 && y <= render->display->yres) {
						z = (-(A*x) - (B*y) - D) / C;
						if (z < render->display->fbuf[x + (y*render->display->xres)].z)
							GzPutDisplay(render->display, x, y, ctoi(render->flatcolor[RED]),
								ctoi(render->flatcolor[GREEN]),
								ctoi(render->flatcolor[BLUE]), 1, (int)z);
					}
				}
			}
		}
	}
	else if (render->interp_mode == GZ_NORMALS) {

		GzCoord E = { 0, 0, -1 }, R, n;
		GzColor color{ 0,0,0 }, ambient;

		/* Create edge vectors for interpolating Nx */
		float nxV1[3] = { v2[0] - v1[0], v2[1] - v1[1], n2[X] - n1[X] };
		float nxV2[3] = { v3[0] - v2[0], v3[1] - v2[1], n3[X] - n2[X] };

		float A1 = nxV1[1] * nxV2[2] - nxV2[1] * nxV1[2];
		float B1 = nxV2[0] * nxV1[2] - nxV1[0] * nxV2[2];
		float C1 = nxV1[0] * nxV2[1] - nxV2[0] * nxV1[1];
		float D1 = -(A1*v1[0] + B1*v1[1] + C1*n1[X]);

		/* Create edge vectors for interpolating Ny */
		float nyV1[3] = { v2[0] - v1[0], v2[1] - v1[1], n2[Y] - n1[Y] };
		float nyV2[3] = { v3[0] - v2[0], v3[1] - v2[1], n3[Y] - n2[Y] };

		float A2 = nyV1[1] * nyV2[2] - nyV2[1] * nyV1[2];
		float B2 = nyV2[0] * nyV1[2] - nyV1[0] * nyV2[2];
		float C2 = nyV1[0] * nyV2[1] - nyV2[0] * nyV1[1];
		float D2 = -(A2*v1[0] + B2*v1[1] + C2*n1[Y]);

		/* Create edge vectors for interpolating Nz */
		float nzV1[3] = { v2[0] - v1[0], v2[1] - v1[1], n2[Z] - n1[Z] };
		float nzV2[3] = { v3[0] - v2[0], v3[1] - v2[1], n3[Z] - n2[Z] };

		float A3 = nzV1[1] * nzV2[2] - nzV2[1] * nzV1[2];
		float B3 = nzV2[0] * nzV1[2] - nzV1[0] * nzV2[2];
		float C3 = nzV1[0] * nzV2[1] - nzV2[0] * nzV1[1];
		float D3 = -(A3*v1[0] + B3*v1[1] + C3*n1[Z]);

		for (int y = (int)yMin; y <= (int)yMax; y++) {
			for (int x = (int)xMin; x <= (int)xMax; x++) {
				if (((E1[0] * x + E1[1] * y + E1[2]) > 0 && (E2[0] * x + E2[1] * y + E2[2]) > 0 && (E3[0] * x + E3[1] * y + E3[2]) > 0)
					|| ((E1[0] * x + E1[1] * y + E1[2]) < 0 && (E2[0] * x + E2[1] * y + E2[2]) < 0 && (E3[0] * x + E3[1] * y + E3[2]) < 0)
					|| (E1[0] * x + E1[1] * y + E1[2]) == 0) {
					if (x >= 0 && x <= render->display->xres && y >= 0 && y <= render->display->yres) {
						z = (-(A*x) - (B*y) - D) / C;						
						if (z < render->display->fbuf[x + (y*render->display->xres)].z) {
							
							if (render->tex_fun != NULL) {
								/* Interpolate U,V coordinates */
								float uInt = (-(uA*x) - (uB*y) - uD) / uC;
								float vInt = (-(vA*x) - (vB*y) - vD) / vC;

								/* Transform interpolated U,V coordinates back to affine space */
								float vZInt = z / (MAXINT - z);
								GzTextureIndex uv = { uInt * (vZInt + 1), vInt * (vZInt + 1) };

								/* Set Ka and Kd values based on texture function */
								(GzTexture(render->tex_fun))(uv[U], uv[V], render->Ka);
								(GzTexture(render->tex_fun))(uv[U], uv[V], render->Kd);
							}

							ambient[RED] = max(0, min((render->Ka[RED] * render->ambientlight.color[RED]), 1));
							ambient[GREEN] = max(0, min((render->Ka[GREEN] * render->ambientlight.color[GREEN]), 1));
							ambient[BLUE] = max(0, min((render->Ka[BLUE] * render->ambientlight.color[BLUE]), 1));

							n[X] = (-(A1*x) - (B1*y) - D1) / C1;
							n[Y] = (-(A2*x) - (B2*y) - D2) / C2;
							n[Z] = (-(A3*x) - (B3*y) - D3) / C3;

							/* Normalize normals */
							float normScale = sqrt(n[X] * n[X] + n[Y] * n[Y] + n[Z] * n[Z]);
							n[X] /= normScale; n[Y] /= normScale; n[Z] /= normScale;

							color[RED] = 0; color[GREEN] = 0; color[BLUE] = 0;

							for (int i = 0; i < render->numlights; i++) {
								float nDotL = ((n[X] * render->lights[i].direction[X])
									+ (n[Y] * render->lights[i].direction[Y])
									+ (n[Z] * render->lights[i].direction[Z]));

								float nDotE = ((n[X] * E[X]) + (n[Y] * E[Y]) + (n[Z] * E[Z]));

								if ((nDotL > 0 && nDotE < 0) || (nDotL < 0 && nDotE > 0)) {
									color[RED] += 0; color[GREEN] += 0; color[BLUE] += 0;
									continue;
								}
								else if (nDotL < 0 && nDotE < 0) {
									nDotL = -nDotL;
								}

								R[X] = (2 * nDotL * n[X]) - render->lights[i].direction[X];
								R[Y] = (2 * nDotL * n[Y]) - render->lights[i].direction[Y];
								R[Z] = (2 * nDotL * n[Z]) - render->lights[i].direction[Z];

								float rScale = sqrt(R[X] * R[X] + R[Y] * R[Y] + R[Z] * R[Z]);
								R[X] /= rScale; R[Y] /= rScale; R[Z] /= rScale;

								float rDotE = max(0, min((R[X] * E[X]) + (R[Y] * E[Y]) + (R[Z] * E[Z]), 1));

								color[RED] += (render->Ks[RED] * render->lights[i].color[RED] * pow(rDotE, render->spec))
									+ (render->Kd[RED] * render->lights[i].color[RED] * nDotL);

								color[GREEN] += (render->Ks[GREEN] * render->lights[i].color[GREEN] * pow(rDotE, render->spec))
									+ (render->Kd[GREEN] * render->lights[i].color[GREEN] * nDotL);

								color[BLUE] += (render->Ks[BLUE] * render->lights[i].color[BLUE] * pow(rDotE, render->spec))
									+ (render->Kd[BLUE] * render->lights[i].color[BLUE] * nDotL);
							}
							color[RED] = max(0, min(color[RED] + ambient[RED], 1));
							color[GREEN] = max(0, min(color[GREEN] + ambient[GREEN], 1));
							color[BLUE] = max(0, min(color[BLUE] + ambient[BLUE], 1));

							GzPutDisplay(render->display, x, y, ctoi(color[RED]),
								ctoi(color[GREEN]),
								ctoi(color[BLUE]), 1, (int)z);
						}
					}
				}
			}
		}
	}
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short ctoi(float color)		/* convert float color to GzIntensity short */
{
	return(short)((int)(color * ((1 << 12) - 1)));
}
