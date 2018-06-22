#include <cstdio>
#include <cstring>
#include <pthread.h>

#include "IThread.hpp"

#define INVALID_TID     0UL
#define THREAD_NAME_LEN 16

namespace alf
{
struct IThread::ThreadImpl
{
    pthread_mutex_t state_mutex;

    pthread_mutex_t event_mutex;
    pthread_cond_t signal_cond;

    ThreadImpl()
        : state_mutex(PTHREAD_MUTEX_INITIALIZER), event_mutex(PTHREAD_MUTEX_INITIALIZER), signal_cond(PTHREAD_COND_INITIALIZER)
    {
    }
};

IThread::IThread(std::string name, IMessageQueue* queue)
    : mRunning(false), mThreadName(name), mThreadId(INVALID_TID), mSignals(0), mMsgQue(queue)
{
    mImpl = new ThreadImpl;
}

IThread::~IThread()
{
    join();

    delete mImpl;
    delete mMsgQue;
}

void IThread::start()
{
    pthread_mutex_lock(&mImpl->state_mutex);

    if (mThreadId == INVALID_TID)
    {
        int err = pthread_create((pthread_t*)&mThreadId, NULL, &threadFunc, (void*)this);
        if (err == 0)
        {
            // Set the name of the created thread
            if (!mThreadName.empty())
            {
                char name[THREAD_NAME_LEN];

                strncpy(name, mThreadName.c_str(), THREAD_NAME_LEN - 1);
                name[THREAD_NAME_LEN - 1] = '\0';
                pthread_setname_np((pthread_t)mThreadId, name);
            }
        }
        else
        {
            perror("pthread_create");
        }
    }

    pthread_mutex_unlock(&mImpl->state_mutex);
}

void IThread::join()
{
    pthread_mutex_lock(&mImpl->state_mutex);

    if (mThreadId != INVALID_TID)
    {
        if (pthread_join((pthread_t)mThreadId, NULL) != 0)
        {
            perror("pthread_join");
        }
        mThreadId = INVALID_TID;
    }

    pthread_mutex_unlock(&mImpl->state_mutex);
}

bool IThread::isRunning() const
{
    return mRunning;
}

std::string IThread::getName() const
{
    return mThreadName;
}

ThreadIdType IThread::getId() const
{
    return mThreadId;
}

void IThread::postSignal(SignalBits newSignal)
{
    pthread_mutex_lock(&mImpl->event_mutex);
    mSignals |= newSignal;
    pthread_cond_signal(&mImpl->signal_cond);
    pthread_mutex_unlock(&mImpl->event_mutex);
}

SignalBits IThread::waitSignal()
{
    SignalBits signals;

    pthread_mutex_lock(&mImpl->event_mutex);

    while (mSignals == 0)
    {
        pthread_cond_wait(&mImpl->signal_cond, &mImpl->event_mutex);
    }

    signals = mSignals;
    mSignals = 0;
    pthread_mutex_unlock(&mImpl->event_mutex);

    return signals;
}

void IThread::postMessage(const void* msg)
{
    if (mMsgQue == NULL)
    {
        return;
    }

    pthread_mutex_lock(&mImpl->event_mutex);
    mMsgQue->push(msg);
    mSignals |= SIGNAL_MSG;
    pthread_cond_signal(&mImpl->signal_cond);
    pthread_mutex_unlock(&mImpl->event_mutex);
}

bool IThread::getMessage(void* msg)
{
    if (mMsgQue == NULL)
    {
        return false;
    }

    bool result;

    pthread_mutex_lock(&mImpl->event_mutex);
    result = mMsgQue->pop(msg);
    pthread_mutex_unlock(&mImpl->event_mutex);

    return result;
}

void* IThread::threadFunc(void* threadObj)
{
    // Get the thread instance
    IThread *thread = (IThread*)threadObj;

    printf("tid(%lx) %s started\n", thread->getId(), thread->getName().c_str());

    thread->mRunning = true;

    thread->run();

    thread->mRunning = false;

    printf("tid(%lx) %s exit\n", thread->getId(), thread->getName().c_str());

    return NULL;
}

} //namespace alf
