#include "pch.hpp"

int main()
{
    Engine::MiniDump dump;
    dump.BeginDump();

    Engine::ServerSocketManager* serverSocketManager = new Engine::ServerSocketManager();

    while (true)
    {
        //serverSocketManager->Broadcast();
        
        this_thread::sleep_for(std::chrono::seconds(1));
    }

    delete serverSocketManager;
    return 0;
}