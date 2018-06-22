#include <cstdio>
#include <unistd.h>

#include "Demo1Thread.hpp"
#include "MainThread.hpp"

#define READ_BUF_SIZE   256

int main()
{
    alf::MainThread mainT;
    mainT.start();

    usleep(500); // Add a delay to make prompt output unaffected by thread output

    char buf[READ_BUF_SIZE];
    do
    {
        fputs("$ ", stdout);
        fgets(buf, sizeof(buf), stdin);
        mainT.sendCommand(buf);
        usleep(500);
    } while (mainT.isRunning());

    mainT.join();

    return 0;
}
