#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "smtpserver.h"
#include "random.h"


const char  SMTP_CONF_FILE_NAME[] = "smtp.conf";
const char  SMTP_ACC_FILE_NAME[] = "smtpaccount.conf";

const char SMTP_DELIMITER[]= " ";
const char EOL[] = "\r\n";
const char BLANK_LINE[] = "\r\n";


const char SMTP_GREETING[] = "220 Simple SMTP Server v1.0 is ready\r\n";


SMTPServer::SMTPServer(unsigned short port):TCPServer(port)
{
    initCmd(); // khoi tao lenh ma Server co the xu ly
    conf = new SMTPServerConfig();
}

bool SMTPServer::loadServerConfig(const string& confFileName)
{
    ifstream f(confFileName,ios::in);
    if(!f.good())
    {
        cerr << "Error: cannot open SMTP configure file" << endl;
        return false;
    }
    else
    {
        string str;
        string name,value;
        while(!f.eof())
        {
            getline(f,str);
            if(readAttribute(str,name,value))
            {
                if(name=="port")
                {
                    port = stoi(value);
                }
                else if(name == "conn-timeout")
                {
                   int connTimeout = stoi(value);
                   getServerConfig()->setTimeOut(connTimeout);
                }
                else if(name=="mailbox")
                {
                   getServerConfig()->setMailBox(value);
                }
            }
        }
        f.close();
        return true;
    }
}

bool SMTPServer::configServer()
{
    return (loadServerConfig(SMTP_CONF_FILE_NAME) && conf->loadAccountsFromFile(SMTP_ACC_FILE_NAME));
}

void SMTPServer::cleanServer()
{
    conf->removeAllAccount();
}

SMTPServer::~SMTPServer()
{
    if(isRunning())
    {
        stop();
        cout<< "SMTP Server stopped!\n";
    }
}

void SMTPServer::initCmd() // them cac lenh giao thuc va ham xu ly tuong ung
{
    addCmd(HELO, FUNC_CAST(&SMTPSession::doHelo));
    addCmd(QUIT, FUNC_CAST(&SMTPSession::doQuit));
    addCmd(NOOP, FUNC_CAST(&SMTPSession::doNoop));
    addCmd(MAIL, FUNC_CAST(&SMTPSession::doMail));
    addCmd(RCPT, FUNC_CAST(&SMTPSession::doRcpt));
    addCmd(RSET, FUNC_CAST(&SMTPSession::doRset));
    addCmd(DATA, FUNC_CAST(&SMTPSession::doData));
}

// viet ham doc request tu client
int SMTPServer::readCmd(TcpSocket& slave, string& cmdLine)
{
    try
    {
        char cmdBuffer[256];
        int byteRead = slave.recvLine(cmdBuffer,256);
        if(byteRead >= 2) // loai bo CRLF (\r\n) o cuoi xau chua lenh
        {
            cmdBuffer[byteRead-2] = 0;
            cmdLine = cmdBuffer;
        }
        return byteRead;
    }
    catch(SocketException&e )
    {
        cerr << e.what() << endl;
        return -1;
    }
}


// viet ham phan tich cu phap
unsigned short SMTPServer::parseCmd(const string& cmdLine, string cmd_argv[], int& cmd_argc)
{
    cmd_argc = 0; // so tham so = 0

    // read command arguments into array of string
    cmd_argv[cmd_argc].clear();
    istringstream iss(cmdLine);
    while (cmd_argc < SERVER_CMD_ARG_NUM && iss.good())
    {
        iss >> cmd_argv[cmd_argc];
        cmd_argc++;
    }

    // check command name and return command ID
    if(cmd_argc>0 && !cmd_argv[0].empty())
    {
        for(int i = 0; i<this->numCmd; i++)
        {
            if(cmd_argv[0] == cmdNameList[i])
                return i ;
        }
    }
    return SERVER_CMD_UNKNOWN;
}

void SMTPServer::startNewSession(TcpSocket slave)
{
    // create new session
    SMTPSession* session = new SMTPSession(slave,conf);
    string cmdLine;
    string cmdArgv[SERVER_CMD_ARG_NUM];
    int cmdArgc;  // number of command arguments
    unsigned short cmdId; // ID of command
    int cmdLen; // length of a SMTP command
    try
    {
        slave.send(SMTP_GREETING);
        while(!session->isQuit())
        {
            // Nhan lenh
            cmdLen = readCmd(slave,cmdLine);
            // Kiem tra lenh
            if(cmdLen <= 0)
                break;
            // Phan tich lenh
            cmdId = parseCmd(cmdLine, cmdArgv, cmdArgc );
            // Thuc hien lenh trong session
            doCmd(session,cmdId,cmdArgv,cmdArgc);
        }
        // session finish
        delete session;
    }
    catch(SocketException&e)
    {
        cerr << e.what() << endl;
        delete session;
    }
}

