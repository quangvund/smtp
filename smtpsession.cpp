#include <cstring>
#include <algorithm>    // std::find
#include <fstream>
#include <iostream>
#include "smtpsession.h"
#include "smtpserverconfig.h"
#include "random.h"


SMTPSession::SMTPSession(TcpSocket* slave,ServerConfig* conf):Session(slave,conf)
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

void SMTPSession::doHelo(char* cmd_argv[], int cmd_argc)
{
    response = "250 Welcome to SMTP Server v1.0\r\n";
    slave->send(response);
}

void SMTPSession::doMail(char* cmd_argv[], int cmd_argc)
{
    /*if(info->status==STATUS_INIT){
        slave->send("503 Bad sequence of commands\r\n");
    }
    else */
    if(cmd_argc==2)
    {
        if(strncmp(cmd_argv[1],"FROM:",5)!=0)
        {
            response = "501 Invalid parameter\r\n";
            slave->send(response);
            return;
        }
        else
        {
            char* fromAddr = cmd_argv[1] + 5;
            // trim all space on the left
            // while(*fromAddr==' ')fromAddr++;
            // trim < and >
            char* p = strchr(fromAddr,'<');
            if(p!=NULL)
            {
                char* q = strchr(fromAddr,'>');
                if(q!=NULL)
                {
                    *q = 0;
                    fromAddr = p+1;
                }
                else
                {
                    response = "501 Invalid parameter\r\n";
                    slave->send(response);
                    return;
                }
            }
            //trim all space on the right
            int len = strlen(fromAddr);
            int i=1;
            while(*(fromAddr+len-i)==' ')
            {
                *(fromAddr+len-i) = 0;
                i++;
            }
            // check email format
            p = strchr(fromAddr,'@');
            if(p==NULL)
            {
                response = "501 Invalid parameter\r\n";
                slave->send(response);
                return;
            }
            else
            {
                char* q = strchr(p+1,'.');
                if(q==NULL)
                {
                    response = "501 Invalid parameter\r\n";
                    slave->send(response);
                    return;
                }
            }
            // check whether user and domain name is valid or not
            SMTPServerConfig* smtpConf = dynamic_cast<SMTPServerConfig*>(this->conf);
            if(smtpConf->isValidEmailAccount(fromAddr))
            {
                info->fromAddress = fromAddr;
                response = "250 Sender \"" + info->fromAddress + "\" OK...\r\n";
                slave->send(response);
                info->status = STATUS_FROM;
            }
            else
            {
                response = "504 Not local user\r\n";
                slave->send(response);
            }
        }
    }
    else
    {
        response = "501 Invalid parameter\r\n";
        slave->send(response);
    }
}

void SMTPSession::doRcpt(char* cmd_argv[], int cmd_argc)
{
    if(info->status==STATUS_INIT)
    {
        response = "503 Bad sequence of commands\r\n";
        slave->send(response);
    }
    else if(cmd_argc==2)
    {
        //...
        if(strncmp(cmd_argv[1],"TO:",3)!=0)
        {
            response= "501 Invalid parameter\r\n";
            slave->send(response);
        }
        else
        {
            char* toAddr = cmd_argv[1] + 3;

            // trim all space on the left
            // while(*fromAddr==' ')fromAddr++;
            // trim < and >
            char* p = strchr(toAddr,'<');
            if(p!=NULL)
            {
                char* q = strchr(toAddr,'>');
                if(q!=NULL)
                {
                    *q = 0;
                    toAddr = p+1;
                }
                else
                {
                    response = "501 Invalid parameter\r\n";
                    slave->send(response);
                    return;
                }
            }
            //trim all space on the right
            int len = strlen(toAddr);
            int i=1;
            while(*(toAddr+len-i)==' ')
            {
                *(toAddr+len-i) = 0;
                i++;
            }
            // check email format
            p = strchr(toAddr,'@');
            if(p==NULL)
            {
                response = "501 Invalid parameter\r\n";
                slave->send(response);
                return;
            }
            else
            {
                char* q = strchr(p+1,'.');
                if(q==NULL)
                {
                    response = "501 Invalid parameter\r\n";
                    slave->send(response);
                    return;
                }
            }
            // check whether user and domain name is valid or not
            SMTPServerConfig* smtpConf = dynamic_cast<SMTPServerConfig*>(this->conf);
            if(smtpConf->isValidEmailAccount(toAddr))
            {
                string toAddress(toAddr);
                vector<string>::iterator it;
                it = find(info->toAddress.begin(),info->toAddress.end(),toAddress);
                if(it==info->toAddress.end())
                    info->toAddress.push_back(toAddress);
                response = "250 Recipient \"" + toAddress + "\" OK...\r\n";
                slave->send(response);
                info->status = STATUS_TO;
            }
            else
            {
                response = "504 Not local user\r\n";
                slave->send(response);
            }
        }
    }
    else
    {
        response = "501 Invalid parameter\r\n";
        slave->send(response);
    }

}

void SMTPSession::doRset(char* cmd_argv[], int cmd_argc)
{
    this->reset();
    response = "250 OK\r\n";
    slave->send(response);
}

void SMTPSession::doVrfy(char* cmd_argv[], int cmd_argc)
{
    if(info->status==STATUS_INIT)
    {
        response = "503 Bad sequence of commands\r\n";
        slave->send(response);
    }
    else if(cmd_argc==2)
    {
        string reply;
        SMTPAccount* acc  = (SMTPAccount*)conf->getAccount(cmd_argv[1]);
        if(acc)
        {
            response = "220 User " + acc->getEmailAddress() + " OK\r\n";
            slave->send(response);
        }
        else
        {
            response = "504 Unknown user\r\n";
            slave->send(response);
        }
    }
    else
    {
         response = "501 Invalid parameter\r\n";
         slave->send(response);
    }
}
void SMTPSession::saveEmail()
{
    vector<ofstream*> files;
    string filename;
    for(auto& x:info->toAddress){
        filename = ".\\mail\\" + x + "\\" + createAlphabetDigitString(12) + ".eml";
        ofstream* file = new ofstream(filename,ios::out|ios::binary);
        files.push_back(file);
    }
    char buffer[1024];
    int byte_recv = slave->recvLine(buffer, 1024);
    bool done = false;
    while(byte_recv>0 && !done)
    {
        if(byte_recv==3 && strncmp(buffer,".\r\n",3)==0) // end of email
        {
            done = true;
            break;
        }
        else // ghi ra file
        {
             for(auto& f:files)
             {
                 f->write(buffer, byte_recv);
             }
             byte_recv = slave->recvLine(buffer, 1024);
        }
    }
    if(done)
    {
        for(auto& f:files)
        {
            f->close();
            delete f;
        }
        response = "250 Message accepted for delivery.\r\n";
        slave->send(response);
    }
    else if(byte_recv==0)
    {
        cerr << "Peer reset connection\n";
    } else if(byte_recv < 0)
    {
        cerr << "Socket error when receiving email\n";
    }
}

void SMTPSession::doData(char* cmd_argv[], int cmd_argc)
{
    if(info->status!=STATUS_TO)
    {
        response = "503 Bad sequence of commands\r\n";
        slave->send(response);
    }
    else
    {
        response = "354 Enter message. End with CRLF.CRLF...\r\n";
        slave->send(response);
        saveEmail();
        reset();
    }

}
void SMTPSession::doNoop(char* cmd_argv[], int cmd_argc)
{
    response = "250 No operation\r\n";
    slave->send(response);
}

void SMTPSession::doQuit(char* cmd_argv[], int cmd_argc)
{
    response = "221 Good bye\r\n";
    slave->send(response);
    quitSession = true;
}
void SMTPSession::doUnknown(char* cmd_argv[], int cmd_argc)
{
    response = "500 Unknown command\r\n" ;
    slave->send(response);
}


