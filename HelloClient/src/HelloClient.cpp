//============================================================================
// Name        : HelloClient.cpp
// Author      : Mohamed Anis Bouriga
// Version     :
// Copyright   : All Rights Reserved!
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "TimeServiceProxy.h"
#include "TimeServiceClientHelper.h"
#include "Poco/RemotingNG/TCP/TransportFactory.h"
#include <iostream>


int main(int argc, char** argv)
{
    try
    {
        // Register TCP transport.
        Poco::RemotingNG::TCP::TransportFactory::registerFactory();

        // Get proxy for remote TimeService.
        std::string uri("remoting.tcp://localhost:9999/tcp/Sample.TimeService/TheTimeService");
        Sample::ITimeService::Ptr pTimeService = Sample::TimeServiceClientHelper::find(uri);

        // Invoke methods on remote object.
        std::string currentTime = pTimeService->currentTimeAsString();
        std::cout << "Current time: " << currentTime << std::endl;
    }
    catch (Poco::Exception& exc)
    {
        std::cerr << exc.displayText() << std::endl;
        return 1;
    }
    return 0;
}
