// 
// batch.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#ifndef BATCH_H
#define BATCH_H

#include <algorithm>
#include <concepts>
#include <execution>
#include <memory>
#include <string>
#include <vector>

#include "img.hpp"
#include "../filters/batchfilters.hpp"
#include "../utils/img_visitor.hpp"

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;


// Batch image class for batch image factory
class ImageBatch : public VisitableImageProcessor {
public:
    ImageBatch(string file_list) : batch(vector<unique_ptr<Image>>{}) {
        std::ifstream file(file_list, std::ios::in);
        string line;
        while (std::getline(file, line)) {
            batch.push_back(make_unique<Image>(line));
        }
    }

    ~ImageBatch() = default;

    ImageBatch(const ImageBatch& other) = delete;

    ImageBatch& operator=(const ImageBatch& other) = delete;

    ImageBatch(ImageBatch&& other) noexcept 
        : batch(std::move(other.batch)) {}

    ImageBatch& operator=(ImageBatch&& other) noexcept {
        if (this != &other) {
            batch = std::move(other.batch);
        }
        return *this;
    }

    template<template<typename Image> typename Filter>
    void accept(Filter<Image>& visitor) requires (BatchableFilter<Filter<Image>>) {
        std::for_each(std::execution::par, batch.begin(), batch.end(), 
            [&visitor](unique_ptr<Image>& img) {
                img->accept(visitor);
            }
        );
    }

    void write_out() {
        std::for_each(std::execution::par, batch.begin(), batch.end(), 
            [](unique_ptr<Image>& img) {
                img->write_out();
            }
        );
    }

private:
    vector<unique_ptr<Image>> batch;
};

#endif