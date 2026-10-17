#ifndef JOB_H
#define JOB_H

#include "../include/core.h"

// add any new job types here for runtime checks
enum class JOB_TYPE {
    MATCH,
    MATCH_WORD,
    START_QUERY,
    BARRIER,
    END
};

class Job
{
public:
    Job(JOB_TYPE type);
    virtual ~Job();

    virtual bool execute() = 0;

    JOB_TYPE type;
};

#endif // JOB_H
