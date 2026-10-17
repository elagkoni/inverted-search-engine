#include "scheduler.h"


//---------------------------------------------------------
//   Scheduler
//---------------------------------------------------------

Scheduler::Scheduler(unsigned int threadCount)
{
    pthread_mutex_init(&m_jobs_mtx, NULL);
    sem_init(&m_jobs_not_empty, 1, 0);

#ifdef DOCUMENT_THREADING
    pthread_barrier_init(&m_barrier, NULL, threadCount);
#elif WORD_THREADING
    // an additional thread because the main thread is also waiting at the barrier (MatchDocument)
    pthread_barrier_init(&m_barrier, NULL, threadCount + 1);
#endif

    m_threadCount = threadCount;
    m_threads = new pthread_t[m_threadCount];

    //
    // Create threads
    //
    for (unsigned int i = 0; i < m_threadCount; i++) {
        if (pthread_create(&m_threads[i], nullptr, threadMain, this) != 0) {
            std::cerr << "Unable to create thread" << std::endl;
        }
    }
}

Scheduler::~Scheduler()
{
    pthread_mutex_destroy(&m_jobs_mtx);
    sem_destroy(&m_jobs_not_empty);
    pthread_barrier_destroy(&m_barrier);
}

ErrorCode Scheduler::submitJob(Job* job)
{
    pthread_mutex_lock(&m_jobs_mtx);

#ifdef DOCUMENT_THREADING
    // if the job is a match document and there have been StartQuery jobs since the last barrier, insert a new barrier
    // because all StartQuery jobs must finish before a MatchDocument job begins
    if (job->type == JOB_TYPE::MATCH && m_createBarrier) {
        insertBarrier();
    } else if (job->type == JOB_TYPE::START_QUERY) {
        m_createBarrier = true;
    }
#endif
    m_jobs.append(job);

    sem_post(&m_jobs_not_empty);
    pthread_mutex_unlock(&m_jobs_mtx);

    return EC_SUCCESS;
}

bool Scheduler::waitForJobsToFinish()
{
    pthread_mutex_lock(&m_jobs_mtx);
    for (unsigned int i = 0; i < m_threadCount; i++) {
        m_jobs.append(new EndJob());
        sem_post(&m_jobs_not_empty);
    }
    pthread_mutex_unlock(&m_jobs_mtx);

    for (unsigned int i = 0; i < m_threadCount; i++) {
        pthread_join(m_threads[i], nullptr);
    }

    delete[] m_threads;

    return true;
}

Job* Scheduler::getJob()
{
    Job* job;

    sem_wait(&m_jobs_not_empty);
    pthread_mutex_lock(&m_jobs_mtx);

    job = m_jobs.first();
    m_jobs.remove(static_cast<unsigned int>(0));

    pthread_mutex_unlock(&m_jobs_mtx);

    return job;
}

void Scheduler::insertBarrier()
{
    for (unsigned int i = 0; i < m_threadCount; i++) {
#ifdef DOCUMENT_THREADING
        // does not lock the jobs list, the caller must hold the lock before calling
        m_jobs.append(new BarrierJob(&m_barrier));
        sem_post(&m_jobs_not_empty);
#elif WORD_THREADING
        submitJob(new BarrierJob(&m_barrier));
#endif
    }
    m_createBarrier = false;
}

//---------------------------------------------------------
//   BarrierJob
//---------------------------------------------------------

BarrierJob::BarrierJob(pthread_barrier_t* m_barrier) : Job(JOB_TYPE::BARRIER)
{
    barrier = m_barrier;
}

bool BarrierJob::execute()
{
    pthread_barrier_wait(barrier);
    return true;
}

EndJob::EndJob() : Job(JOB_TYPE::END)
{
    ;
}

bool EndJob::execute()
{
    return false;
}

//---------------------------------------------------------
//   Thread
//---------------------------------------------------------

void* threadMain(void* parameter)
{
    Scheduler* sched = reinterpret_cast<Scheduler*>(parameter);
    Job* job { nullptr };

    do {
        if (job)
            delete job;
        job = sched->getJob();
    } while (job->execute());

    delete job;

    return nullptr;
}
