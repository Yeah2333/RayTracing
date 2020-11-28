//
// Created by wangzhiyong on 2020/11/27.
//

#ifndef RAYTRACING_SPHERE_H
#define RAYTRACING_SPHERE_H

#include <hittable.h>
#include <vec3.h>

class sphere : public hittable {
public:
    sphere(){};
    sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r),mat_ptr(m) {};

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;
};


/*!
 * t^2*b⋅b+2*t*b⋅(A−C)+(A−C)⋅(A−C)−r^2=0
 */
bool sphere::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0)
        return false;
    auto sqrtdelta = sqrt(discriminant);

    //Find the nearset root that lies in the acceptable range.
    auto root = (-half_b - sqrtdelta) / a; //one root
    if (root < t_min || root > t_max) {
        // one root is over range
        root = (-half_b + sqrtdelta) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    //save hit_record
    rec.t = root;
    rec.p = r.at(rec.t); // intersect point
    rec.normal = (rec.p - center) / radius;
    rec.mat_ptr = mat_ptr;

    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);

    return true;
}


#endif //RAYTRACING_SPHERE_H
