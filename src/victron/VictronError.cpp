#include "VictronError.h"

VictronError::VictronError(const std::string &_cause)
{
    cause = _cause;
    LOG_DEBUG("Error: %s", _cause.c_str());
}


const std::string &VictronError::get_cause() const
{
    return cause;
}

VictronError::~VictronError()
{

}
