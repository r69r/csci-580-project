#include "StdAfx.h"
#include "MyHelperFunction.h"
#include "gz.h"
#include <math.h>

void coordCopy(GzCoord to, GzCoord from) {
	for (int a = 0; a != 3; ++a) {
		to[a] = from[a];
	}
}

void coordPlus(GzCoord left, GzCoord right) {
	for (int a = 0; a != 3; ++a) {
		left[a] += right[a];
	}
}

void coordPlus(GzCoord res, GzCoord left, GzCoord right) {
	for (int a = 0; a != 3; ++a) {
		res[a] = left[a] + right[a];
	}
}

void coordSub(GzCoord left, GzCoord right) {
	for (int a = 0; a != 3; ++a) {
		left[a] -= right[a];
	}
}

void coordSub(GzCoord res, GzCoord left, GzCoord right) {
	for (int a = 0; a != 3; ++a) {
		res[a] = left[a] - right[a];
	}
}

void coordNormalize(GzCoord coord) {
	float dist = 0;
	for (int a = 0; a != 3; ++a) {
		dist += coord[a] * coord[a];
	}
	dist = sqrt(dist);
	for (int a = 0; a != 3; ++a) {
		coord[a] /= dist;
	}
}

void coordMulti(GzCoord coord, float times) {
	for (int a = 0; a != 3; ++a) {
		coord[a] *= times;
	}
}

float coordDot(GzCoord left, GzCoord right) {
	float res = 0;
	for (int a = 0; a != 3; ++a) {
		res += left[a] + right[a];
	}
	return res;
}
