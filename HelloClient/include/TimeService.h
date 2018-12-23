/*
 * TimeService.h
 *
 *  Created on: Dec 22, 2018
 *      Author: vhunter
 */

#ifndef TIMESERVICE_H_
#define TIMESERVICE_H_


#include <string>


namespace Sample {


//@ remote
class TimeService
{
public:
    TimeService();
        /// Creates the TimeService.

    ~TimeService();
        /// Destroys the TimeService.

    std::string currentTimeAsString() const;
        /// Returns the current time, formatted
        /// as a string (HH:MM::SS).
};



} // namespace Sample

#endif /* TIMESERVICE_H_ */
