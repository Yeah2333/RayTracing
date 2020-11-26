//
// Created by wangzhiyong on 2020/11/23.
// Ray Tracing in one weekend
// This code is output PPM format
//
#include <iostream>
#include <vec3.h>
#include <color.h>
#include <ray.h>

double hit_sphere(const point3& center, double radius, const ray& r);

//drawing background
color ray_color(const ray &r) ;

int main() {
    // Image
    const auto aspect_ratio = 16.0 / 9.0;  //image width/height
    const int image_width = 1080;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // Camera

    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0); // plane origin
    auto horizontal = vec3(viewport_width, 0, 0); // horizontal vector ,(32/9 0 0)
    auto vertical = vec3(0, viewport_height, 0); // vertical vector ,(0 2 0)
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    // Render

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height - 1);
            //ray to image plane, origin is orgin , end point is in image plane
            ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }

    }
    std::cerr << "\nDone.\n";
}
/*!
 * t^2*b⋅b+2*t*b⋅(A−C)+(A−C)⋅(A−C)−r^2=0
 *
 * @param center sphere origin
 * @param radius sphere radius
 * @param r a ray
 * @return
 */
double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc,r.direction());
    auto c = oc.length_squared() - radius*radius;
    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0){
        return -1.0;
    } else {
        return (-half_b - sqrt(discriminant)) / a;
    }
}


color ray_color(const ray &r) {
    auto t = hit_sphere(point3(0,0,-1),0.5, r);

    if (t > 0){
        vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1)); //normal , p-origin
        return 0.5*color(N.x()+1, N.y()+1, N.z()+1);
    }

    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

