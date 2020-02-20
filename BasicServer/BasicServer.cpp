// BasicServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <basicServer\ServerListener.h>
#include <basicServer\SocketServer.h>

int main()
{
    try {
        auto spAbstractListener = std::static_pointer_cast<YAbstractThreadListener>(std::make_shared<YServerListener>());
        YSocketServer server(7878);
        server.addListener(spAbstractListener);
        server.start();
        server.join();
    }
    catch (YException & e) {
        std::cout << e.what() << std::endl;
        LOGINFO("%s", e.what());
    }
}
