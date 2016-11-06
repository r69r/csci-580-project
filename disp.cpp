/*   CS580 HW1 display functions to be completed   */

#include	"stdafx.h"  
#include	"Gz.h"
#include	"disp.h"

#define DEFAULT_BG_RED		169
#define DEFAULT_BG_GREEN	169
#define DEFAULT_BG_BLUE		169


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- pass back pointer 
 */

	*framebuffer = new char[3 * width * height];

	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, int xRes, int yRes)
{
/* HW1.2 create a display:
  -- allocate memory for indicated resolution
  -- pass back pointer to GzDisplay object in display
*/

	if (xRes > MAXXRES || yRes > MAXYRES)
		return GZ_FAILURE;

	*display = new GzDisplay{ unsigned short(xRes), unsigned short(yRes), new GzPixel[sizeof(GzPixel) * xRes * yRes] };

	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* HW1.3 clean up, free memory */

	if (display != NULL)
		delete display;

	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes)
{
/* HW1.4 pass back values for a display */

	if (display == NULL)
		return GZ_FAILURE;

	*xRes = display->xres;
	*yRes = display->yres;

	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* HW1.5 set everything to some default values - start a new frame */

	if (display == NULL)
		return GZ_FAILURE;

/* Set default background color */
		
	for (int j = 0; j < display->yres; j++)
		for (int i = 0; i < display->xres; i++) {
			display->fbuf[ARRAY(i, j)].red = DEFAULT_BG_RED << 4;
			display->fbuf[ARRAY(i, j)].green = DEFAULT_BG_GREEN << 4;
			display->fbuf[ARRAY(i, j)].blue = DEFAULT_BG_BLUE << 4;
			display->fbuf[ARRAY(i, j)].alpha = 1;
			display->fbuf[ARRAY(i, j)].z = MAXINT;
		}

	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.6 write pixel values into the display */

	if (display == NULL)
		return GZ_FAILURE;

	if ((i >= 0 && i < display->xres) && (j >= 0 && j < display->yres)) {
		display->fbuf[ARRAY(i, j)].red = max(0, min(r, 4095));
		display->fbuf[ARRAY(i, j)].green = max(0, min(g, 4095));
		display->fbuf[ARRAY(i, j)].blue = max(0, min(b, 4095));
		display->fbuf[ARRAY(i, j)].alpha = max(0, min(a, 4095));
		display->fbuf[ARRAY(i, j)].z = z;
	}

	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.7 pass back a pixel value to the display */
	
	if (display == NULL)
		return GZ_FAILURE;

	*r = display->fbuf[ARRAY(i, j)].red;
	*g = display->fbuf[ARRAY(i, j)].green;
	*b = display->fbuf[ARRAY(i, j)].blue;
	*a = display->fbuf[ARRAY(i, j)].alpha;
	*z = display->fbuf[ARRAY(i, j)].z;

	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

/* HW1.8 write pixels to ppm file -- "P6 %d %d 255\r" */

	if (display == NULL)
		return GZ_FAILURE;

	unsigned char redVal;
	unsigned char greenVal;
	unsigned char blueVal;

	fprintf(outfile, "P6 %d %d 255\r", display->xres, display->yres);

	for (int j = 0; j < display->yres; j++)
		for (int i = 0; i < display->xres; i++) {
			redVal = unsigned char((display->fbuf[ARRAY(i, j)].red) >> 4);
			greenVal = unsigned char((display->fbuf[ARRAY(i, j)].green) >> 4);
			blueVal = unsigned char((display->fbuf[ARRAY(i, j)].blue) >> 4);

			fprintf(outfile, "%c%c%c", int(redVal), int(greenVal), int(blueVal));
		}

	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

/* HW1.9 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/

	if (display == NULL)
		return GZ_FAILURE;

	int fbIndex = 0;

	for (int j = 0; j < display->yres; j++)
		for (int i = 0; i < display->xres; i++) {
			framebuffer[fbIndex++] = unsigned char((display->fbuf[ARRAY(i, j)].blue) >> 4);;
			framebuffer[fbIndex++] = unsigned char((display->fbuf[ARRAY(i, j)].green) >> 4);
			framebuffer[fbIndex++] = unsigned char((display->fbuf[ARRAY(i, j)].red) >> 4);
		}

	return GZ_SUCCESS;
}