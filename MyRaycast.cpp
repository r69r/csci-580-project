#include "StdAfx.h"
#include "MyRaycast.h"
#include "gz.h"
#include "rend.h"
#include "MyHelperFunction.h"

#define DISPARRAY(x,y) (x+(y*render->display->xres))

int GzInitRay(MyRay* ray, GzCoord startPoint, GzCoord endPoint) {
	if (ray == NULL) ray = new MyRay();
	coordCopy(ray->startPoint, startPoint);
	coordSub(ray->direction, endPoint, startPoint);
	coordNormalize(ray->direction);
	return GZ_SUCCESS;
}

float GzFindZWithXYCoord(MyRay* ray, GzCoord coord) {
	float dist = coord[0]- ray->startPoint[0];
	return ray->startPoint[2] + dist * ray->direction[2];
}

int GzFindFocalPoint(MyRaycast* raycast, float focalDistance) {
	MyRay* ray = raycast->ray;
	GzCoord focalPoint;
	coordCopy(focalPoint, ray->direction);
	coordMulti(focalPoint, focalDistance);
	coordPlus(focalPoint, ray->startPoint);
	coordCopy(raycast->focalPoint, focalPoint);
	return GZ_SUCCESS;
}

int GzFindNearestHit(MyRaycast* raycast, GzRender* render) {
	MyRay* ray = raycast->ray;
	GzCoord dir2d;
	coordCopy(ray->direction, dir2d);
	dir2d[2] = 0;
	coordNormalize(dir2d);
	if (dir2d[0] == 0 && dir2d[1] == 0)return GZ_FAILURE;
	GzCoord coord;
	coordCopy(coord, ray->startPoint);
	GzDisplay* disp = render->display;
	while (coord[0] > 0 && coord[0] < disp->xres && coord[1] > 0 && coord[1] < disp->yres) {
		int x = coord[0], y = coord[1];
		GzCoord rayhit;

		float z = GzFindZWithXYCoord(ray, coord);
		if (render->display->fbuf[DISPARRAY(x, y)].z <= z) {
			coordCopy(raycast->nearestHit, coord);
			raycast->nearestHit[2] = z;
			return GZ_SUCCESS;
		}
		coordPlus(coord, dir2d);
	}
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
