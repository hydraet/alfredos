#ifndef ALF_DEMO1THREAD_HPP
#define ALF_DEMO1THREAD_HPP

#include "IThread.hpp"

namespace alf
{
/**
 * Demo 1 module
 */
class Demo1Thread : public IThread
{
public:
    enum MsgIdType
    {
        MSG_CMD_0,
        MSG_CMD_1
    };

    struct MsgType
    {
        MsgIdType id;
        void* data;
    };

    Demo1Thread();

    virtual ~Demo1Thread();

protected:
    virtual void run();
};

} //namespace alf

#endif /* ALF_DEMO1THREAD_HPP */
