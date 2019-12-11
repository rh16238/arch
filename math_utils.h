#pragma once
#include <optional>
#include <iostream>
#include <vector>
typedef __int64_t int64_t;
typedef __uint64_t uint64_t;

template<class T>
auto operator<<(std::ostream& os, const T& t) -> decltype(t.print(os), os)
{
	t.print(os);
	return os;
}
