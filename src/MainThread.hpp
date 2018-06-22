#ifndef ALF_MAINTHREAD_HPP
#define ALF_MAINTHREAD_HPP

#include "unordered_map"

#include "IThread.hpp"

namespace alf
{
/**
 * Main thread
 *
 * This thread Handles other threads.
 */
class MainThread : public IThread
{
public:
    struct MsgType
    {
        char* cmd;
    };

    MainThread();

    virtual ~MainThread();

    void sendCommand(const char* cmdStr);

protected:
    virtual void run();

private:
    void simpleTest(int iter);

    std::unordered_map<std::string, IThread*> mThreadMap;
};

} //namespace alf

#endif /* ALF_MAINTHREAD_HPP */
