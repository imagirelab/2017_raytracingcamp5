#ifndef RT_STRUCT_H
#define RT_STRUCT_H

#include <cmath>
#include "my_rand.h"


class Vec3 {
	static my_rand s_rand;
public:
	double x, y, z;
	Vec3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

	inline Vec3& operator + (const Vec3 &v) { x += v.x; y += v.y; z += v.z; return *this; }
	inline Vec3& operator - (const Vec3 &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	inline Vec3& normalize() { double inv_l = (1.0 / length()); x *= inv_l; y *= inv_l; z *= inv_l;  return *this; }

	inline Vec3 operator + (const Vec3 &v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
	inline Vec3 operator - (const Vec3 &v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
	inline Vec3 operator * (const Vec3 &v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
	inline Vec3 operator * (double a) const { return Vec3(a * x, a * y, a * z); }
	inline Vec3 operator / (double a) const { return Vec3(x/ a, y / a, z / a); }
	inline Vec3 operator-= (const Vec3 &v) const { return Vec3(x - v.x, y - v.y, z - v.z); }

	inline Vec3 operator-() const { return Vec3(-x, -y, -z); }

	inline double length_sq() const { return dot(*this, *this); }
	inline double length() const { return sqrt(length_sq()); }
	inline Vec3 normalize() const { return (*this) * (1.0/ length()); }
	inline static double dot(const Vec3 &a, const Vec3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	inline static Vec3 cross(const Vec3 &a, const Vec3 &b) { return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

	Vec3 reflect(const Vec3& n) const {return *this - n * 2 * dot(*this, n);}
	bool refract(const Vec3& n, double ni_over_nt, Vec3& refracted) const {
		Vec3 uv = this->normalize();
		double dt = Vec3::dot(uv, n);
		double discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
		if (discriminant > 0) {
			refracted = (uv - n * dt)*ni_over_nt - n * sqrt(discriminant);
			return true;
		}
		else
			return false;
	}

	static Vec3 random_in_unit_disc()
	{
		do {
			Vec3 p = Vec3(s_rand.get(), s_rand.get(), 0) * 2.0 - Vec3(1, 1, 0);
			if (dot(p, p) < 1.0) return p;
		} while (true);
	}

	static Vec3 random_in_unit_sphere() 
	{
		do {
			Vec3 p = Vec3(s_rand.get(), s_rand.get(), s_rand.get()) * 2.0 - Vec3(1, 1, 1);
			if (dot(p, p) < 1.0) return p;
		} while (true);
	}


};

inline Vec3 operator*(double f, const Vec3 &v) {
	return v * f;
}

class Ray
{
private:
	Vec3 o_;
	Vec3 d_;
public:
	Ray() {}
	Ray(const Vec3& o, const Vec3& d) { o_ = o; d_ = d; }
	Vec3 origin() const { return o_; }
	Vec3 direction() const { return d_; }
	Vec3 get(double t) const { return o_ + d_ * t; }
};

class Material;

struct HitRecord
{
	double t;
	Vec3 p;
	Vec3 normal;
	Material *mat_ptr;
};

class Hitable {
public:
	virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const = 0;
};

class Sphere : public Hitable {
private:
	Vec3 center;
	double radius;
	Material *mat_ptr;
public:
	Sphere() {}
	Sphere(Vec3 cen, double r, Material *m) : center(cen), radius(r), mat_ptr(m) {};
	bool hit(const Ray& r, double tmin, double tmax, HitRecord& rec) const override {
		Vec3 oc = r.origin() - center;
		double a = Vec3::dot(r.direction(), r.direction());
		double b = Vec3::dot(oc, r.direction());
		double c = Vec3::dot(oc, oc) - radius*radius;
		double discriminant = b*b - a*c;
		if (discriminant > 0) {
			double temp = (-b - sqrt(discriminant)) / a;
			if (temp < tmax && temp > tmin) {
				rec.t = temp;
				rec.p = r.get(rec.t);
				rec.normal = (rec.p - center) / radius;
				rec.mat_ptr = mat_ptr;
				return true;
			}
			temp = (-b + sqrt(discriminant)) / a;
			if (temp < tmax && temp > tmin) {
				rec.t = temp;
				rec.p = r.get(rec.t);
				rec.normal = (rec.p - center) / radius;
				rec.mat_ptr = mat_ptr;
				return true;
			}
		}
		return false;
	}
};

class HitableList : public Hitable {
private:
	enum{
		LIST_MAX = 255,
	};
	Hitable *list[LIST_MAX];
	int n_;
public:
	HitableList() : n_(0) {}
	~HitableList() {
		for (int i = 0; i < n_; i++) {
			if (list[i]) delete list[i]; list[i] = nullptr;
		}
	}
	void Append(Hitable *l) { if (n_ < LIST_MAX) { list[n_++] = l; } }
	bool hit(const Ray &r, double tmin, double tmax, HitRecord& rec) const override
	{
		HitRecord temp_rec;
		bool hit_anything = false;
		double closest_so_far = tmax;
		for (int i = 0; i < n_; i++) {
			if (list[i]->hit(r, tmin, closest_so_far, temp_rec)) {
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}
		return hit_anything;
	}
};

class Material {
protected:
	static my_rand s_rand;
	static double schlick(double cosine, double ref_idx) {
		double r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0)*pow((1 - cosine), 5);
	}

public:
	virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const = 0;
};

class Lambertian : public Material {
private:
	Vec3 albedo;
public:
	Lambertian(const Vec3& a) : albedo(a) {}
	virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
		Vec3 target = rec.p + rec.normal + Vec3::random_in_unit_sphere();
		scattered = Ray(rec.p, target - rec.p);
		attenuation = albedo;
		return true;
	}
};

class Metal : public Material {
private:
	Vec3 albedo;
	float fuzz;
public:
	Metal(const Vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
	virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
		Vec3 reflected = r_in.direction().normalize().reflect(rec.normal);
		scattered = Ray(rec.p, reflected + Vec3::random_in_unit_sphere() * fuzz);
		attenuation = albedo;
		return (Vec3::dot(scattered.direction(), rec.normal) > 0);
	}
};

class Dielectric : public Material {
public:
	Dielectric(float ri) : ref_idx(ri) {}
	virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
		Vec3 outward_normal;
		Vec3 reflected = r_in.direction().reflect(rec.normal);
		double ni_over_nt;
		attenuation = Vec3(1.0, 1.0, 1.0);
		Vec3 refracted;
		double reflect_prob;
		double cosine;
		if (Vec3::dot(r_in.direction(), rec.normal) > 0) {
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
//         cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
			cosine = Vec3::dot(r_in.direction(), rec.normal) / r_in.direction().length();
			cosine = sqrt(1 - ref_idx*ref_idx*(1 - cosine*cosine));
		}
		else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -Vec3::dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		if (r_in.direction().refract(outward_normal, ni_over_nt, refracted))
			reflect_prob = schlick(cosine, ref_idx);
		else
			reflect_prob = 1.0;
		if (s_rand.get() < reflect_prob)
			scattered = Ray(rec.p, reflected);
		else
			scattered = Ray(rec.p, refracted);
		return true;
	}

	float ref_idx;
};



#endif // !RT_STRUCT_H