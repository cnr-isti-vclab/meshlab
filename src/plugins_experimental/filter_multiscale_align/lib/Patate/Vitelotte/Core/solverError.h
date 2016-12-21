/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_SOLVER_ERROR_
#define _VITELOTTE_SOLVER_ERROR_


#include <string>


namespace Vitelotte
{


/**
 * \brief A class that encodes the status of a FemSolver object.
 */
class SolverError
{
public:
    enum Status
    {
        STATUS_OK,
        STATUS_WARNING,
        STATUS_ERROR
    };

public:
    inline SolverError() : m_status(STATUS_OK), m_message() {}
    inline Status status() const { return m_status; }
    inline const std::string& message() const { return m_message; }
    inline void resetStatus() { m_status = STATUS_OK; m_message.clear(); }

    inline void set(Status status, const std::string& message)
    {
        if(m_status > status)
            return;
        m_status = status;
        m_message = message;
    }

    inline void error(const std::string& message)
    {
        set(STATUS_ERROR, message);
    }

    inline void warning(const std::string& message)
    {
        set(STATUS_WARNING, message);
    }

private:
    Status m_status;
    std::string m_message;
};


}


#endif
