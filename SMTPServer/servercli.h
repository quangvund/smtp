#ifndef SERVERCLI_H_INCLUDED
#define SERVERCLI_H_INCLUDED

#include "cli.h"
#include "smtpserver.h"
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

// typedef void (SocketCmdLineInterface::*SOCK_CLI_CMD_FUNC)(char*[], int);

class ServerCLI : public CmdLineInterface
{
private:
    SMTPServer* smtp;
public:
    ServerCLI();
    ~ServerCLI();
protected:
    virtual void initCmd();
private:
    void doStart(string cmd_argv[], int cmd_argc);
    void doStop(string cmd_argv[], int cmd_argc);
    void doStatus(string cmd_argv[], int cmd_argc);
    void doRestart(string cmd_argv[], int cmd_argc);
    void doHelp(string cmd_argv[], int cmd_argc);
};

#endif // SERVERCLI_H_INCLUDED
