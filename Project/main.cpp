//
// main.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#include <memory>
#include <string>
#include <iostream>

#include "utils/img_factory.hpp"
#include "img/img.hpp"
#include "img/batch.hpp"
#include "filters/batchfilters.hpp"
#include "filters/filter.hpp"
#include "filters/homography.hpp"

using std::make_unique;
using std::string;
using std::unique_ptr;

using ImageBases = abstract_factory<Image(string), ImageBatch(string)>;    
using ParallelImageProcessors = concrete_factory<ImageBases, ImageAccessor, ImageBatchParallel>;


int main() {
    // Create image processor
    unique_ptr<ParallelImageProcessors> process(make_unique<ParallelImageProcessors>());

    // Create batch
    string batch_file = "data/test_batch.txt";
    auto batch = process->create<ImageBatch>(batch_file);

    // Apply batch filters
    batch = filter<SobelFilter>(batch);
    batch = filter<GaussianFilter>(batch);
    // matrix3x3 m = {{
    //     {5.202, -0.086, -7.758},
    //     {-0.003, 5.898, -8.176},
    //     {0.016, 1.318, 1.0}
    // }};
    // batch = filter<HomographyFilter>(batch, m); // Should not compile

    // Create image
    // string img_file = "data/in/img_1.jpg";
    // auto img = process->create<Image>(img_file);

    // // Apply image filters
    // matrix3x3 m = {{
    //      {5.202, -0.086, -7.758},
    //      {-0.003, 5.898, -8.176},
    //      {0.016, 1.318, 1.0}
    // }};
    // img = filter<HomographyFilter>(img, m);

    // Write out
    batch->write_out();
    //img->write_out();

    return 0;
}