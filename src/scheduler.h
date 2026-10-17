#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "list.h"
#include "job.h"
#include <pthread.h>
#include <semaphore.h>

//---------------------------------------------------------
//   Scheduler
///     All StartQueryJobs must be executed before the first MatchDocument (i.e. not in parallel) and vice versa.
///     To that end when submitting jobs a barrier is placed if the job is a MatchJob and there have been StartQueryJobs since the last barrier and vice versa.
//---------------------------------------------------------

class Scheduler
{
public:
    Scheduler(unsigned int threadCount);
    ~Scheduler();

    /* public API */
    ErrorCode submitJob(Job* job);
    bool waitForJobsToFinish();
    void insertBarrier();

    /* private API (at the same time it must be available to pthreads (C threads) */
    Job* getJob();

    pthread_barrier_t   m_barrier;
private:

    bool            m_createBarrier { false };

    pthread_t*      m_threads       { nullptr };
    unsigned int    m_threadCount   { 0 };

    List<Job*>      m_jobs;
    pthread_mutex_t m_jobs_mtx;
    sem_t           m_jobs_not_empty; // used to block getJob when the list is empty, the max value is INT_MAX so it is big enough for our use case (if we had more than 2 billion jobs then it would stop working)
};

//---------------------------------------------------------
//   BarrierJob
//---------------------------------------------------------

/* special job that tells the thread to wait for all threads to arrive at this point */
class BarrierJob : public Job
{
public:
    BarrierJob(pthread_barrier_t* m_barrier);

    bool execute() override;

private:
    pthread_barrier_t*   barrier;
};

/* special job that tells the thread to exit */
class EndJob : public Job
{
public:
    EndJob();

    bool execute() override;
};

void* threadMain(void* parameter);

#endif // SCHEDULER_H
