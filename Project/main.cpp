//
// main.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#include <memory>

#include "utils/img_factory.hpp"
#include "img/img.hpp"
#include "img/batch.hpp"

using std::make_unique;
using std::unique_ptr;

using ImgProcessor = concrete_factory<abstract_factory<VisitableImageProcessor>, Image, ImageBatch>;

int main() {
    // Create image processor
    unique_ptr<ImgProcessor> img_processor(make_unique<ImgProcessor>());

    // Create batch
    //auto batch = img_processor->create<ImageBatch>("data/file_list.txt");

    // Create filters

    // Apply filters

    return 0;
}