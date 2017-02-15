
#include "Logger.h"

using namespace std;

void Logger::start(int n, const std::string& fname)
{
    if(!CONF.debug)
        return;
    
    step = n;
    fileName = fname;
    
    /* create output dir */
    mkdir(OUTPUT_DIR, S_IRWXU | S_IRGRP | S_IROTH);
    
    /* build file name */
    string fileName = fname + (string)"." + toString(n, 5);
    
    /* open log stream */
    s.open(fileName.c_str(), ios_base::out);

    s << fixed;
    s << setprecision(3);

    /* error handling */
    if(!isOpen())
        error("Logger", "cannot open log file " + fileName);
    
    /* mark log file with a timestamp */
    mark();
}

void Logger::stop()
{
    if(isOpen())
    {
        mark();
        close();
    }
}

void Logger::mark()
{
    /* error handling */
    if(!isOpen())
        error("Logger", "the stream is closed!");

    /* mark log file with a timestamp */
    char tmp[50];
    time_t curtime = time(NULL);
    tm *loctime = localtime(&curtime);
    strftime(tmp, 50, "\n--- MARK @ %d-%m-%Y %H:%M:%S\n", loctime);
    s << tmp << endl;
}

void Logger::setFile()
{
    if(CONF.debug)
    {
        if(step != now)
        {
            stop();
            start(now, fileName);
        }
    }
}
