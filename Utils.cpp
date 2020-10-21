//
// Created by NIshant Sabharwal on 10/7/20.
//

#include <vector>
#include <sstream>
#include "Utils.h"

static const char *error_log = "error.log";
static bool debug = true;

vector<string> Utils::split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

void Utils::logf(const char* format, ...) {
    int maxLine = 4096;
    char tformat[1024];
    char logLine[maxLine];
    int len;
    va_list ap;
    FILE *out;
    struct tm time_buffer;

    time_t tm = time(NULL);
    len = strftime(tformat, sizeof(tformat), "%m/%d %H:%M:%S", localtime_r(&tm, &time_buffer));
    snprintf(tformat + len, sizeof(tformat) - len, " %s", format);

    out = fopen(error_log, "a+");
    if (out) {
        va_start(ap, format);
        vsnprintf(logLine, maxLine, tformat, ap);
        logLine[maxLine-1] = 0;
        fprintf(out, "%s", logLine);
        if (debug) {
            printf("%s", logLine);
        }
        va_end(ap);
        fclose(out);
    }
}

void Utils::errf(const char* format, ...) {
    int maxLine = 4096;
    char tformat[1024];
    char logLine[maxLine];
    int len;
    va_list ap;
    FILE *out;
    struct tm time_buffer;

    time_t tm = time(NULL);
    len = strftime(tformat, sizeof(tformat), "%m/%d %H:%M:%S", localtime_r(&tm, &time_buffer));
    snprintf(tformat + len, sizeof(tformat) - len, " %s", format);

    out = fopen(error_log, "a+");
    if (out) {
        va_start(ap, format);
        vsnprintf(logLine, maxLine, tformat, ap);
        logLine[maxLine-1] = 0;
        fprintf(out, "%s", logLine);
        printf("%s", logLine);
        va_end(ap);
        fclose(out);
    }
}

/*void Utils::errf(const char* format, ...) {
    char tformat[1024];
    int len;
    va_list ap;
    FILE *out;
    struct tm time_buffer;

    time_t tm = time(NULL);
    len = strftime(tformat, sizeof(tformat), "%m/%d %H:%M:%S", localtime_r(&tm, &time_buffer));
    snprintf(tformat + len, sizeof(tformat) - len, " %s", format);

    out = fopen(error_log, "a+");
    if (out) {
        va_start(ap, format);
        vfprintf(out, tformat, ap);
        vprintf(tformat, ap);
        va_end(ap);
        fclose(out);
    }
}*/
