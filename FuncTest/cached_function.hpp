#ifndef CACHED_FUNCTION_H
#define CACHED_FUNCTION_H

// based on http://madebyevan.com/obscure-cpp-features/ but extended to not hardcode the return and parameter types
// It allows you to easily make a cache for any function, so that the function is never called twice for the same parameters
// instead of this:

/*
int fib(int n) {
if (n < 2) return n;
return fib(n - 1) + fib(n - 2);
}
*/

// write this to add a cache to all but the first call:

/*
int fib(int n) {
if (n < 2) return n;
return cached_function(fib, n - 1) + cached_function(fib, n - 2);
}
*/

// Note that C++17 may support this syntax: 


#include <map>
#include <tuple>

// http://stackoverflow.com/questions/28509273/get-types-of-function-parameters
#include <boost/function_types/result_type.hpp>

namespace CachedFunction {

	template <typename FuncType>
	using ResultType = typename boost::function_types::result_type<FuncType>::type;

	template <class FuncType, class SoleArgType>
	ResultType<FuncType> cached_function(FuncType functionPointer, SoleArgType argument) {
		static std::map< SoleArgType, ResultType<FuncType> > cache;
		auto cacheIterator = cache.find(argument);
		if (cacheIterator != cache.end()) return cacheIterator->second;
		return cache[argument] = functionPointer(argument);
	}
	
	template <class FuncType, class ... ArgTypes>
	ResultType<FuncType> cached_function(FuncType functionPointer, ArgTypes ... arguments) {
		static_assert(sizeof...(ArgTypes) >= 2, "Wrong overload used for cached_function template instantiation; expected at least 2 function arguments for specialization using tuple");
		// a tuple type to map all our function's parameters to the return type
		using ArgumentTupleType = std::tuple<ArgTypes...>;
		ArgumentTupleType parameters = std::make_tuple(arguments...);
		static std::map< ArgumentTupleType, ResultType<FuncType> > cache;
		auto cacheIterator = cache.find(parameters);
		if (cacheIterator != cache.end()) return cacheIterator->second;
		return cache[parameters] = functionPointer(arguments...);
	}

};

#endif //CACHED_FUNCTION_H