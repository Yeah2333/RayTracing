//
// Created by wangzhiyong on 2020/11/24.
//

#ifndef RAYTRACING_COLOR_H
#define RAYTRACING_COLOR_H

#include <vec3.h>

void write_color(std::ostream &out, color pixel_color) {
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color.x()) << " "
        << static_cast<int>(255.999 * pixel_color.y()) << " "
        << static_cast<int>(255.999 * pixel_color.z()) << "\n";
}

#endif //RAYTRACING_COLOR_H
