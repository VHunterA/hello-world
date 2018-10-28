//============================================================================
// Name        : Hello.cpp
// Author      : Mohamed Anis Bouriga
// Version     :
// Copyright   : All Rights Reserved!
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

int main()
{
	boost::asio::io_context io;

	boost::asio::deadline_timer t(io, boost::posix_time::seconds(10));
	t.wait();

	std::cout << "!!!Hello World!!!" << std::endl;

	return 0;
}
