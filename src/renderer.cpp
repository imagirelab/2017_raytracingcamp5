#include "renderer.h"

renderer::renderer(int w, int h)
	:steps_(0)
{
	WIDTH = w;
	HEIGHT = h;
}

renderer::~renderer()
{

}

int renderer::update(double *dest, const double *src)
{
	#pragma omp parallel for
	for (int i = 0; i < 3 * WIDTH * HEIGHT; i++) {
		dest[i] = src[i];
	}

	return ++steps_;
}
