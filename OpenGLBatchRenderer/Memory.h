#pragma once

#include <memory>

template<class T>
using Ref = std::shared_ptr<T>;

template<class T>
using Scope = std::unique_ptr<T>;

template<class T, class ...Args>
constexpr Ref<T> make_ref(Args&& ... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<class T, class ...Args>
constexpr Scope<T> make_scope(Args&& ... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}
