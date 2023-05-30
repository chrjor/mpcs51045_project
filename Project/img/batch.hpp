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
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "img.hpp"
#include "../utils/img_visitor.hpp"

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;


// Batch image base class
class ImageBatch : public BatchVisitable {
public:
    ImageBatch(string file_list = "") : batch(vector<unique_ptr<ImageAccessor>>{}) {
        if (!file_list.empty()) {
            std::ifstream file(file_list, std::ios::in);
            string line;
            while (std::getline(file, line)) {
                batch.push_back(make_unique<ImageAccessor>(line));
            }
        }
    }

    virtual ~ImageBatch() = default;

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

    void accept(BatchableVisitorFilter& v) override {
        run_accept(std::execution::seq, v);
    }

    template<typename ExecutionPolicy, typename Filter>
    requires BatchableFilter<Filter>
    void run_accept(ExecutionPolicy&& policy, Filter& visitor) {
        std::for_each(policy, batch.begin(), batch.end(), 
            [&visitor](unique_ptr<ImageAccessor>& img) {
                img->accept(visitor);
            }
        );
    }

    virtual void write_out() {
        run_write_out(std::execution::seq);
    }

    template<typename ExecutionPolicy>
    void run_write_out(ExecutionPolicy&& policy) {
        std::for_each(policy, batch.begin(), batch.end(), 
            [](unique_ptr<ImageAccessor>& img) {
                img->write_out();
            }
        );
    }

protected:
    vector<unique_ptr<ImageAccessor>> batch;
};

// Batch image class for batch image factory
class ImageBatchParallel : public ImageBatch {
public:
    ImageBatchParallel(string file_list = "") : ImageBatch(file_list) {}

    ~ImageBatchParallel() = default;

    void accept(BatchableVisitorFilter& v) override {
        run_accept(std::execution::par, v);
    }
    
    void write_out() override final {
        run_write_out(std::execution::par);
    }
};

#endif