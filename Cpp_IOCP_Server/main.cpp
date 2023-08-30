#include "pch.hpp"

int main()
{
    Engine::MiniDump dump;
    dump.BeginDump();

    char sendData[1000] = "Server Send Hello to Client";
    while (true)
    {
        Engine::SendBufferRef sendBuffer = Engine::GSendBufferManager->Open(4096);

        BYTE* buffer = sendBuffer->Buffer();
        (reinterpret_cast<Engine::PacketHeader*>(buffer))->size = (sizeof(sendData) + sizeof(Engine::PacketHeader));
        (reinterpret_cast<Engine::PacketHeader*>(buffer))->id = 1; // Server Send Hello To Client
        ::memcpy(&buffer[4], sendData, sizeof(sendData));
        sendBuffer->Close((sizeof(sendData) + sizeof(Engine::PacketHeader)));
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}