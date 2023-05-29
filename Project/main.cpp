//
// main.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#include <memory>

#include "img/batch.hpp"
#include "img/img.hpp"
#include "filters/batchfilters.hpp"
#include "filters/homography.hpp"
#include "utils/img_factory.hpp"
#include "utils/img_visitor.hpp"

using std::make_unique;
using std::unique_ptr;

using processor_factory = abstract_factory<VisitableImageProcessor>;   
using ImgProcessor = concrete_factory<abstract_factory<typename VisitableImageProcessor>, Image, ImageBatch>;

int main() {
    // Create image processor
    unique_ptr<ImgProcessor> img_processor = make_unique<ImgProcessor>();

    // Create batch
    auto batch = img_processor.create<ImageBatch>("data/file_list.txt");

    // Create filters

    // Apply filters

    return 0;
}