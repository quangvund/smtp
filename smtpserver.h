#ifndef SMTPSERVER_H_
#define SMTPSERVER_H_


#include <mutex>
#include <fstream>
#include "tcpserver.h"
#include "smtpsession.h"
#include "smtpserverconfig.h"


class SMTPServer: public TCPServer
{
private:
    unsigned short connTimeout = 0;
    std::mutex* logMutex = NULL;
    std::fstream* logFile = NULL;
public:
    SMTPServer(unsigned short port=25);
    ~SMTPServer();

protected:
    void configServer();
    void cleanServer();
    SMTPServerConfig* getServerConfig() {return (SMTPServerConfig*)conf;}
    void loadServerConfig(const string& confFileName);
    void startNewSession(TcpSocket* slave);
    void initCmd();
private:
    int readCmd(TcpSocket* slave, char* buffer, int buflen);
    unsigned short parseCmd(char *sCmdBuf, int len, char * cmd_argv[], int& cmd_argc);
protected:
};

#endif // SMTPSERVER_H_
