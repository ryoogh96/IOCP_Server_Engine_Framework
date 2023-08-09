#include "pch.hpp"

int main()
{
    Engine::MiniDump dump;
    dump.beginDump();

    Engine::ServerSocketManager* serverSocketManager = new Engine::ServerSocketManager();

    while (true)
    {
        //serverSocketManager->broadcast();
        
        this_thread::sleep_for(std::chrono::seconds(1));
    }

    delete serverSocketManager;
    return 0;
}