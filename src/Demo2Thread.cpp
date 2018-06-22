#include <cstdio>

#include "Demo2Thread.hpp"

#define MSG_QUE_MAX_SIZE    256

namespace alf
{
Demo2Thread::Demo2Thread()
    : IThread("demo2", new MessageQueue<MsgType>(MSG_QUE_MAX_SIZE))
{
}

Demo2Thread::~Demo2Thread()
{
}

void Demo2Thread::run()
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
                    case MSG_CMD_2:
                        printf("tid(%lx) %s executes cmd_id %d", getId(), getName().c_str(), msg.id);

                        if (msg.data == NULL)
                        {
                            putchar('\n');
                        }
                        else
                        {
                            printf(" with cmd_data \"%s\"\n", (char*)msg.data);

                            if (msg.id == MSG_CMD_2)
                            {
                                // Relay this message to the same thread with different command id
                                printf("Relay message to cmd_id 1\n");
                                msg.id = MSG_CMD_1;
                                postMessage(&msg);
                            }
                            else
                            {
                                delete[] (char*)msg.data;
                            }
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
