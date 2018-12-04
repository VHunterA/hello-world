//============================================================================
// Name        : Hello.cpp
// Author      : Mohamed Anis Bouriga
// Version     :
// Copyright   : All Rights Reserved!
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/SQL/Session.h"
#include "Poco/SQL/SQLite/Connector.h"
#include <iostream>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;

using namespace Poco::SQL::Keywords;
using Poco::SQL::Session;
using Poco::SQL::Statement;

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

class TimeRequestHandler: public HTTPRequestHandler
{
public:
    TimeRequestHandler(const std::string& format): _format(format)
    {
    }

    void handleRequest(HTTPServerRequest& request,
                       HTTPServerResponse& response)
    {
        Application& app = Application::instance();
        app.logger().information("Request from "
            + request.clientAddress().toString());

        Timestamp now;
        std::string dt(DateTimeFormatter::format(now, _format));

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        std::ostream& ostr = response.send();
        ostr << "<html><head><title>HTTPTimeServer powered by "
                "POCO C++ Libraries</title>";
        ostr << "<meta http-equiv=\"refresh\" content=\"1\"></head>";
        ostr << "<body><p style=\"text-align: center; "
                "font-size: 48px;\">";
        ostr << dt;
        ostr << "</p></body></html>";
    }

private:
    std::string _format;
};

class TimeRequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
    TimeRequestHandlerFactory(const std::string& format):
        _format(format)
    {
    }

    HTTPRequestHandler* createRequestHandler(
        const HTTPServerRequest& request)
    {
        if (request.getURI() == "/")
            return new TimeRequestHandler(_format);
        else
            return 0;
    }

private:
    std::string _format;
};

class HTTPTimeServer: public Poco::Util::ServerApplication
{
public:
    HTTPTimeServer(): _helpRequested(false)
    {
    }

    ~HTTPTimeServer()
    {
    }

protected:
    void initialize(Application& self)
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet& options)
    {
        ServerApplication::defineOptions(options);

        options.addOption(
        Option("help", "h", "display argument help information")
            .required(false)
            .repeatable(false)
            .callback(OptionCallback<HTTPTimeServer>(
                this, &HTTPTimeServer::handleHelp)));
    }

    void handleHelp(const std::string& name,
                    const std::string& value)
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader(
            "A web server that serves the current date and time.");
        helpFormatter.format(std::cout);
        stopOptionsProcessing();
        _helpRequested = true;
    }

    int main(const std::vector<std::string>& args)
    {
        if (!_helpRequested)
        {
            unsigned short port = (unsigned short)
                config().getInt("HTTPTimeServer.port", 9980);
            std::string format(
                config().getString("HTTPTimeServer.format",
                                   DateTimeFormat::SORTABLE_FORMAT));

            ServerSocket svs(port);
            HTTPServer srv(new TimeRequestHandlerFactory(format),
                svs, new HTTPServerParams);
            srv.start();
            waitForTerminationRequest();
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};

struct Prime
{
	static std::vector<int> primesList;
    int         value;

    Prime():value(0){

    }

    Prime& operator++(){
    	++value;
    	return *this;
    }
    Prime operator++(int){
    	Prime p(*this);
    	++value;
    	return p;
    }
    bool isPrime(){
    	for(int a:primesList){
    		if( (a!=0) && (a!=1) && (value%a)==0 ){
    			return false;
    		}
    	}
    	return true;
    }

};

class KeyStrokeRunnable: public Poco::Runnable
{
	bool* runEncore;
	Poco::Mutex* mx;

	virtual void run()
	{
		std::cin.ignore();
		mx->lock();
		*runEncore = false;
		mx->unlock();
	}
public:
	KeyStrokeRunnable(bool* rE, Poco::Mutex* m):Poco::Runnable(){
		runEncore = rE;
		mx = m;
	}
};

std::vector<int> Prime::primesList;

//int main(int argc, char** argv)
//{
//    // register SQLite connector
//    Poco::SQL::SQLite::Connector::registerConnector();
//
//    // create a session
//    Session session("SQLite", "primes.db");
//
//    // create table if not exists
//    session << "CREATE TABLE IF NOT EXISTS Primes (prime INTEGER)", now;
//
//    Prime prime;
//
//    // a simple query
//    Statement selectAll(session);
//    selectAll << "SELECT prime FROM Primes",
//        into(prime.value),
//        range(0, 1); //  iterate over result set one row at a time
//
//    while (!selectAll.done())
//    {
//    	if(selectAll.execute()){
//        	Prime::primesList.push_back(prime.value);
//    	}
//    }
//
//    // a simple query
//    Statement selectMax(session);
//    selectMax << "SELECT MAX(prime) FROM Primes",
//        into(prime.value),
//        range(0, 1); //  iterate over result set one row at a time
//
//    while (!selectMax.done())
//    {
//    	selectMax.execute();
//    }
//
//
//    Statement insert(session);
//    insert << "INSERT INTO Primes VALUES(?)",
//        use(prime.value);
//
//    bool runEncore = true;
//    Poco::Mutex mx;
//
//    KeyStrokeRunnable runnable(&runEncore,&mx);
//    Poco::Thread thread;
//    thread.start(runnable);
//
//    while(true){
//    	++prime;
//    	mx.lock();
//    	if(!runEncore){
//        	mx.unlock();
//        	break;
//    	}
//    	mx.unlock();
//    	if(prime.isPrime()){
//    		Prime::primesList.push_back(prime.value);
//    		printf("prime number found: %d\n",prime.value);
//            insert.execute();
//    	}
//    }
//
//    thread.join();
//
////    HTTPTimeServer app;
////    return app.run(argc, argv);
//    return 0;
//}

int main(int argc, char* argv[])
{
    auto fileStream = std::make_shared<ostream>();

    // Open stream to output file.
    pplx::task<void> requestTask = fstream::open_ostream(U("results.html")).then([=](ostream outFile)
    {
        *fileStream = outFile;

        // Create http_client to send the request.
        http_client client(U("http://www.bing.com/"));

        // Build request URI and start the request.
        uri_builder builder(U("/search"));
        builder.append_query(U("q"), U("cpprestsdk github"));
        return client.request(methods::GET, builder.to_string());
    })

    // Handle response headers arriving.
    .then([=](http_response response)
    {
        printf("Received response status code:%u\n", response.status_code());

        // Write response body into the file.
        return response.body().read_to_end(fileStream->streambuf());
    })

    // Close the file stream.
    .then([=](size_t)
    {
        return fileStream->close();
    });

    // Wait for all the outstanding I/O to complete and handle any exceptions
    try
    {
        requestTask.wait();
    }
    catch (const std::exception &e)
    {
        printf("Error exception:%s\n", e.what());
    }

    return 0;
}
