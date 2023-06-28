#include <iostream>
#include "servercli.h"

ServerCLI::ServerCLI():CmdLineInterface("server>")
{
    smtp = new SMTPServer();
    initConsole();
    initCmd();

    if(smtp->start())
        cout << "SMTP Server started" << endl;
    else
        cout << "SMTP Server failed to start" <<endl;

}
ServerCLI::~ServerCLI()
{
    delete smtp;
}

void ServerCLI::initConsole()
{
    system("color f0");
    //_setmode(_fileno(stdout), _O_U16TEXT);
    SetConsoleTitleW(L"Chương trình SMTP Server");
    HANDLE hdlConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX consoleFont;
    consoleFont.cbSize = sizeof(consoleFont);
    //COORD font_size = {11,24};
    GetCurrentConsoleFontEx(hdlConsole, FALSE, &consoleFont);
    memcpy(consoleFont.FaceName, L"Consolas", sizeof(consoleFont.FaceName));
    consoleFont.dwFontSize = {11,24};
    SetCurrentConsoleFontEx(hdlConsole, FALSE, &consoleFont);
}

void ServerCLI::initCmd()
{
    addCmd("start",CLI_CAST(&ServerCLI::doStart));
    addCmd("stop",CLI_CAST(&ServerCLI::doStop));
    addCmd("restart",CLI_CAST(&ServerCLI::doRestart));
    addCmd("status",CLI_CAST(&ServerCLI::doStatus));
    addCmd("help", CLI_CAST(&ServerCLI::doHelp));
}

void ServerCLI::doStart(char * cmd_argv[], int cmd_argc)
{
    if(smtp->isRunning())
        cout << "SMTP server is running!" << endl;
    else if(smtp->start())
        cout << "SMTP Server started" << endl;
    else
        cout << "SMTP Server failed to start" <<endl;
}

void ServerCLI::doStop(char * cmd_argv[], int cmd_argc)
{
    smtp->stop();
    cout << "SMTP Server stopped" << endl;
}



void ServerCLI::doRestart(char * cmd_argv[], int cmd_argc)
{

    if(smtp->restart())
        cout << "SMTP Server restarted" << endl;
    else
        cout << "SMTP Server failed to restart" <<endl;

}

void ServerCLI::doStatus(char * cmd_argv[], int cmd_argc)
{

    if(smtp->isRunning())
        cout << "SMTP Server is running\n";
    else
        cout << "SMTP Server is not running\n";

}

void ServerCLI::doHelp(char * cmd_argv[], int cmd_argc)
{
    cout << "Cac lenh cua chuong trinh:" << endl;
    cout << "- start               Bat SMTP server" << endl;
    cout << "- stop                Tat SMTP server" << endl;
    cout << "- status              Trang thai SMTP server" << endl;
    cout << "- help                Tro giup" << endl;
    cout << "- quit                Thoat" << endl;
}
