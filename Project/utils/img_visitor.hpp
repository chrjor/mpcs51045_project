//
// visitor.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#ifndef VISITOR_H
#define VISITOR_H
#include<type_traits>

#include "types.hpp"

// Visitor interface
template<typename T>
struct Visitor {
    virtual void visit(T&) const {}
};

// Visitor helper class (for variadic template, multiple batches/images at once)
template<typename T> struct VisitorHelper;
template<typename ...Ts>
struct VisitorHelper<typelist<Ts...>> : public Visitor<Ts>... {
    using Visitor<Ts>::visit...;
};

// Visitable interface
class Image;
struct VisitableImageProcessor {
    virtual void accept(Visitor<Image>& v) const {}
};

#endif