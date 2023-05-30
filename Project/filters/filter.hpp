//
// filter.hpp
// Christian Jordan
// MPSC 51045 - Final Project
//

// Filter helper function 
template<typename FilterType, typename T, typename... Args>
auto filter(T& img_obj, Args&&... args) {
    FilterType filter(std::forward<Args>(args)...);
    img_obj.accept(filter);
    return std::move(img_obj);
}

// Filter helper unique pointer overload
template<typename FilterType, typename T, typename... Args>
auto filter(std::unique_ptr<T>& img_obj, Args&&... args) {
    FilterType filter(std::forward<Args>(args)...);
    img_obj->accept(filter);
    return std::move(img_obj);
}