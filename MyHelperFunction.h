#ifndef MY_HELPER_FUNCTION_H
#define MY_HELPER_FUNCTION_H
#endif
#include "gz.h"
#include <math.h>

void coordCopy(GzCoord to, GzCoord from);

void coordPlus(GzCoord left, GzCoord right);

void coordPlus(GzCoord res, GzCoord left, GzCoord right);


void coordSub(GzCoord left, GzCoord right);

void coordSub(GzCoord res, GzCoord left, GzCoord right);

void coordNormalize(GzCoord coord);

void coordMulti(GzCoord coord, float times);
float coordDot(GzCoord left, GzCoord right);