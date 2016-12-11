
#ifndef LOGGER_H
#define LOGGER_H

#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sys/dir.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Configuration.h"
#include "definition.h"

extern const Configuration CONF;
extern int now;

class Logger
{
    /* start the log */
    void start(int n = 0);
    /* stop the log */
    void stop();
    /* check if the stream is open or not */
    bool isOpen() const { return s.is_open(); }
    /* close the stream */
    void close() { s.close(); }
    /* simulation step */
    int step;
public:
    /* stream */
    ofstream s;
    /* constructor */
    Logger() { start(); }
    /* destructor */
    ~Logger() { stop(); }
    /* mark log file with a timestamp */
    void mark();
    /* switch log file */
    void setFile();
};

#endif
