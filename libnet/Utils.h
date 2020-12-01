#ifndef UTILS_H
#define UTILS_H


namespace LibNet
{
    // Miscellaneous utility functions.
    class Utils
    {
    public:
        // Output user-defined message and information about the last error to stdout and exit.
        static void err_exit(const std::string& msg);
    };
}

#endif
