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
#include <memory>
#include <string>

#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/bmp.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <ctre.hpp>

#include "visitor.hpp"

using std::string;
using std::unique_ptr;
using std::make_unique;
using std::vector;
using boost::gil::read_image;
using boost::gil::write_view;
using boost::gil::view;
using boost::gil::jpeg_tag;
using boost::gil::png_tag;
using boost::gil::bmp_tag;
using boost::gil::rgb8_image_t;


// Image classes for concrete image factory
class Image : public Visitable<Image> {
public:
    Image(std::string_view path) {
        if (auto [whole, out, file, ext] = ctre::match<"(.*)(\\.(jpg|png|jpeg|bmp))">(path); whole) {
            
            if (!out.to_view().empty() && !file.to_view().empty() && !ext.to_view().empty()) {
                img = make_unique<rgb8_image_t>();

                if (ext.to_view() == "jpg" || ext.to_view() == "jpeg") {
                    read_image(std::string(path), *img, jpeg_tag{});
                }
                else if (ext.to_view() == "png") {
                    read_image(std::string(path), *img, png_tag{});
                }
                else if (ext.to_view() == "bmp") {
                    read_image(std::string(path), *img, bmp_tag{});
                }
                out_dir = out.to_string();
                file_name = file.to_string();
            }
            else {
                throw std::runtime_error("Invalid file path");
            }
        }   
    }

    ~Image() noexcept = default;

    Image(const Image& other) {
        img = make_unique<rgb8_image_t>(*other.img);
        out_dir = other.out_dir;
        file_name = other.file_name;
    }

    Image& operator=(const Image& other) {
        img = make_unique<rgb8_image_t>(*other.img);
        out_dir = other.out_dir;
        file_name = other.file_name;
        return *this;
    }

    Image(Image&& other) noexcept {
        img = std::move(other.img);
        out_dir = other.out_dir;
        file_name = other.file_name;
    }

    Image& operator=(Image&& other) noexcept {
        img = std::move(other.img);
        out_dir = other.out_dir;
        file_name = other.file_name;
        return *this;
    }
    
    template<typename Visitor>
    void accept(Visitor& v) { v.visit(*this); }

    void write_out() { write_view(out_dir + "_out" + file_name, view(*img), jpeg_tag{}); }

    rgb8_image_t& get_image() { return *img; }

    void set_image(rgb8_image_t& new_img) { 
        img = make_unique<rgb8_image_t>(std::forward<rgb8_image_t>(new_img)); 
    }

private:
    unique_ptr<rgb8_image_t> img;
    string out_dir;
    string file_name;
};

#endif