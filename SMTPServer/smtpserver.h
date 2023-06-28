#ifndef SMTPSERVER_H_
#define SMTPSERVER_H_


#include "tcpserver.h"
#include "smtpsession.h"
#include "smtpserverconfig.h"


class SMTPServer: public TCPServer
{

public:
    SMTPServer(unsigned short port=25);
    ~SMTPServer();
    bool configServer();
protected:

    void cleanServer();
    SMTPServerConfig* getServerConfig() {return (SMTPServerConfig*)conf;}
    bool loadServerConfig(const string& confFileName);
    void startNewSession(TcpSocket slave);
    void initCmd();
private:
    int readCmd(TcpSocket& slave, string& cmdLine);
    unsigned short parseCmd(const string& cmdLine, string cmd_argv[], int& cmd_argc);
protected:
};

#endif // SMTPSERVER_H_
