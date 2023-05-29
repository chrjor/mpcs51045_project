//
// types.hpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#ifndef TYPES_H
#define TYPES_H

// Typelist
template <typename... Ts> struct typelist {};

// Type2Type pattern
template <typename T>
struct Type2Type 
{
    using type = T;
};

#endif