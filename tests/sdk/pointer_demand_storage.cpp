#include <k3dsdk/data.h>
#include <k3dsdk/pointer_demand_storage.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/typed_array.h>
#include <k3dsdk/types.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

struct test_data
{
	k3d::string_t text = "initial_text";
};

struct property_owner : public k3d::property_collection
{
	property_owner() : m_data(init_owner(*this) + init_name("data") + init_label("Data") + init_description("Data"))
	{
		m_data.set_update_slot(boost::bind(&property_owner::update_value, this, _1, _2));
	}

	void update_value(const std::vector<k3d::ihint*>& hints, test_data& Data)
	{
		Data.text = "updated_text";
	}

	k3d_data(test_data*, immutable_name, change_signal, no_undo, pointer_demand_storage, no_constraint, read_only_property, no_serialization) m_data;
};

int main(int argc, char* argv[])
{
	try
	{
		property_owner props;
		props.m_data.reset(new test_data());

		if(props.m_data.pipeline_value()->text != "initial_text")
			throw std::runtime_error("initial text does not match");

		boost::signals2::signal<void(k3d::ihint*)> changed_signal;
		changed_signal.connect(k3d::hint::converter<
		k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(props.m_data.make_slot()));
		changed_signal(nullptr);

		if(props.m_data.pipeline_value()->text != "updated_text")
			throw std::runtime_error("updated text does not match");
	}
	catch(std::exception& e)
	{
		std::cerr << "uncaught exception: " << e.what() << std::endl;
		return 1;
	}
	catch(...)
	{
		std::cerr << "unknown exception" << std::endl;
	}
	
	return 0;
}

