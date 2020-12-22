//
// Created by wangzhiyong on 2020/11/23.
// This code is output PPM format
//

#include <raytracing.h>
#include <color.h>
#include <hittable_list.h>
#include <sphere.h>
#include <camera.h>
#include <iostream>
#include <material.h>
#include <vector>
#include <thread>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

hittable_list random_scene(); //scene
color ray_color(const ray& r, const hittable& world, int depth) ; // a ray
void raytracing_thread(int k); // multi thread
void writeimage_stb(vector<vector<color>> image_data);

//const auto aspect_ratio = 21.0 / 9.0;  //image width/height
//const int image_width = 1440;
//const int image_height = static_cast<int>(image_width / aspect_ratio);
//const int samples_per_pixel = 500;  // one pixel samples numbers
//const int max_depth = 50; // raytracing depth

//TODO test output parameters
const auto aspect_ratio = 16.0 / 9.0;  //image width/height
const int image_width = 300;
const int image_height = static_cast<int>(image_width / aspect_ratio);
const int samples_per_pixel = 5;  // one pixel samples numbers
const int max_depth = 5; // raytracing depth

// World
auto world = random_scene();

// Camera
point3 lookfrom(13,2,3);
point3 lookat(0,0,0);
vec3 vup(0,1,0);
auto dist_to_focus = 10.0;
auto aperture = 0.1;

std::string output_path = "../output/image.png";
int thread_num = 1; //thread num, depends computer core.

camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

vector<vector<color>> image_file; // image data

int main() {

    // allocate image_file memory
    image_file.resize(image_width);

    for (auto &u : image_file) {
        u.resize(image_height);
    }

    // assignment threads
    std::vector<std::thread> threads;
    for (int k = 0; k < thread_num ; ++k) {
        threads.push_back(std::thread(raytracing_thread,k));
    }
    for (auto &u:threads) {
        u.join();
    }

    //TODO output image_file to ppm format
//    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
//    for (int j=image_height-1; j>=0;j--) {
//        for (int i = 0; i<image_width;i++) {
//            std::cout << image_file[i][j].x() << " "
//                << image_file[i][j].y()  << " "
//                << image_file[i][j].z()   << "\n";
//
//        }
//    }
    writeimage_stb(image_file);//write image to png format
    std::cerr << "\nDone.\n";
}

color ray_color(const ray& r, const hittable& world, int depth) {

    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        //random direction, rec.p + rec.normal is unit radius sphere origin, random_in_unit_sphere is a point in sphere
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);

//        //TODO add flag to change diffuse method
//        point3 target = rec.p + rec.normal + random_in_hemisphere(rec.normal);
//        //point3 target = rec.p + rec.normal + random_unit_vector(); // point on the sphere by unit vector in sphere
//        //point3 target = rec.p + rec.normal + random_in_unit_sphere);
//
//        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1); //origin is intersect point
//        // 0.5 : absorb 50% light

    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0 );
    return (1.0 - t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0),1000,ground_material));

    for (int a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; ++b) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4,0.2,0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    //diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5,1);
                    auto fuzz = random_double(0,0.5);
                    sphere_material = make_shared<metal>(albedo,fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }

        }

    }
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

void raytracing_thread(int k ){

    for (int j = image_height - k - 1 ; j >= 0; j -= thread_num ){
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);

            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }

            //write_color(std::cout,pixel_color,samples_per_pixel);
            write_image(image_file, i, j, pixel_color, samples_per_pixel);
        }

    }
}
void writeimage_stb(vector<vector<color>> image_data){
    unsigned char *data;
    data = (unsigned char*)malloc(image_width*image_height*3*sizeof(unsigned char));
    for (int j=image_height-1; j>=0;j--) {
        for (int i = 0; i<image_width;i++) {
            data[(image_height-j-1)*image_width*3+i*3] = (unsigned char)image_file[i][j].x();
            data[(image_height-j-1)*image_width*3+i*3+1] = (unsigned char)image_file[i][j].y();
            data[(image_height-j-1)*image_width*3+i*3+2] = (unsigned char)image_file[i][j].z();
//            std::cout << image_file[i][j].x() << " "
//                      << image_file[i][j].y()  << " "
//                      << image_file[i][j].z()   << "\n";

        }
    }
    stbi_write_png(output_path.c_str(),image_width,image_height,3,data,0);
}

