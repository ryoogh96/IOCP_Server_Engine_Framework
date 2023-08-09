#include "pch.hpp"

int main()
{
    Engine::MiniDump dump;
    dump.beginDump();

    Engine::ServerSocketManager* serverSocketManager = new Engine::ServerSocketManager();

    while (true)
    {
        //serverSocketManager->broadcast();
        
		Sleep(1000);
    }

    delete serverSocketManager;
    return 0;
}