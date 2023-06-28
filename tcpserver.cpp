#ifdef _WIN32
//#define _WIN32_WINNT 0x0601
#include <thread>
//#include "mingw.thread.h"
#else
#include <thread>
#endif // _WIN32

#include <iostream>
#include "tcpserver.h"

#define _CONCURRENT_MT_SERVER_

TCPServer::TCPServer(unsigned short localPort)
{
    this->port = localPort;
    running = false;
    this->numCmd = 0;
}

TCPServer::~TCPServer()
{
}


void TCPServer::addCmd(const string& cmdName, CMD_FUNC f)
{
    this->cmdDoFunc[this->numCmd]=f;
    this->cmdNameList[this->numCmd] = cmdName;
    this->numCmd++;
}

void TCPServer::doCmd(Session* session, uint8_t cmdId, char * cmd_argv[], int cmd_argc)
{
    if(cmdId == SERVER_CMD_UNKNOWN)
        session->doUnknown(cmd_argv, cmd_argc);
    else
    {
        CMD_FUNC f = this->cmdDoFunc[cmdId];
        if(f!=NULL&&session!=NULL)
            (session->*f)(cmd_argv, cmd_argc);  // call with member function pointer
    }
}

bool TCPServer::start()
{
    try
    {
        this->configServer();
        this->master.setListen(this->port);
        running = true;
        // create a thread to run TCPServer::run
        std::thread t(&TCPServer::run,this);
        t.detach();
        return true;
    }
    catch(SocketException& e)
    {
        cerr << e.what() << endl;
        return false;
    }
}
void TCPServer::stop()
{
    try
    {
        master.close();
        running = false;
        this->cleanServer();
    }
    catch(SocketException& e)
    {
        cerr << e.what() << endl;

    }
}

bool TCPServer::restart()
{
    master.close();
    running = false;
    try
    {
        master.setListen(this->port);
        running = true;
        // create a thread to run TCPServer::run

        std::thread t(&TCPServer::run,this);
        t.detach();
        return true;

    }
    catch(SocketException& e)
    {
        cerr << e.what() << endl;
        return false;
    }
}

bool TCPServer::isRunning()
{
    return this->running;
}


void TCPServer::run()
{
    TcpSocket *slave;
    while(running) // vong lap xu ly cua server
    {
        // chap nhan ket noi tu client
        try
        {
            slave = master.accept();
            if(!slave)
                continue;
        }
        catch(SocketException& e)
        {
            if(!running)
                break;
            cerr << e.what() << endl;
            continue;
        }
        // Create a thread and add to ThreadPool
        try
        {
#ifdef _CONCURRENT_MT_SERVER_
            std::thread t(&TCPServer::startNewSession,this,slave);
            t.detach();
#else
            this->startNewSession(slave);
#endif // _CONCURRENCE_H*/
        }
        catch (exception& e)
        {
            cerr << "Khong tao duoc thread thuc hien Session\n" ;
        }

    }
}



