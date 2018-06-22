#ifndef ALF_ITHREAD_HPP
#define ALF_ITHREAD_HPP

#include <cstdint>
#include <cstring>
#include <string>

namespace alf
{
typedef uint64_t ThreadIdType;

typedef uint32_t SignalBits;
enum : uint32_t
{
    SIGNAL_EXIT     = 0x1 << 0,
    SIGNAL_MSG      = 0x1 << 1,
    SIGNAL_SUSPEND  = 0x1 << 2,
    SIGNAL_RESUME   = 0x1 << 3
};

/**
 * Message queue interface
 *
 * Used by IThread class to set up the message queue with generic types.
 */
class IMessageQueue
{
public:
    virtual ~IMessageQueue() {}

    virtual void push(const void* msg) = 0;
    virtual bool pop(void* msg) = 0;
};

/**
 * Abstract class for threads
 *
 * Support thread-specific execution context and a message queue.
 * Derived classes must implement the run() method.
 */
class IThread
{
public:
    IThread(std::string name, IMessageQueue* queue = NULL);

    virtual ~IThread();

    void start();

    void join();

    bool isRunning() const;

    std::string getName() const;

    ThreadIdType getId() const;

    /**
     * Post signals
     *
     * The same signal (flag) is set only once before waitSiganl() is called.
     */
    void postSignal(SignalBits newSignal);

    /**
     * Post messages
     *
     * Message is sent to the thread's queue and can be popped by getMessage().
     */
    void postMessage(const void* msg);

protected:
    /**
     * Abstract method executed when thread is started
     */
    virtual void run() = 0;

    /**
     * Wait for signals
     *
     * Suspend execution until a signal occurs and then return current signals.
     */
    SignalBits waitSignal();

    /**
     * Get a message
     *
     * Return true if a message is found; false otherwise.
     */
    bool getMessage(void* msg);

    bool mRunning;

private:
    static void* threadFunc(void* threadObj);

    std::string mThreadName;
    ThreadIdType mThreadId;

    SignalBits mSignals;
    IMessageQueue* mMsgQue;

    struct ThreadImpl; // Forward forward declaration of the implementation class
    ThreadImpl* mImpl; // To hide platform-specific implementation details
};

/**
 * Message queue of generic types
 */
template<class T> class MessageQueue : public IMessageQueue
{
public:
    MessageQueue(size_t size)
        : mSize(size), mHead(0), mTail(0)
    {
        mQueue = new T[mSize];
    }

    virtual ~MessageQueue()
    {
        delete mQueue;
    }

    virtual void push(const void* msg)
    {
        int newTail = (mTail + 1) % mSize;

        // Drop message if queue is full.
        if (newTail != mHead)
        {
            void* buf = (uint8_t*)mQueue + mTail * sizeof(T);
            memcpy(buf, msg, sizeof(T));
            mTail = newTail;
        }
    }

    virtual bool pop(void* msg)
    {
        if (mHead == mTail)
        {
            return false;
        }

        const void* buf = (uint8_t*)mQueue + mHead * sizeof(T);
        memcpy(msg, buf, sizeof(T));
        mHead = (mHead + 1) % mSize;
        return true;
    }

private:
    T* mQueue;
    size_t mSize;
    int mHead;
    int mTail;
};

} //namespace alf

#endif /* ALF_ITHREAD_HPP */
