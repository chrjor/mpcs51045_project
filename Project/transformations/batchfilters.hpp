// 
// batchfilters.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#ifndef BATCHFILTERS_H
#define BATCHFILTERS_H
#include <concepts>

#include <boost/gil.hpp>
#include <boost/gil/image_processing/convolve.hpp>
#include <boost/gil/rgb.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/algorithm.hpp>

#include "../img/visitor.hpp"
#include "../img/batch.hpp"

using boost::gil::rgb8_image_t;
using boost::gil::rgb32f_pixel_t;
using boost::gil::const_view;
using boost::gil::view;
using boost::gil::copy_pixels;
using boost::gil::convolve_rows;
using boost::gil::convolve_cols;
using boost::gil::kernel_1d_fixed;
using boost::gil::color_converted_view;
using boost::gil::rgb8_pixel_t;


// Gaussian filter
class GaussianFilter : public Visitor<ImageBatch> {
public:
    // Gaussian kernel
    static constexpr float kernel[] = {0.00022923296f, 0.0059770769f, 0.060597949f, 0.24173197f, 0.38292751f,
                                       0.24173197f, 0.060597949f, 0.0059770769f, 0.00022923296f};

    // Apply the filter to an image
    void apply(Image& img_obj) {
        rgb8_image_t& img_view = img_obj.get_image();
        rgb8_image_t img_copy(img_view.dimensions());
        // Convert to rgb32f
        auto img_view_32f = color_converted_view<rgb32f_pixel_t>(const_view(img_view));
        auto img_copy_32f = color_converted_view<rgb32f_pixel_t>(view(img_copy));
        kernel_1d_fixed<float, 9> kernel_(kernel, 4);
        // Convolve
        convolve_rows<rgb32f_pixel_t>(img_view_32f, kernel_, img_copy_32f);
        convolve_cols<rgb32f_pixel_t>(img_copy_32f, kernel_, img_copy_32f);
        // Convert back to rgb8
        copy_pixels(color_converted_view<rgb8_pixel_t>(img_copy_32f), view(img_view));
        // Move back to img_obj
        img_obj.set_image(img_copy);
    }

    void visit(Image& img_obj) { apply(img_obj); }

    // This filter is independent of specific information about each image
    static bool is_independent() { return true; }
};

static_assert(BatchableFilter<GaussianFilter>, "GaussianFilter must satisfy BatchableFilter");


// Sobel filter
class SobelFilter : public Visitor<ImageBatch> {
public:
    // Sobel kernels
    static constexpr float kernel_x[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    static constexpr float kernel_y[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

    // Apply the filter to an image
    void apply(Image& img_obj) {
        rgb8_image_t& img_view = img_obj.get_image();
        rgb8_image_t img_copy(img_view.dimensions());
        // Convert to rgb32f
        auto img_view_32f = color_converted_view<rgb32f_pixel_t>(const_view(img_view));
        auto img_copy_32f = color_converted_view<rgb32f_pixel_t>(view(img_copy));
        // Convolve
        convolve_rows<rgb32f_pixel_t>(img_view_32f, kernel_x, img_copy_32f);
        convolve_cols<rgb32f_pixel_t>(img_copy_32f, kernel_x, img_copy_32f);
        convolve_rows<rgb32f_pixel_t>(img_view_32f, kernel_y, img_copy_32f);
        convolve_cols<rgb32f_pixel_t>(img_copy_32f, kernel_y, img_copy_32f);
        // Convert back to rgb8
        copy_pixels(color_converted_view<rgb8_pixel_t>(img_copy_32f), view(img_view));
        // Move back to img_obj
        img_obj.set_image(img_copy);
    }

    void visit(Image& img_obj) { apply(img_obj); }

    // This filter is independent of specific information about each image
    static bool is_independent() { return true; }
};

static_assert(BatchableFilter<SobelFilter>, "SobelFilter must satisfy BatchableFilter");

#endif