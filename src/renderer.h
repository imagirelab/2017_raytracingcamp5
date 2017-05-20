#ifndef RENDERER_H
#define RENDERER_H

#include "my_rand.h"

class Vec3 {
public:
	double x, y, z;

	Vec3(double a, double b, double c) { x = a; y = b; z = c; }
};

class renderer{
private:
	int steps_;
	int WIDTH;
	int HEIGHT;
	my_rand rand_;

	Vec3 raytrace();
public:
	renderer(int w, int h);
	~renderer();
	
	int update(const double *src, double *dest);// ã…óÕëÅÇ≠î≤ÇØÇÈÇ±Ç∆
};

#endif // !RENDERER_H