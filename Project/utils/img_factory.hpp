//
// img_factory.cpp
// Christian Jordan
// MPSC 51045 - Final Project
//

// Adapted from advanced_factory.h
#ifndef IMG_FACTORY_H
#define IMG_FACTORY_H

#pragma warning(disable:4250)

#include <memory>
#include <type_traits>

#include "types.hpp"

using std::unique_ptr;
using std::add_const_t;


// Adapt signature (converts input T to const reference to T)
template<typename T> 
struct adapt_signature 
{
    using type = add_const_t<T> &;
};

// Adapt signature (specialization for reference to T)
template<typename T> 
struct adapt_signature<T &> 
{
    using type = T;
};

// Adapt signature (specialization for move reference to T)
template<typename T>
struct adapt_signature<T &&> 
{
    using type = T;
};

// Signature (matches function signature in list of function signatures)    
template<typename Result, typename ...Ts> 
struct Signature;

template<typename Result, typename ...Ts> 
struct Signature<Result, Result, Ts...> 
{
    using type = Result;
};

template<typename Result, typename ...Args, typename ...Ts> 
struct Signature<Result, Result*(Args...), Ts...> 
{
    using type = Result*(Args...);
};

template<typename Result, typename T, typename ...Ts> 
struct Signature<Result, T, Ts...> : public Signature<Result, Ts...> {};

// Abstract factory base classes
template<typename T>
struct abstract_creator 
{
    virtual unique_ptr<T> doCreate(Type2Type<T> &&) const = 0;
};

template<typename Result, typename ...Args>
struct abstract_creator<Result*(Args...)>
{
    virtual unique_ptr<Result> doCreate(Type2Type<Result>&&, typename adapt_signature<Args>::type...) const = 0;
};

// Abstract factory specialization
template<typename T> 
struct abstract_factory;

template<typename ...Ts>
struct abstract_factory<typelist<Ts...>> : public abstract_creator<Ts>... 
{
    template<class U, typename ...Args> 
    unique_ptr<U> create(Args&&... args) const 
    {
        abstract_creator<typename Signature<U, Ts...>::type> const& creator = *this;
        return creator.doCreate(Type2Type<U>(), std::forward<Args>(args)...);
    }
    virtual ~abstract_factory() {}
};

// Concrete factory specializations
template<typename AbstractFactory, typename Abstract, typename Concrete>
struct concrete_creator : virtual public AbstractFactory 
{
    unique_ptr<Abstract> doCreate(Type2Type<Abstract>&&) const 
    {
        return std::make_unique<Concrete>();
    }
};

template<typename AbstractFactory, typename Result, typename... Args, typename Concrete>
struct concrete_creator<AbstractFactory, Result*(Args...), Concrete> : virtual public AbstractFactory 
{
    unique_ptr<Result> doCreate(Type2Type<Result>&&, typename adapt_signature<Args>::type... args) const 
    {
        return std::make_unique<Concrete>(std::forward<adapt_signature<Args>::type>(args)...);
    }
};

template<typename AbstractFactory, typename... ConcreteTypes>
struct concrete_factory;

template<typename... AbstractTypes, typename... ConcreteTypes>
struct concrete_factory<abstract_factory<typelist<AbstractTypes...>>, typelist<ConcreteTypes...>>
        : public concrete_creator<abstract_factory<typelist<AbstractTypes...>>, AbstractTypes, ConcreteTypes>... {};

#endif