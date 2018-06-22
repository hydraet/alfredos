#include <cstdio>

#include "Demo1Thread.hpp"

#define MSG_QUE_MAX_SIZE    256

namespace alf
{
Demo1Thread::Demo1Thread()
    : IThread("demo1", new MessageQueue<MsgType>(MSG_QUE_MAX_SIZE))
{
}

Demo1Thread::~Demo1Thread()
{
}

void Demo1Thread::run()
{
    while (true)
    {
        SignalBits signal = waitSignal();

        if (signal & SIGNAL_EXIT)
        {
            break;
        }

        if (signal & SIGNAL_MSG)
        {
            MsgType msg;
            while (getMessage(&msg))
            {
                switch (msg.id)
                {
                    case MSG_CMD_0:
                    case MSG_CMD_1:
                        printf("tid(%lx) %s executes cmd_id %d", getId(), getName().c_str(), msg.id);

                        if (msg.data == NULL)
                        {
                            putchar('\n');
                        }
                        else
                        {
                            printf(" with cmd_data \"%s\"\n", (char*)msg.data);

                            delete[] (char*)msg.data;
                        }
                        break;

                    default:
                        fprintf(stderr, "tid(%lx) %s got invalid command %d\n", getId(), getName().c_str(), msg.id);
                        break;
                }
            }
        }
    }
}

} //namespace alf
