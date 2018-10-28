//============================================================================
// Name        : Hello.cpp
// Author      : Mohamed Anis Bouriga
// Version     :
// Copyright   : All Rights Reserved!
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

int main()
{
    using namespace boost::lambda;
    typedef std::istream_iterator<int> in;

    std::for_each(
        in(std::cin), in(), std::cout << (_1 * 3) << " " );
}
