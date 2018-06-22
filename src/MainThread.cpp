#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "Demo1Thread.hpp"
#include "Demo2Thread.hpp"
#include "MainThread.hpp"

#define MSG_QUE_MAX_SIZE    256

namespace alf
{
static void printCommandUsage()
{
    printf("Options:\n\
    [s]end thread_name cmd_id cmd_data\tSend command to a thread\n\
    [t]est [iteration=10]\t\tRun test cases\n\
    [q]uit\t\t\t\tTerminate all threads and exit\n\
    [h]elp\t\t\t\tPrint this message\n");
}

MainThread::MainThread()
    : IThread("mainThread", new MessageQueue<MsgType>(MSG_QUE_MAX_SIZE))
{
    // Create child threads that are handled by MainThread
    IThread* thread;
    thread = new Demo1Thread;
    mThreadMap.insert({thread->getName(), thread});
    thread = new Demo2Thread;
    mThreadMap.insert({thread->getName(), thread});
}

MainThread::~MainThread()
{
    for (const auto& it : mThreadMap)
    {
        delete it.second;
    }
}

void MainThread::sendCommand(const char* cmdStr)
{
    MsgType msg;
    size_t size = strlen(cmdStr);

    if (size > 1)
    {
        msg.cmd = new char[size];
        memcpy(msg.cmd, cmdStr, size - 1);
        msg.cmd[size - 1] = '\0';

        postMessage(&msg);
    }
}

void MainThread::run()
{
    // Start the execution of the handling threads
    for (const auto& it : mThreadMap)
    {
        it.second->start();
    }

    while (true)
    {
        SignalBits signal = waitSignal();

        if (signal & SIGNAL_EXIT)
        {
            break;
        }

        if (signal & SIGNAL_MSG)
        {
            bool exit = false;
            MsgType msg;
            while (!exit && getMessage(&msg))
            {
                const char* str = strtok(msg.cmd, " ");
                if (str[0] == 'q' || strcmp(str, "quit") == 0)
                {
                    postSignal(SIGNAL_EXIT);
                    exit = true;
                }
                else if (str[0] == 't' || strcmp(str, "test") == 0)
                {
                    int iter = 0;

                    str = strtok(NULL, " ");
                    if (str != NULL)
                    {
                        iter = atoi(str);
                    }
                    if (iter == 0)
                    {
                        iter = 10;
                    }
                    simpleTest(iter);
                }
                else if (str[0] == 'h' || strcmp(str, "help") == 0)
                {
                    printCommandUsage();
                }
                else if (str[0] == 's' || strcmp(str, "send") == 0)
                {
                    // Parse the string and dispatch the command to its corresponding thread.
                    do
                    {
                        str = strtok(NULL, " ");
                        if (str == NULL)
                        {
                            printCommandUsage();
                            break;
                        }

                        auto it = mThreadMap.find(str);
                        if (it == mThreadMap.end())
                        {
                            printf("thread %s is not found\n", str);
                            break;
                        }

                        str = strtok(NULL, " ");
                        if (str == NULL || isdigit(str[0]) == 0)
                        {
                            printCommandUsage();
                            break;
                        }

                        Demo1Thread::MsgType msg;
                        msg.id = (Demo1Thread::MsgIdType)atoi(str);

                        str = strtok(NULL, "");
                        if (str == NULL)
                        {
                            msg.data = NULL;
                        }
                        else
                        {
                            msg.data = new char[strlen(str) + 1];
                            strcpy((char*)msg.data, str);
                        }

                        it->second->postMessage(&msg);
                    } while (false);
                }
                
                delete[] msg.cmd;
            }
        }
    }

    for (const auto& it : mThreadMap)
    {
        it.second->postSignal(SIGNAL_EXIT);
        it.second->join();
    }
}

void MainThread::simpleTest(int iter)
{
    printf("Send messages to running threads:\n");
    for (int i = 0; i < iter; ++i)
    {
        for(int cmd_id = 0; cmd_id < 2; ++cmd_id)
        {
            for (const auto& it : mThreadMap)
            {
                Demo1Thread::MsgType msg;

                msg.id = (Demo1Thread::MsgIdType)cmd_id;

                char* buf = new char[64];
                snprintf(buf, 64, "iteration %d", i);
                msg.data = (void*)buf;

                it.second->postMessage(&msg);
            }
        }
    }

    usleep(10000);

    printf("\n\nTest relay messages:\n");
    auto it = mThreadMap.find("demo2");
    if (it != mThreadMap.end())
    {
        for (int i = 0; i < iter; ++i)
        {
            Demo2Thread::MsgType msg;

            msg.id = Demo2Thread::MSG_CMD_2;

            char* buf = new char[64];
            snprintf(buf, 64, "iteration %d", i);
            msg.data = (void*)buf;

            it->second->postMessage(&msg);
        }
    }
}

} //namespace alf
