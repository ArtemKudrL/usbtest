// io and std::string <-> char* conversion
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

// multithreading
#include <mutex>
#include <thread>

// just for reverse
#include <algorithm>

#include "tcp_connection.hpp"
#include "parser_conversion.h"

constexpr std::size_t bufLen = 0x100; // = 256, 1 data len byte maximum
constexpr char filePrefix[] = "/dev/USBtty"; // only for current USB/COM converter
constexpr char syncBytes[] = "\x25\x25\x25\x25"; // Protocol
constexpr char endCommand[] = "end"; // enter it to exit

bool running = true; // thread communication w/o tcp
std::mutex m;

void reciever(int comPort)
{
    char buf[bufLen];
    std::string str;
    std::ifstream file(filePrefix + std::to_string(comPort));

    try { // init exception handling

    if (!file.is_open())
        throw std::runtime_error("Failed to open COM port "
                                + std::to_string(comPort));
    Client client; // constructor includes init
    m.unlock();

    while (running)
    {
        file >> str;
        try { // weird data handling
        // converts every 2 str chars to 1 buf byte
        fromHex(buf, str.c_str());

        // 0 only if syncBytes in beginning
        if (str.assign(buf).find(syncBytes))
            throw std::invalid_argument("Wrong sync bytes");

        // because syncBytes well defined
        unsigned char dataLen = buf[sizeof(syncBytes)-1];
        if (std::strlen(buf) != sizeof(syncBytes) + dataLen)
            throw std::invalid_argument("Wrong data size");

        } catch (...)
        {
            if (str == endCommand)
                std::strcpy(buf, "end");
            else
                std::strcpy(buf, "error");
        }

        client.sendMsg(buf, bufLen);
    }
    } catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        running = false;
        m.unlock();
    }
}

void processer(int comPort)
{
    char buf[bufLen];
    std::string str;
    std::ofstream file(filePrefix + std::to_string(comPort));

    try { // init exception handling

    if (!file.is_open())
        throw std::runtime_error("Failed to open COM port " +
                                std::to_string(comPort));
    Client client; // constructor includes init
    m.unlock();


    while (running)
    {
        client.recieveMsg(buf, bufLen);

        // because syncBytes well defined
        unsigned char dataLen = buf[sizeof(syncBytes)-1];
        std::reverse(buf + sizeof(syncBytes), buf + sizeof(syncBytes) + dataLen);
        str = toHex(buf + sizeof(syncBytes)-1);

        file << str << std::endl;
    }
    } catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        running = false;
        m.unlock();
    }
}

int main()
{
    // recieving correct com port
    constexpr int comPortMin = 1;
    constexpr int comPortMax = 99;
    int comPort1, comPort2;
    do {
        std::cout << "Введите номер COM для ПОТОК №1 число от "<< comPortMin << "-" << comPortMax << std::endl;
        std::cin >> comPort1;
    } while (!(comPortMin <= comPort1 && comPort1 <= comPortMax));
    do {
        std::cout << "Введите номер COM для ПОТОК №2 число от "<< comPortMin << "-" << comPortMax << std::endl;
        std::cin >> comPort2;
    } while (!(comPortMin <= comPort2 && comPort1 <= comPortMax));

    try { // handling server creation exceptions

    Server server;
    char buf[bufLen];
    std::cout << "START" << std::endl;

    constexpr int recvIdx = 0; // used to identify client in communication
    m.lock();
    std::thread recvThread(reciever, comPort1);
    recvThread.detach();
    m.lock();
    if (!running) // client creation or file open failed
        return EXIT_FAILURE;
    m.unlock();
    server.acceptClient();

    constexpr int procIdx = 1;  // used to identify client in communication
    m.lock();
    std::thread procThread(processer, comPort2);
    procThread.detach();
    m.lock();
    if (!running) // client creation or file open failed
        return EXIT_FAILURE;
    m.unlock();
    server.acceptClient();

    server.endListen(); // no more clients

    while (running)
    {
        std::cout << "Insert_TX_Packet" << std::endl;
        server.recieveMsg(recvIdx, buf, bufLen);

        // client msg to end
        if (std::strcmp(buf, "end") == 0)
            running = false;
        // client msg to tell us about weird data
        else if (std::strcmp(buf, "error") == 0)
            std::cerr << "Error_TX_Packet" << std::endl;
        else
            server.sendMsg(procIdx, buf, bufLen);
    }

    return EXIT_SUCCESS;

    } catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl; // server failed
        return EXIT_FAILURE;
    }
}
