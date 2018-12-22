/*
 * TimeService.cpp
 *
 *  Created on: Dec 22, 2018
 *      Author: vhunter
 */

#include "../include/TimeService.h"

#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"


namespace Sample {


TimeService::TimeService()
{
}


TimeService::~TimeService()
{
}


std::string TimeService::currentTimeAsString() const
{
    Poco::DateTime now;
    return Poco::DateTimeFormatter::format(now, "%H:%M:%S");
}


}


