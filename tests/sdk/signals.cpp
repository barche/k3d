#include <k3dsdk/signal_system.h>
#include <k3dsdk/signal_slots.h>

#include <iostream>
#include <set>
#include <stdexcept>
#include <string>

void on_signal(std::string Name)
{
  std::cerr << "Emitted signal with name " << Name << std::endl;
}

typedef boost::signals2::signal<void ()> signal_type;

int main(int argc, char* argv[])
{
	try
	{
    signal_type A;

    A.connect(boost::bind(on_signal, "A"));
    A();
	}
	catch(std::exception& e)
	{
		std::cerr << "caught exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

