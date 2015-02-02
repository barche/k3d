#include <k3dsdk/qtui/convert.h>

#include <stdexcept>

int main(int argc, char* argv[])
{
	try
	{
		const k3d::string_t str_in = "test string";

		boost::any any_str_in = str_in;

		QVariant var = k3d::convert<QVariant>(any_str_in);

		if(k3d::convert<k3d::string_t>(var.toString()) != str_in)
			throw std::runtime_error("Conversion to QVariant failed");

		boost::any any_str_out = k3d::convert<boost::any>(var);
		if(boost::any_cast<k3d::string_t>(any_str_out) != str_in)
			throw std::runtime_error("Conversion to boost::any failed");

		const boost::any i = 42;
		if(boost::any_cast<k3d::int32_t>(k3d::convert<boost::any>(k3d::convert<QVariant>(i))) != 42)
			throw std::runtime_error("Conversion of int32_t failed");
	}
	catch(const std::runtime_error& err)
	{
		std::cerr << "Test failed with error: " << err.what() << std::endl;
		return 1;
	}

	return 0;
}
