//
// visitor.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#ifndef VISITOR_H
#define VISITOR_H
#include<type_traits>

template <typename... Ts> struct typelist {};

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
template<typename T>
struct Visitable {
    virtual void accept(const Visitor<T>& v) const {
        v.visit(*static_cast<const T*>(this));
    }
};

#endif