#pragma once

#include <algorithm>

template<class T>
constexpr const T& MyMin(const T& a, const T& b)
{
	return (a < b) ? a : b;
}

template<class T>
constexpr const T& MyMax(const T& a, const T& b)
{
	return(a < b) ? b : a;
}

 template<class T>
 constexpr const T& MyClamp(const T& value, const T& minimum, const T& maximum)
 {
	 return MyMin(
		 MyMax(
			 value,
			 minimum
		 ), 
		 maximum
	 );
 }