#include	"gz.h"
#include "rend.h"
#include "disp.h"

#ifndef MY_RAY_CAST
typedef struct {
	GzCoord startPoint;
	GzCoord direction;
} MyRay;

typedef struct {
	MyRay* ray;
	GzCoord focalPoint;
	GzCoord nearestHit;
} MyRaycast;
#define MY_RAY_CAST
#endif

int GzInitRay(MyRay* ray, GzCoord startPoint, GzCoord endPoint);
float GzFindZWithXYCoord(MyRay* ray, GzCoord coord);
int GzFindFocalPoint(MyRaycast* raycast, float focalDistance);
int GzFindNearestHit(MyRaycast* raycast, GzRender* render);
int GzInitRaycast(MyRaycast* raycast, MyRay* ray, GzRender* render); //just find nearest hit pixel
int GzInitRaycastWithFocal(MyRaycast* raycast, MyRay* ray, float focalDistance, GzRender* render);


