//
// visitor.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

#ifndef VISITOR_H
#define VISITOR_H

#include<concepts>
#include<type_traits>


// Forward declaration
class ImageAccessor;

// Visitor interface
struct VisitorFilter {
    virtual ~VisitorFilter() = default;
    virtual void visit(ImageAccessor&) {}
};

template<typename T>
concept BatchableFilter = requires(T a, ImageAccessor& img) {
    { a.apply(img) } -> std::same_as<void>;
    { T::is_independent() } -> std::same_as<bool>;
};

struct BatchableVisitorFilter : public VisitorFilter {
    virtual ~BatchableVisitorFilter() = default;

    // Methods for batchable visitors
    virtual void apply(ImageAccessor& img) = 0;
    static bool is_independent() { return true; }
};

// Visitable interfaces
struct Visitable {
    virtual ~Visitable() = default;
    virtual void accept(VisitorFilter&) {}
};

struct BatchVisitable : public Visitable {
    virtual ~BatchVisitable() = default;
    virtual void accept(BatchableVisitorFilter&) {}
};

#endif