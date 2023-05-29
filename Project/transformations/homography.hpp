// 
// homography.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#ifndef HOMOGRAPHY_H
#define HOMOGRAPHY_H
#include <algorithm>
#include <cmath>
#include <thread>
#include <vector>   
#include <array>

#include <boost/gil.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>

#include "../img/img.hpp"

using std::lerp;
using std::round;
using boost::gil::rgb8_image_t;
using boost::gil::rgb8_pixel_t;
using boost::gil::point_t;
using boost::gil::pixel;
using boost::gil::view;
using boost::gil::nth_channel_view;

using matrix3x3 = std::array<std::array<double, 3>, 3>;


// Helper function to apply a matrix transformation to a point
point_t point_transform(const matrix3x3& m, const point_t& p) {
    double transformedX = m[0][0] * p.x + m[0][1] * p.y + m[0][2];
    double transformedY = m[1][0] * p.x + m[1][1] * p.y + m[1][2];
    double transformedZ = m[2][0] * p.x + m[2][1] * p.y + m[2][2];

    double normalizedX = transformedX / transformedZ;
    double normalizedY = transformedY / transformedZ;

    return point_t(normalizedX, normalizedY);
}

// Apply bilinear interpolation using the four nearest pixels
pixel<unsigned char, boost::gil::rgb_layout_t> 
bilinear_interpolate(rgb8_image_t& img, const point_t& p, const matrix3x3& m) {

    // Transform the point, and check if it is out of bounds
    point_t transformed_p = point_transform(m, p);
    if (transformed_p.x < 0 || transformed_p.x >= img.width() ||
        transformed_p.y < 0 || transformed_p.y >= img.height()) {
        return pixel<unsigned char, boost::gil::rgb_layout_t>(0, 0, 0);
    }

    // Get the four nearest pixels
    auto x1 = std::floor(transformed_p.x);
    auto y1 = std::floor(transformed_p.y);
    auto x2 = std::ceil(transformed_p.x);
    auto y2 = std::ceil(transformed_p.y);

    // Get the distance from the nearest pixels
    auto dx = transformed_p.x - x1;
    auto dy = transformed_p.y - y1;

    // Get the pixel values
    auto img_view = boost::gil::view(img);
    auto p1 = img_view(static_cast<int>(x1), static_cast<int>(y1));
    auto p2 = img_view(static_cast<int>(x2), static_cast<int>(y1));
    auto p3 = img_view(static_cast<int>(x1), static_cast<int>(y2));
    auto p4 = img_view(static_cast<int>(x2), static_cast<int>(y2));
    auto r = static_cast<int>(round(lerp(lerp(p1[0], p2[0], dx), lerp(p3[0], p4[0], dx), dy)));
    auto g = static_cast<int>(round(lerp(lerp(p1[1], p2[1], dx), lerp(p3[1], p4[1], dx), dy)));
    auto b = static_cast<int>(round(lerp(lerp(p1[2], p2[2], dx), lerp(p3[2], p4[2], dx), dy)));

    return pixel<unsigned char, boost::gil::rgb_layout_t>(r, g, b);
}

// Apply homography to an image
class homography : public Visitor<Image> {
public:
    homography(const matrix3x3& m) : m(m) {}

    void visit(Image& img) {
        rgb8_image_t out = new_out(img.get_image(), m);
        parallel_apply_homography(img.get_image(), m, out);
        img.set_image(out);
    }

private:
    // Create a new image with the correct dimensions
    rgb8_image_t new_out(rgb8_image_t& img, const matrix3x3& m) {
        // Get the corners of the image
        std::array<point_t, 4> corners = {
            point_t(0, 0),
            point_t(img.width(), 0),
            point_t(0, img.height()),
            point_t(img.width(), img.height())
        };

        // Transform the corners
        auto transformed_corners = corners;
        std::transform(corners.begin(), corners.end(), transformed_corners.begin(),
                    [&m](const auto& p) {
                        return point_transform(m, p);
                     });

        // Get the min and max of the transformed corners
        auto [min_e, max_e] = std::minmax_element(
            transformed_corners.begin(), transformed_corners.end(),
            [](const auto& a, const auto& b) {
                return a.x * a.y < b.x * b.y;
            }
        );

        // Get the width and height and return new image
        int width = max_e->x - min_e->x;
        int height = max_e->y - min_e->y;
        rgb8_image_t out(width, height);
        return out;
    }

    // Apply the homography in parallel using jthread
    void parallel_apply_homography(const rgb8_image_t& img, const matrix3x3& m, rgb8_image_t& out) {
        // Get the number of threads and the chunk size
        int num_threads = std::thread::hardware_concurrency();
        int chunk_size = out.height() / num_threads;

        std::vector<std::jthread> threads;
        // Create the threads
        for (int i = 0; i < num_threads; ++i) {
            int start = i * chunk_size;
            int end = (i + 1) * chunk_size;
            if (i == num_threads - 1) {
                end = out.height();
            }
            // Apply the homography to the chunk
            threads.emplace_back([this, &img, &m, &out, start, end]() {
                rgb8_image_t img_cpy = img;
                for (int y = start; y < end; ++y) {
                    for (int x = 0; x < out.width(); ++x) {
                        auto pixel = bilinear_interpolate(img_cpy, point_t{x, y}, m);
                        nth_channel_view(view(out), 0)(x, y) = pixel[0];
                        nth_channel_view(view(out), 1)(x, y) = pixel[1];
                        nth_channel_view(view(out), 2)(x, y) = pixel[2];

                    }
                }
            });
        }
    }

    matrix3x3 m;
};

#endif