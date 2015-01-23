#ifndef K3DSDK_STRING_CAST_H
#define K3DSDK_STRING_CAST_H

#include <k3dsdk/types.h>

#include <boost/filesystem/path.hpp>

#include <boost/format.hpp>

#include <limits>
#include <sstream>
#include <string>

namespace k3d
{

///////////////////////////////////////////////////////////////////////////
// string_cast

/// Converts any serializable type to a string
template<typename type>
const string_t string_cast(const type& RHS)
{
	std::ostringstream buffer;
	if(std::numeric_limits<type>::is_specialized)
	        buffer.precision(std::numeric_limits<type>::digits10 + 1);

	buffer << RHS;
	return buffer.str();
}

/// Specialization of string_cast for type bool_t
template<>
inline const string_t string_cast<bool_t>(const bool_t& RHS)
{
	return RHS ? "true" : "false";
}

/// Specialization of string_cast for strings
template<>
inline const string_t string_cast<string_t>(const string_t& RHS)
{
	return RHS;
}

/// Specialization of string_cast() for boost::filesystem::path
template<>
inline const string_t string_cast<boost::filesystem::path>(const boost::filesystem::path& RHS)
{
	return RHS.string();
}

/// Specialization of string_cast() for boost::format
template<>
inline const string_t string_cast<boost::format>(const boost::format& RHS)
{
	return RHS.str();
}

/////////////////////////////////////////////////////////////////////////////
// from_string

/// Converts a string into any serializeable type
template<typename type>
type from_string(const string_t& Value, const type& Default)
{
	type result = Default;
	std::istringstream stream(Value.c_str());
	stream >> result;

	return result;
}

/// Specialization of from_string for type bool_t
template<>
inline bool_t from_string(const string_t& Value, const bool_t& Default)
{
	bool_t result = Default;

	if(Value == "true")
		result = true;
	else if(Value == "false")
		result = false;

	return result;
}

/// Specialization of from_string for strings
template<>
inline string_t from_string(const string_t& Value, const string_t&)
{
	return Value;
}

/// Specialization of from_string for boost::filesystem::path
template<>
inline boost::filesystem::path from_string(const string_t& Value, const boost::filesystem::path&)
{
	return boost::filesystem::path(Value);
}

} // namespace k3d

#endif // !K3DSDK_STRING_CAST_H

