#include "renderer.h"
#include "my_rand.h"

renderer::renderer(int w, int h)
	:steps_(0)
{
	WIDTH = w;
	HEIGHT = h;
}

renderer::~renderer()
{

}

Vec3 renderer::raytrace()
{
	return Vec3(rand_.get(), rand_.get(), rand_.get() );
}

int renderer::update(const double *src, double *dest)
{
	#pragma omp parallel for
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			int index = 3 * (y * WIDTH + x);
			Vec3 color = raytrace();
			dest[index + 0] = src[index + 0] + color.x;
			dest[index + 1] = src[index + 1] + color.y;
			dest[index + 2] = src[index + 2] + color.z;
		}
	}

	return ++steps_;
}
