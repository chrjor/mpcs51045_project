// 
// batchfilters.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#ifndef BATCHFILTERS_H
#define BATCHFILTERS_H

#include <array>
#include <cmath>
#include <concepts>

#include <boost/gil.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/image_processing/convolve.hpp>
#include <boost/gil/rgb.hpp>
#include <boost/gil/typedefs.hpp>

#include "../img/img.hpp"
#include "../img/batch.hpp"
#include "../utils/img_visitor.hpp"

using boost::gil::rgb8_image_t;
using boost::gil::rgb8_pixel_t;
using boost::gil::rgb32f_pixel_t;
using boost::gil::const_view;
using boost::gil::view;
using boost::gil::copy_pixels;
using boost::gil::convolve_rows_fixed;
using boost::gil::convolve_cols_fixed;
using boost::gil::kernel_1d_fixed;
using boost::gil::color_converted_view;


// Gaussian filter
class GaussianFilter : public BatchableVisitorFilter {
public:
    // Gaussian kernel
    static constexpr float kernel[9] = {0.00022923296f, 0.0059770769f, 
        0.060597949f, 0.24173197f, 0.38292751f, 0.24173197f, 0.060597949f, 
        0.0059770769f, 0.00022923296f};

    // Apply the filter to an image
    void apply(ImageAccessor& img_obj) {
        // Get image
        rgb8_image_t convolved(img_obj.get_image());
        // Convolve
        kernel_1d_fixed<float, 9> kernel_(kernel, 4);
        convolve_rows_fixed<rgb32f_pixel_t>(const_view(convolved), kernel_, view(convolved));
        convolve_cols_fixed<rgb32f_pixel_t>(const_view(convolved), kernel_, view(convolved));
        // Move back
        img_obj.set_image(convolved);
    }

    void visit(ImageAccessor& img_obj) { apply(img_obj); }

    // This filter is independent of specific information about each image
    static bool is_independent() { return true; }
};

static_assert(BatchableFilter<GaussianFilter>, "GaussianFilter must satisfy BatchableFilter");

// Sobel filter
class SobelFilter : public BatchableVisitorFilter {
public:
    // Sobel kernels
    static constexpr float kernel_x[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    static constexpr float kernel_y[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

    // Apply the filter to an image
    void apply(ImageAccessor& img_obj) {
        // Get image
        rgb8_image_t convolved_x(img_obj.get_image());
        rgb8_image_t convolved_y(img_obj.get_image());
        // Convolve
        kernel_1d_fixed<float, 9> kernel_x_(kernel_x, 4);    
        kernel_1d_fixed<float, 9> kernel_y_(kernel_y, 4);
        convolve_rows_fixed<rgb32f_pixel_t>(const_view(convolved_x), kernel_x_, view(convolved_x));
        convolve_cols_fixed<rgb32f_pixel_t>(const_view(convolved_x), kernel_y_, view(convolved_x));
        convolve_rows_fixed<rgb32f_pixel_t>(const_view(convolved_y), kernel_x_, view(convolved_y));
        convolve_cols_fixed<rgb32f_pixel_t>(const_view(convolved_y), kernel_y_, view(convolved_y)); 
        // Calculate magnitude
        auto magnitude_image = convolved_x;
        auto view_x = const_view(convolved_x);
        auto view_y = const_view(convolved_y);
        auto view_magnitude = view(magnitude_image);
        for (int y = 0; y < img_obj.get_image().height(); y++) {
            for (int x = 0; x < img_obj.get_image().width(); x++) {
                rgb32f_pixel_t px_x = view_x(x, y);
                rgb32f_pixel_t px_y = view_y(x, y);
                rgb8_pixel_t& px_magnitude = view_magnitude(x, y);
                for (int i = 0; i < 3; i++) {
                    float magnitude = std::sqrt(px_x[i]*px_x[i] + px_y[i]*px_y[i]);
                    px_magnitude[i] = std::min(255.0f, std::max(0.0f, magnitude));
                }
            }
        }
        // Move back
        img_obj.set_image(magnitude_image);
    }

    void visit(ImageAccessor& img_obj) { apply(img_obj); }

    // This filter is independent of specific information about each image
    static bool is_independent() { return true; }
};

static_assert(BatchableFilter<SobelFilter>, "SobelFilter must satisfy BatchableFilter");

#endif