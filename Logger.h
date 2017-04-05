
#ifndef LOGGER_H
#define LOGGER_H

#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sys/dir.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <iostream>

#include "Configuration.h"
#include "definition.h"

extern const Configuration CONF;
extern int now;

class Logger
{
    /* start the log */
  void start(int n = 0, const std::string& fname = LOG_FILE_NAME);
    /* stop the log */
    void stop();
    /* check if the stream is open or not */
    bool isOpen() const { return s.is_open(); }
    /* close the stream */
    void close() { s.close(); }
    /* simulation step */
    int step;
    std::string fileName;
    int indentation;    
public:
    /* stream */
    std::ofstream s;
    /* constructor */
    Logger(const std::string& fname = LOG_FILE_NAME) { start(0, fname); indentation = 0;}
    /* destructor */
    ~Logger() { stop(); }
    /* mark log file with a timestamp */
    void mark();
    /* switch log file */
    void setFile();
    /* indentation */
    const int& getIndentation() const {return indentation;}
    const int& incrementIndentation() {return (indentation = indentation + 1);}
    const int& decrementIndentation() {return (indentation = indentation - 1);}
};

#endif
