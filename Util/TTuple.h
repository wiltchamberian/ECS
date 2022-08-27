/*
std::tuple的取代品，
为何要重新实现tuple?因为std::tuple的内存布局和模板参数顺序是反的，c++标准并未规定这一点，
导致使用有些受限
另外std::tuple做了一些不必要的默认参数推导，这对于某些使用场景是不必要的
*/

#ifndef __SUN_UTIL_TTUPLE_H
#define __SUN_UTIL_TTUPLE_H

#include <type_traits>


namespace Sun {

#define CONSTEXPR 

   template<typename ... _Arg>
   class TTuple;

   template<>
   class TTuple<>
   {
   };

   template<typename _T>
   class TTuple<_T> {
   public:
       TTuple() {}
       template<typename _T2>
       CONSTEXPR TTuple() {}


       template<typename _T2>
       CONSTEXPR TTuple(const _T2& tr):ele_(tr){}
       //TTuple(const _T& tr) : ele_(tr)
       //{}

       template<int index>
       auto& get() {
           static_assert(false ,"index overflow!");
       }
       template<>
       auto& get<0>() {
           return ele_;
       }

       template<typename _Ele>
       _Ele& get() {
           static_assert(std::is_same_v<_T, _Ele>);
           return ele_;
       }

       // const version
       template<int index>
       const auto& get() const {
           static_assert(false, "index overflow!");
       }
       template<>
       const auto& get<0>() const {
           return ele_;
       }
       template<typename _Ele>
       const _T& get() const {
           static_assert(std::is_same_v<_T, _Ele>);
           return ele_;
       }
   protected:
       _T ele_;
   };

   template<typename _Head>
   class TTuple<_Head, _Head> :public TTuple<_Head>
   {
   public:
       CONSTEXPR TTuple() {}
       //TTuple(const _Head& head, const _Head& head2)
       //    :TTuple<_Head>(head)
       //    ,ele_(head2)
       //{
       //}
       template<typename _Head2>
       CONSTEXPR TTuple(const _Head2& head,const _Head2& head2)
           : TTuple<_Head2>(head)
           , ele_(head2) {}


       template<int index>
       auto& get() {
           static_assert(index == 0 || index == 1);
           if constexpr (index == 0) {
               return TTuple<_Head>::ele_;
           }
           else {
               return ele_;
           }
       }

       template<typename _Ele>
       _Ele& get() {
           return TTuple<_Head>::ele_;
       }

       //const version
       template<int index>
       const auto& get() const {
           static_assert(index == 0 || index == 1);
           if constexpr (index == 0) {
               return TTuple<_Head>::ele_;
           }
           else {
               return ele_;
           }
       }

       template<typename _Ele>
       const _Ele& get() const {
           return TTuple<_Head>::ele_;
       }
   protected:
       _Head ele_;
   };

   template<typename _Head, typename ... Arg>
   class TTuple<_Head, Arg...> :public TTuple<_Head>,public TTuple<Arg...>
   {
       using Up = TTuple<_Head>;
       using Down = TTuple<Arg...>;
   public:
       CONSTEXPR TTuple() {}
       //TTuple(const _Head& head, const Arg& ... tail)
       //    :Up(head)
       //    ,Down(tail...)
       //{ }

       template<typename _This, typename ... _Rest>
       CONSTEXPR TTuple(const _This& this_arg, const _Rest& ... rest)
       : Up(this_arg),Down(rest...)
       {
       }
       
       template<int index>
       auto& get() {
           return TTuple<Arg...>::template get<index-1>();
       }
       template<>
       auto& get<0>() {
           return TTuple<_Head>::ele_;
       }

       template<typename _Ele>
       _Ele& get() {
           if constexpr (std::is_same_v<_Ele, _Head>) {
               return TTuple<_Head>::ele_;
           }
           else {
               return TTuple<Arg...>::template get<_Ele>();
           }  
       }

       //const version
       template<int index>
       const auto& get() const {
           return TTuple<Arg...>::template get<index - 1>();
       }
       template<>
       const auto& get<0>() const {
           return TTuple<_Head>::ele_;
       }

       template<typename _Ele>
       const _Ele& get() const {
           if constexpr (std::is_same_v<_Ele, _Head>) {
               return TTuple<_Head>::ele_;
           }
           else {
               return TTuple<Arg...>::template get<_Ele>();
           }
       }
   };

#if _HAS_CXX17
   template <class... _Types>
   TTuple(_Types...)->TTuple<_Types...>;
#endif

}


#endif