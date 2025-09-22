#include "EnumCodes.h"

constexpr inline bool operator==(Codes const& left, unsigned int const& right) {
	return static_cast<unsigned int>(left) == right;
}

constexpr inline bool operator==(unsigned int const& left, Codes const& right) {
	return static_cast<unsigned int>(right) == left;
}

constexpr inline bool operator==(ClientCodes const& left, unsigned int const& right) {
	return static_cast<unsigned int>(left) == right;
}

constexpr inline bool operator==(unsigned int const& left, ClientCodes const& right) {
	return static_cast<unsigned int>(right) == left;
}

