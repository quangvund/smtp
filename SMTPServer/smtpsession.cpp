#include <cstring>
#include <algorithm>    // std::find
#include <fstream>
#include <iostream>
#include "smtpsession.h"
#include "smtpserverconfig.h"
#include "random.h"


SMTPSession::SMTPSession(const TcpSocket& slave,ServerConfig* conf):Session(slave,conf)
{
    this->info = new SMTPSessionInfo();
    this->quitSession = false;
    this->info->status = STATUS_INIT;
}

void SMTPSession::reset()
{
    this->info->status = STATUS_INIT;
    this->info->fromAddress.clear();
    this->info->toAddress.clear();
}

SMTPSession::~SMTPSession()
{
    delete info;
}

void SMTPSession::doHelo(string cmd_argv[], int cmd_argc)
{
    response = "250 Welcome to SMTP Server v1.0\r\n";
    slave.send(response);
}

void SMTPSession::doMail(string cmd_argv[], int cmd_argc)
{
    if(cmd_argc==2)
    {

    }
    else
    {
        response = "501 Invalid parameter\r\n";
        slave.send(response);
    }
}

void SMTPSession::doRcpt(string cmd_argv[], int cmd_argc)
{
    if(info->status==STATUS_INIT)
    {
        response = "503 Bad sequence of commands\r\n";
        slave.send(response);
    }
    else if(cmd_argc==2)
    {
        // add code here



    }
    else
    {
        response = "501 Invalid parameter\r\n";
        slave.send(response);
    }

}

void SMTPSession::doRset(string cmd_argv[], int cmd_argc)
{
    // add code here
}


void SMTPSession::saveEmail()
{
    // add code here to receive email data from client and save email

}

void SMTPSession::doData(string cmd_argv[], int cmd_argc)
{
    if(info->status!=STATUS_TO)
    {
        response = "503 Bad sequence of commands\r\n";
        slave.send(response);
    }
    else
    {
        // Add code here
    }

}
void SMTPSession::doNoop(string cmd_argv[], int cmd_argc)
{
    // add code here
}

void SMTPSession::doQuit(string cmd_argv[], int cmd_argc)
{
    // add code here
}


void SMTPSession::doUnknown(string cmd_argv[], int cmd_argc)
{
    response = "500 Unknown command\r\n" ;
    slave.send(response);
}


