// 
// img.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#ifndef IMG_H
#define IMG_H

#include <algorithm>
#include <execution>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <memory>
#include <string>
#include <variant>

#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/bmp.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <ctre.hpp>

#include "../utils/img_visitor.hpp"

using std::string;
using std::unique_ptr;
using std::make_unique;
using std::vector;
using boost::gil::read_image;
using boost::gil::write_view;
using boost::gil::view;
using boost::gil::jpeg_tag;
using boost::gil::png_tag;
using boost::gil::rgb8_image_t;

namespace fs = std::filesystem;

// Enum for image tag type
enum class ImageTagType {
    JPEG,
    PNG,
    NONE
};

class Image : public Visitable {
public:
    virtual ~Image() = default;
    virtual void write_out() {}
};  

// Image base class
class ImageAccessor : public Image {
public:
    ImageAccessor(string path = "") {
        if (!path.empty()) {
            // Parse input path using CTRE
            if (auto [whole, pre, dir, post, ext] = ctre::match<"(.*)(\\/in\\/)(.*)(jpg|png|jpeg)">(path); whole) {
                if (!pre.to_view().empty() && !dir.to_view().empty()
                    && !post.to_view().empty() && !ext.to_view().empty()
                    ) {
                    img = make_unique<rgb8_image_t>();
                    if (ext.to_view() == "jpg" || ext.to_view() == "jpeg") {
                        read_image(std::string(path), *img, jpeg_tag{});
                        tag = ImageTagType::JPEG;
                    }
                    else if (ext.to_view() == "png") {
                        read_image(std::string(path), *img, png_tag{});
                        tag = ImageTagType::PNG;
                    }
                    out_path = pre.to_string() + "/out/" + post.to_string() +  ext.to_string();
                }
                else {
                    throw std::runtime_error("Invalid file path");
                }
            }   
        }   
    }

    virtual ~ImageAccessor() noexcept = default;

    ImageAccessor(const ImageAccessor& other) {
        img = make_unique<rgb8_image_t>(*other.img);
        out_path = other.out_path;
    }

    ImageAccessor& operator=(const ImageAccessor& other) {
        img = make_unique<rgb8_image_t>(*other.img);
        out_path = other.out_path;
        return *this;
    }

    ImageAccessor(ImageAccessor&& other) noexcept {
        img = std::move(other.img);
        out_path = other.out_path;
    }

    ImageAccessor& operator=(ImageAccessor&& other) noexcept {
        img = std::move(other.img);
        out_path = other.out_path;
        return *this;
    }

    void set_image(rgb8_image_t& new_img) { 
        img = make_unique<rgb8_image_t>(std::forward<rgb8_image_t>(new_img)); 
    }

    rgb8_image_t& get_image() { return *img; }

    void accept(VisitorFilter& v) override { v.visit(*this); }

    void write_out() { 
        if (tag == ImageTagType::JPEG) {
            write_view(out_path, const_view(*img), jpeg_tag{});
        }
        else if (tag == ImageTagType::PNG) {
            write_view(out_path, const_view(*img), png_tag{});
        }
    }
protected:
    unique_ptr<rgb8_image_t> img;
    ImageTagType tag;
    string out_path;
};


#endif