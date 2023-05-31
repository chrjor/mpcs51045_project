MPCS51045 Final Project 

## Project Description
The project aimed to create a simple image processing application that allows users to apply several filters to images of different types. It relies on Boost GIL library to handle image IO processing but implements its own filters. The application is written in C++ and uses CMake to build. 

The central goal was to implement many of the concepts and C++ features we learned in class, including: concepts, ranges, type traits, generics, and OOP.

## Directory Structure
The project is organized as follows:

- **Project** is the root directory of the project. It includes the main program file (main.cpp) and a CMakeLists.txt file for building the project.

    - **data:** This is the directory where the project's data files are stored. It includes a subdirectory called 'in' for the input image files and 'out' for the resulting processed images. Additionally, there is a 'test_batch.txt' file that contains a file list for input into a batch image processor.

    - **filters:** This directory contains headers for the image processing filters applied in the project. Each filter has its dedicated header file. The 'batchfilters.hpp' houses filters applicable to multiple images without user-specific input. 'homography.hpp' requires a specific transformation matrix from the user.
 
    - **img:** This directory includes image handling files such as 'img.hpp' and 'batch.hpp'. These contain classes and functions for reading and writing images. The supported types are jpeg and png.

    - **utils:** This directory contains utility files, like 'img_factory.hpp' for creating Image objects and 'img_visitor.hpp' for applying operations on these images.


## Build Instructions
To build the project, run the following commands from the root directory of the project:

```
mkdir build
cd build
cmake ..
make
```

## Run Instructions
To run the project, run the following commands from the root directory of the project:

```
./build/MPCS51045_Project
```


## Project Analysis
Some of the more challenging aspects of the project included adapting the abstract factory pattern to the task, implementing concepts and ranges, and handling the image processing using Boost GIL with various jpeg and png libraries. I had initially tried to create a custom abstract factory, but ran into some issues with the implementation and ended up selecting a simpler one from our class notes.

I also had sought out to create an Image class that could handle range based filter combinations using a pipeline of filters but the complexity involved with IO from Boost GIL, differences in kernel sizes, and generally the 2D layout of the images prevented me from finishing the implementation. 

Another consideration made concerned the object for image batches `ImageBatch`. I thought an interesting implementation choice could be using template variadics and type traits to create a list of different images that could be processed by iterating down either an inheritance hierarchy or fold expression. I ended up choosing to make them parallelizable instead (by passing an execution policy to the std::for_each) as it seemed more useful for the project and worked with my implementation of the visitor pattern.

The homography filter currently may not work as intended but it utilizes linear interpolation from C++20 and jthreads to fan out the work of applying the transformation matrix to the image in chunks.

Some of the key points in the project are:
  - Image filters utilize the visitor pattern to apply operations on images
  - The visitor methods use a BatchableFilter concept which creates constraints on the types of filters that can be applied to a batch of images
  - Image batches are objects that can be processed in parallel
  - The CTRE library is used to process file names and select the appropriate Boost GIL interface for reading and writing images

## Summary
The project provided an excellent opportunity to apply and integrate various C++ features and concepts learned throughout the course.

Future work could improve on some aspects, such as the homography filter's functionality and further exploiting parallel processing for image batches.