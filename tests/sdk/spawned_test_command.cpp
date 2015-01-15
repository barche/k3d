#include <cstdlib>
#include <iostream>
#include <string>

/// Helper program that is spawned and writes something to the standard error and output streams
int main(int argc, char* argv[])
{
	const char* env_val = getenv("spawn_test_env");
	if(env_val)
	{
		std::cout << env_val;
		return 0;
	}

	if(argc != 3)
	{
		std::cerr << "Need exactly 2 arguments or spawn_test_env must be set!" << std::endl;
		return -1;
	}

	const std::string std_str = argv[1];
	const std::string err_str = argv[2];

	if(!std_str.empty())
		std::cout << std_str;

	if(!err_str.empty())
		std::cerr << err_str;

	return 0;
}

