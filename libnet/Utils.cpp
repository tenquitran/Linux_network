#include "LibNet.h"

//////////////////////////////////////////////////////////////////////

using namespace LibNet;

//////////////////////////////////////////////////////////////////////


void Utils::err_exit(const std::string& msg)
{
    perror(msg.c_str());
    exit(1);
}