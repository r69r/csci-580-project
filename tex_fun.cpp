/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

GzColor	*image = NULL;
int xs, ys;
int texReset = 1;
int bokehReset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{

	if (u < 0 || u > 1 || v < 0 || v > 1)
		return GZ_FAILURE;

	unsigned char pixel[3];
	unsigned char dummy;
	char foo[8];
	int i, j;
	FILE *fd;

	if (texReset) {          /* open and load texture file */
		fd = fopen("texture", "rb");
		if (fd == NULL) {
			fprintf(stderr, "texture file not found\n");
			exit(-1);
		}
		fscanf(fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor)*(xs + 1)*(ys + 1));
		if (image == NULL) {
			fprintf(stderr, "malloc for texture image failed\n");
			exit(-1);
		}

		for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
			fread(pixel, sizeof(pixel), 1, fd);
			image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		texReset = 0;          /* init is done */
		fclose(fd);
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds */
	/* determine texture cell corner values and perform bilinear interpolation */
	/* set color to interpolated GzColor value and return */

	u *= (xs - 1);
	v *= (ys - 1);

	int A[2] = { floorf(u), ceilf(v) };
	int B[2] = { ceilf(u), ceilf(v) };
	int C[2] = { ceilf(u), floorf(v) };
	int D[2] = { floorf(u), floorf(v) };

	GzColor a, b, c, d, p;
	a[RED] = image[A[U] + (xs * A[V])][RED];
	a[GREEN] = image[A[U] + (xs * A[V])][GREEN];
	a[BLUE] = image[A[U] + (xs * A[V])][BLUE];

	b[RED] = image[B[U] + (xs * B[V])][RED];
	b[GREEN] = image[B[U] + (xs * B[V])][GREEN];
	b[BLUE] = image[B[U] + (xs * B[V])][BLUE];

	c[RED] = image[C[U] + (xs * C[V])][RED];
	c[GREEN] = image[C[U] + (xs * C[V])][GREEN];
	c[BLUE] = image[C[U] + (xs * C[V])][BLUE];

	d[RED] = image[D[U] + (xs * D[V])][RED];
	d[GREEN] = image[D[U] + (xs * D[V])][GREEN];
	d[BLUE] = image[D[U] + (xs * D[V])][BLUE];

	float s = (u - A[U]) / (B[U] - A[U]);
	float t = (A[V] - v) / (A[V] - D[V]);

	p[RED] = (s * t * c[RED]) + ((1 - s) * t * d[RED]) + (s * (1 - t) * b[RED]) + ((1 - s) * (1 - t) * a[RED]);
	p[GREEN] = (s * t * c[GREEN]) + ((1 - s) * t * d[GREEN]) + (s * (1 - t) * b[GREEN]) + ((1 - s) * (1 - t) * a[GREEN]);
	p[BLUE] = (s * t * c[BLUE]) + ((1 - s) * t * d[BLUE]) + (s * (1 - t) * b[BLUE]) + ((1 - s) * (1 - t) * a[BLUE]);

	color[RED] = p[RED];
	color[GREEN] = p[GREEN];
	color[BLUE] = p[BLUE];
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	if (u < 0 || u > 1 || v < 0 || v > 1)
		return GZ_FAILURE;

	GzColor texLUT[11] = { {0,0,0}, { 0,0,1 }, { 0,1,0 }, { 0,1,1 }, { 1,0,0 }, { 1,0,1 }, { 1,1,0 }, { 1,1,1 },
	{ 0,0,0.69 }, { 0,0.69,0 }, { 0,0.69,0.69 } };

	float lutIndex = (u + v) * 5;

	int lowerBound = floorf(lutIndex);
	int upperBound = lowerBound + 1;

	float b = lutIndex - lowerBound;
	float a = 1 - b;

	color[RED] = a * texLUT[lowerBound][RED] + b * texLUT[upperBound][RED];
	color[GREEN] = a * texLUT[lowerBound][GREEN] + b * texLUT[upperBound][GREEN];
	color[BLUE] = a * texLUT[lowerBound][BLUE] + b * texLUT[upperBound][BLUE];
	
	return GZ_SUCCESS;
}

/* Bokeh texture function */
int bokehtex_fun(float u, float v, GzColor color)
{
	if (u < 0 || u > 1 || v < 0 || v > 1)
		return GZ_FAILURE;

	unsigned char pixel[3];
	unsigned char dummy;
	char foo[8];
	int i, j;
	FILE *fd;

	if (bokehReset) {          /* open and load texture file */
		fd = fopen("bokeh", "rb");
		if (fd == NULL) {
			fprintf(stderr, "texture file not found\n");
			exit(-1);
		}
		fscanf(fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor)*(xs + 1)*(ys + 1));
		if (image == NULL) {
			fprintf(stderr, "malloc for texture image failed\n");
			exit(-1);
		}

		for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
			fread(pixel, sizeof(pixel), 1, fd);
			image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		bokehReset = 0;          /* init is done */
		fclose(fd);
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds */
	/* determine texture cell corner values and perform bilinear interpolation */
	/* set color to interpolated GzColor value and return */

	u *= (xs - 1);
	v *= (ys - 1);

	int A[2] = { floorf(u), ceilf(v) };
	int B[2] = { ceilf(u), ceilf(v) };
	int C[2] = { ceilf(u), floorf(v) };
	int D[2] = { floorf(u), floorf(v) };

	GzColor a, b, c, d, p;
	a[RED] = image[A[U] + (xs * A[V])][RED];
	a[GREEN] = image[A[U] + (xs * A[V])][GREEN];
	a[BLUE] = image[A[U] + (xs * A[V])][BLUE];

	b[RED] = image[B[U] + (xs * B[V])][RED];
	b[GREEN] = image[B[U] + (xs * B[V])][GREEN];
	b[BLUE] = image[B[U] + (xs * B[V])][BLUE];

	c[RED] = image[C[U] + (xs * C[V])][RED];
	c[GREEN] = image[C[U] + (xs * C[V])][GREEN];
	c[BLUE] = image[C[U] + (xs * C[V])][BLUE];

	d[RED] = image[D[U] + (xs * D[V])][RED];
	d[GREEN] = image[D[U] + (xs * D[V])][GREEN];
	d[BLUE] = image[D[U] + (xs * D[V])][BLUE];
	
	float s = (u - A[U]) / (B[U] - A[U]);
	float t = (A[V] - v) / (A[V] - D[V]);

	if (B[U] == A[U]) s = 0;
	if (A[V] == D[V]) t = 0;

	p[RED] = (s * t * c[RED]) + ((1 - s) * t * d[RED]) + (s * (1 - t) * b[RED]) + ((1 - s) * (1 - t) * a[RED]);
	p[GREEN] = (s * t * c[GREEN]) + ((1 - s) * t * d[GREEN]) + (s * (1 - t) * b[GREEN]) + ((1 - s) * (1 - t) * a[GREEN]);
	p[BLUE] = (s * t * c[BLUE]) + ((1 - s) * t * d[BLUE]) + (s * (1 - t) * b[BLUE]) + ((1 - s) * (1 - t) * a[BLUE]);

	color[RED] = p[RED];
	color[GREEN] = p[GREEN];
	color[BLUE] = p[BLUE];
}

/* Free texture memory */
int GzFreeTexture()
{
	if (image != NULL)
		free(image);
	return GZ_SUCCESS;
}

