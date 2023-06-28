#include <iostream>
#include "servercli.h"

ServerCLI::ServerCLI():CmdLineInterface("server>")
{
    cout << "Simple SMTP Server v1.0" << endl;
    initCmd();
    smtp = new SMTPServer();
    if(!smtp->configServer())
    {
        cerr << "Error: Cannot load server configuration" << endl;
        delete smtp;
        exit(-1);
    }

}
ServerCLI::~ServerCLI()
{
    delete smtp;
}



void ServerCLI::initCmd()
{
    addCmd("start",CLI_CAST(&ServerCLI::doStart));
    addCmd("stop",CLI_CAST(&ServerCLI::doStop));
    addCmd("restart",CLI_CAST(&ServerCLI::doRestart));
    addCmd("status",CLI_CAST(&ServerCLI::doStatus));
    addCmd("help", CLI_CAST(&ServerCLI::doHelp));
}

void ServerCLI::doStart(string cmd_argv[], int cmd_argc)
{
    if(smtp->isRunning())
        cout << "SMTP server is running!" << endl;
    else if(smtp->start())
        cout << "SMTP Server started" << endl;
    else
        cout << "SMTP Server failed to start" <<endl;
}

void ServerCLI::doStop(string cmd_argv[], int cmd_argc)
{
    smtp->stop();
    cout << "SMTP Server stopped" << endl;
}



void ServerCLI::doRestart(string cmd_argv[], int cmd_argc)
{

    if(smtp->restart())
        cout << "SMTP Server restarted" << endl;
    else
        cout << "SMTP Server failed to restart" <<endl;

}

void ServerCLI::doStatus(string cmd_argv[], int cmd_argc)
{

    if(smtp->isRunning())
        cout << "SMTP Server is running\n";
    else
        cout << "SMTP Server is not running\n";

}

void ServerCLI::doHelp(string cmd_argv[], int cmd_argc)
{
    cout << "Cac lenh cua chuong trinh:" << endl;
    cout << "- start               Bat SMTP server" << endl;
    cout << "- stop                Tat SMTP server" << endl;
    cout << "- status              Trang thai SMTP server" << endl;
    cout << "- help                Tro giup" << endl;
    cout << "- quit                Thoat" << endl;
}
