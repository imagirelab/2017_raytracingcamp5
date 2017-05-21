#include <cfloat>
#include "renderer.h"

#define M_PI 3.141592653589793826433

my_rand Vec3::s_rand;
my_rand Material::s_rand;

void Camera::init(Vec3 from, Vec3 lookat, Vec3 up, double fov, double aspect, double aperture, double focus_dist)
{
	lens_radius_ = aperture / 2;
	double theta = fov * M_PI / 180;
	double half_height = tan(theta / 2);
	double half_width = aspect * half_height;
	origin_ = from;
	Vec3 d = from - lookat;
	w_ = d.normalize();
	u_ = Vec3::cross(up, w_).normalize();
	v_ = Vec3::cross(w_, u_);
	lower_left_corner_ = origin_ - u_ * (half_width * focus_dist) - v_ * (half_height * focus_dist) - w_ * focus_dist;
	horizontal_ = u_ * (2.0 * half_width * focus_dist);
	vertical_ = v_ * (2.0 * half_height * focus_dist);
}


renderer::renderer(int w, int h)
	:steps_(0)
{
	WIDTH = w;
	HEIGHT = h;

	// camera
	Vec3 from(13, 2, 3);
	Vec3 lookat(0, 0, 0);
	Vec3 up(0, 1, 0);
	double fov = 20.0;
	double aspect = (double)WIDTH / (double)HEIGHT;
	double dist_to_focus = 10.0;
	double aperture = 0.1;

	cam_.init(from, lookat, up, 20, aspect, aperture, dist_to_focus);

	// scene
	double R = cos(M_PI / 4);
	scene_.Append(new Sphere(Vec3(0, 0, -1), 0.5, new Lambertian(Vec3(0.1, 0.2, 0.5))));
	scene_.Append(new Sphere(Vec3(0, -100.5, -1), 100, new Lambertian(Vec3(0.8, 0.8, 0.0))));
	scene_.Append(new Sphere(Vec3(1, 0, -1), 0.5, new Metal(Vec3(0.8, 0.6, 0.2), 0.0)));
	scene_.Append(new Sphere(Vec3(-1, 0, -1), 0.5, new Dielectric(1.5)));
	scene_.Append(new Sphere(Vec3(-1, 0, -1), -0.45, new Dielectric(1.5)));
}

renderer::~renderer()
{
}

Vec3 renderer::raytrace(Ray r, int depth)
{
	// noise for debug
//	return Vec3(rand_.get(), rand_.get(), rand_.get());

	HitRecord rec;
	if (scene_.hit(r, 0.001, DBL_MAX, rec)) {
		Ray scattered;
		Vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * raytrace(scattered, depth + 1);
		}
		else {
			return Vec3(0, 0, 0);
		}
	}
	else {
		Vec3 unit_direction = r.direction().normalize();
		double t = 0.5*(unit_direction.y + 1.0);
		return (1.0 - t)*Vec3(1.0, 1.0, 1.0) + t * Vec3(0.5, 0.7, 1.0);
	}
}

Vec3 renderer::color(double u, double v)
{
	Ray r = cam_.get_ray(u, v);
	return raytrace(r, 0);
}

int renderer::update(const double *src, double *dest)
{
	const double INV_WIDTH = 1.0 / (double)WIDTH;
	const double INV_HEIGHT = 1.0 / (double)HEIGHT;

//	#pragma omp parallel for
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			int index = 3 * (y * WIDTH + x);

			double u = ((double)x + rand_.get()) * INV_WIDTH;
			double v = ((double)y + rand_.get()) * INV_HEIGHT;

			Vec3 color = this->color(u, v);
			dest[index + 0] = src[index + 0] + color.x;
			dest[index + 1] = src[index + 1] + color.y;
			dest[index + 2] = src[index + 2] + color.z;
		}
	}

	return ++steps_;
}
