// BasicServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <basicServer\ServerListener.h>
#include <basicServer\SocketServer.h>

int main()
{
    try {
        auto splistener = std::make_shared<YServerListener>();
        std::shared_ptr<YAbstractThreadListener> spAbstract = std::static_pointer_cast<YAbstractThreadListener>(splistener);
        YSocketServer server(7878);
        server.addListener(spAbstract);
        server.start();
        Sleep(100000000);
        //server.join();

    }
    catch (YException & e) {
        std::cout << e.what() << std::endl;
        LOGINFO("%s", e.what());
    }
}
