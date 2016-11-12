#include "StdAfx.h"
#include "MyRaycast.h"
#include "gz.h"
#include "rend.h"
#include "MyHelperFunction.h"

#define DISPARRAY(x,y) (x+(y*render->display->xres))

void clampWithRes(GzCoord coord, int xres, int yres) {
	if (coord[0] < 0)coord[0] = 0;
	if (coord[0] >= xres) coord[0] = xres - 1;
	if (coord[1] < 0)coord[1] = 0;
	if (coord[1] >= yres) coord[1] = yres - 1;
	if (coord[2] < 0)coord[2] = 0;
	if (coord[2] > 1) coord[2] = 1;
}

int GzInitRay(MyRay* ray, GzCoord startPoint, GzCoord endPoint) {
	if (ray == NULL) ray = new MyRay();
	coordCopy(ray->startPoint, startPoint);
	coordSub(ray->direction, endPoint, startPoint);
	coordNormalize(ray->direction);
	return GZ_SUCCESS;
}

float GzFindZWithXYCoord(MyRay* ray, GzCoord coord) {
	float dist = coord[0]- ray->startPoint[0];
	dist = dist < 0 ? -dist : dist;
	return ray->startPoint[2] + dist * ray->direction[2];
}

void GzChangeDirLengthXYtoOne(GzCoord dir) {
	float x = dir[0], y = dir[1], z = dir[2];
	dir[2] = 0;
	coordNormalize(dir);
	if (x != 0)
		dir[2] = z * (dir[0] / x);
	else
		dir[2] = z * (dir[1] / y);
}

int GzFindFocalPoint(MyRaycast* raycast, float focalDistance) {
	MyRay* ray = raycast->ray;
	coordCopy(raycast->focalPoint, ray->direction);
	coordMulti(raycast->focalPoint, focalDistance);
	coordPlus(raycast->focalPoint, ray->startPoint);
	return GZ_SUCCESS;
}

int GzFindNearestHit(MyRaycast* raycast, GzRender* render) {
	MyRay* ray = raycast->ray;
	GzCoord dir2d;
	coordCopy(dir2d, ray->direction);
	//GzChangeDirLengthXYtoOne(dir2d);
	if (dir2d[0] == 0 && dir2d[1] == 0)return GZ_FAILURE;
	GzCoord coord;
	coordCopy(coord, ray->startPoint);
	GzDisplay* disp = render->display;
	while (coord[0] > 0 && coord[0] < disp->xres && coord[1] > 0 && coord[1] < disp->yres) {
		int x = coord[0], y = coord[1];
		x = coord[0] - x > 0.5f ? x + 1 : x;
		y = coord[1] - y > 0.5f ? y + 1 : y;
		float z = coord[2];
		float pz = (float)render->display->fbuf[DISPARRAY(x, y)].z/ (float)INT_MAX;
		if (pz <= z) {
			coord[0] = x;
			coord[1] = y;
			coordCopy(raycast->nearestHit, coord);
			return GZ_SUCCESS;
		}
		coordPlus(coord, dir2d);
	}
	coord[2] = 1;
	clampWithRes(coord, render->display->xres, render->display->yres);
	coordCopy(raycast->nearestHit, coord);
	return GZ_SUCCESS;
}

int GzInitRaycastWithFocal(MyRaycast* raycast, MyRay* ray, float focalDistance, GzRender* render) {
	if (raycast == NULL || ray == NULL)return GZ_FAILURE;
	raycast->ray = ray;
	GzFindFocalPoint(raycast, focalDistance);
	GzFindNearestHit(raycast, render);
	return GZ_SUCCESS;
}

int GzInitRaycast(MyRaycast* raycast, MyRay* ray, GzRender* render) {
	if (raycast == NULL || ray == NULL)return GZ_FAILURE;
	raycast->ray = ray;
	GzFindNearestHit(raycast, render);
	return GZ_SUCCESS;
}
