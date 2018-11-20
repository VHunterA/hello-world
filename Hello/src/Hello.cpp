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

struct Person
{
    std::string name;
    std::string address;
    int         age;
};

int main(int argc, char** argv)
{
    // register SQLite connector
    Poco::SQL::SQLite::Connector::registerConnector();

    // create a session
    Session session("SQLite", "sample.db");

    // drop sample table, if it exists
    session << "DROP TABLE IF EXISTS Person", now;

    // (re)create table
    session << "CREATE TABLE Person (Name VARCHAR(30), Address VARCHAR, Age INTEGER(3))", now;

    // insert some rows
    Person person =
    {
        "Bart Simpson",
        "Springfield",
        12
    };

    Statement insert(session);
    insert << "INSERT INTO Person VALUES(?, ?, ?)",
        use(person.name),
        use(person.address),
        use(person.age);

    insert.execute();

    person.name    = "Lisa Simpson";
    person.address = "Springfield";
    person.age     = 10;

    insert.execute();

    // a simple query
    Statement select(session);
    select << "SELECT Name, Address, Age FROM Person",
        into(person.name),
        into(person.address),
        into(person.age),
        range(0, 1); //  iterate over result set one row at a time

    while (!select.done())
    {
        select.execute();
        std::cout << person.name << " " << person.address << " " << person.age << std::endl;
    }

    HTTPTimeServer app;
    return app.run(argc, argv);
}
