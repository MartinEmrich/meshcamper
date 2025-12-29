#pragma once

#include"Logging.h"
#include<string>

/* simple generic error exception */
class VictronError
{
private:
    std::string cause;

public:
    VictronError(const std::string &_cause);

    const std::string &get_cause() const;

    ~VictronError();
};
