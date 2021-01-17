#pragma once

#include <algorithm>

 template<class T>
 constexpr const T& MyClamp(const T& value, const T& minimum, const T& maximum)
 {
	 return std::min(
		 std::max(
			 value,
			 minimum), 
		 maximum
	 );
 }