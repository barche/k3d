#include <k3dsdk/system.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <boost/assign/list_of.hpp>

void run_test(const std::string& StdArg, const std::string& ErrArg, const k3d::system::spawn_type SpawnType)
{
	const std::vector<std::string> args = boost::assign::list_of(StdArg)(ErrArg);
	std::string std_out, std_err;

	if(!k3d::system::spawn(k3d::filesystem::generic_path("./spawned-test-command"), args, SpawnType, std_out, std_err, k3d::filesystem::generic_path("../../bin")))
	{
		throw std::runtime_error("error spawning command");
	}

	if(std_out != StdArg)
	{
		std::stringstream err_msg;
		err_msg << "Wrong value for stdout: expected " << StdArg << " but obtained " << std_out;
		throw std::runtime_error(err_msg.str());
	}

	if(std_err != ErrArg)
	{
		std::stringstream err_msg;
		err_msg << "Wrong value for stderr: expected " << ErrArg << " but obtained " << std_err;
		throw std::runtime_error(err_msg.str());
	}
}

void run_test_env(const std::string& EnvValue)
{
	const std::vector<std::string> args;
	std::string std_out, std_err;

	k3d::system::environment_t env;
	env["spawn_test_env"] = EnvValue;

	if(!k3d::system::spawn(k3d::filesystem::generic_path("./spawned-test-command"), args, k3d::system::SPAWN_SYNCHRONOUS, std_out, std_err, k3d::filesystem::generic_path("../../bin"), env))
	{
		throw std::runtime_error("error spawning command");
	}

	if(std_out != EnvValue)
	{
		std::stringstream err_msg;
		err_msg << "Wrong value for env_value: expected " << EnvValue << " but obtained " << std_out;
		throw std::runtime_error(err_msg.str());
	}

	if(!std_err.empty())
	{
		std::stringstream err_msg;
		err_msg << "stderr not empty: " << std_err;
		throw std::runtime_error(err_msg.str());
	}
}

int main(int argc, char* argv[])
{
	try
	{
		// Test with both stderr and stdout
		run_test("standard with space", "error with space", k3d::system::SPAWN_SYNCHRONOUS);

		// Test with stdout only
		run_test("standard", "", k3d::system::SPAWN_SYNCHRONOUS);

		// Test with stderr only
		run_test("", "error", k3d::system::SPAWN_SYNCHRONOUS);

		// Test with environment
		run_test_env("my env var");
	}
	catch(const std::runtime_error& err)
	{
		std::cerr << "Test failed with error: " << err.what() << std::endl;
		return 1;
	}

	return 0;
}

