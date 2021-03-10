#pragma once
#include <iostream>

namespace vengine
{
	template<class T>
	using Ref = std::shared_ptr<T>;

	template<class T>
	using Scope = std::unique_ptr<T>;
}
