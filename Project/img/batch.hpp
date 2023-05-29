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
#include "visitor.hpp"

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;


// Batchable concept
template<typename T>
concept BatchableFilter = requires(T a, Image& img) {
    { a.apply(img) } -> std::same_as<void>;
    { T::is_independent() } -> std::same_as<bool>;
};

// Batch image class for batch image factory
class ImageBatch : public Visitable<ImageBatch> {
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

    template<typename T>
    requires BatchableFilter<Visitor<T>>
    void accept(Visitor<T>& v) {
        std::for_each(std::execution::par, batch.begin(), batch.end(), 
            [&v](unique_ptr<Image>& img) {
                img->accept(v);
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