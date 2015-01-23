#include <k3dsdk/types.h>
#include <k3dsdk/path.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

int main(int argc, char* argv[])
{
	try
	{
		if(argc != 4)
			throw std::runtime_error("Usage: relative_path [ABSOLUTE PATH] [REFERENCE PATH] [EXPECTED RESULT]");

		const boost::filesystem::path absolute_path = boost::filesystem::path(k3d::string_t(argv[1]));
		const boost::filesystem::path reference_path = boost::filesystem::path(k3d::string_t(argv[2]));
		const boost::filesystem::path expected_result = boost::filesystem::path(k3d::string_t(argv[3]));

		const boost::filesystem::path result =  k3d::filesystem::make_relative_path(absolute_path, reference_path);
		if(result != expected_result)
			throw std::runtime_error("expected [" + expected_result.native() + "] got [" + result.native() + "]");
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

