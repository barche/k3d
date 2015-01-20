#include <k3dsdk/system.h>
#include <k3dsdk/gzstream.h>

#include <sstream>

#include <boost/filesystem.hpp>

int main(int argc, char* argv[])
{
	try
	{
		const boost::filesystem::path test_file("gztest.gz");
		if(boost::filesystem::exists(test_file))
			boost::filesystem::remove(test_file);

		k3d::filesystem::ogzstream ofile(test_file);
		ofile.write("test", 4);
		ofile.close();

		k3d::filesystem::igzstream ifile(test_file);

		char read_buffer[5];
		ifile.read(read_buffer, 4);
		read_buffer[4] = '\0';
		if(std::string(read_buffer) != std::string("test"))
		{
			std::stringstream err;
			err << "Read file is not correct, obtained " << read_buffer << " instead of test.";
			throw std::runtime_error(err.str());
		}

		if(ifile.get() != std::char_traits<char>::eof())
			throw std::runtime_error("EOF not reached as expected");
	}
	catch(const std::runtime_error& err)
	{
		std::cerr << "Test failed with error: " << err.what() << std::endl;
		return 1;
	}

	return 0;
}

