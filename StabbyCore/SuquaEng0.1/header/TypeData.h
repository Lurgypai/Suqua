#pragma once

using type_id = unsigned int;

template<typename T>
struct TypeTag {
	static const type_id tag;
};

extern type_id increm();

template<typename T>
const type_id TypeTag<T>::tag = increm();