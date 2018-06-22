#ifndef ALF_DEMO2THREAD_HPP
#define ALF_DEMO2THREAD_HPP

#include "IThread.hpp"

namespace alf
{
/**
 * Demo 2 module
 */
class Demo2Thread : public IThread
{
public:
    enum MsgIdType
    {
        MSG_CMD_0,
        MSG_CMD_1,
        MSG_CMD_2
    };

    struct MsgType
    {
        MsgIdType id;
        void* data;
    };

    Demo2Thread();

    virtual ~Demo2Thread();

protected:
    virtual void run();
};

} //namespace alf

#endif /* ALF_DEMO2THREAD_HPP */
