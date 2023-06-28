#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
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
    initCmd();
    conf = new SMTPServerConfig();
}



void SMTPServer::loadServerConfig(const string& confFileName)
{
    ifstream f(confFileName,ios::in);
    if(!f.good())
    {
        throw ServerConfigException("can not open SMTP configure file\n");
    }
    else
    {
        char str[256];
        char *name,*value;
        while(!f.eof())
        {
            f.getline(str,256);
            if(readAttribute(str,&name,&value,"="))
            {
                if(strcmp(name,"port")==0)
                {
                    port = atoi(value);
                }
                else if(strcmp(name,"conn-timeout")==0)
                {
                   connTimeout = atoi(value);
                   getServerConfig()->setConnTimeout(connTimeout);
                }
                else if(strcmp(name,"mailbox")==0)
                {
                   getServerConfig()->setMailBox(value);
                }
                else if(strcmp(name,"logging")==0)
                {
                    if(strcmp(value,"yes")==0)
                    {
                        logging = true;
                    }else
                    {
                        logging = false;
                    }
                }
                else if(strcmp(name,"logfile")==0)
                {
                    logMutex = new std::mutex();
                    logFile = new std::fstream(value,ios::app);
                }
            }
        }
        f.close();
    }
}

void SMTPServer::configServer()
{
    try
    {
        loadServerConfig(SMTP_CONF_FILE_NAME);
        conf->loadAccountsFromFile(SMTP_ACC_FILE_NAME);
    }
    catch(ServerConfigException& e)
    {
        cerr << e.what() << endl;
    }
}

void SMTPServer::cleanServer()
{
    if(logMutex)
    {
        delete logMutex;
        logMutex = NULL;
    }
    if(logFile)
    {
        logFile->close();
        delete logFile;
        logFile = NULL;
    }
    conf->removeAllAccount();
}

SMTPServer::~SMTPServer()
{
    if(isRunning())
    {
        stop();
        cout<< "SMTP Server stopped!\n";
    }
    if(logMutex)
        delete logMutex;
    if(logFile)
    {
        logFile->close();
        delete logFile;
    }
}

void SMTPServer::initCmd()
{
    addCmd(HELO, FUNC_CAST(&SMTPSession::doHelo));
    addCmd(QUIT, FUNC_CAST(&SMTPSession::doQuit));
    addCmd(NOOP, FUNC_CAST(&SMTPSession::doNoop));
    addCmd(VRFY, FUNC_CAST(&SMTPSession::doVrfy));
    addCmd(MAIL, FUNC_CAST(&SMTPSession::doMail));
    addCmd(RCPT, FUNC_CAST(&SMTPSession::doRcpt));
    addCmd(RSET, FUNC_CAST(&SMTPSession::doRset));
    addCmd(DATA, FUNC_CAST(&SMTPSession::doData));
}

// viet ham doc request tu client
int SMTPServer::readCmd(TcpSocket* slave, char* buffer, int buflen)
{
    try
    {
        int byteRead = slave->recvLine(buffer,buflen,connTimeout);
        if(byteRead >= 2) // loai bo CRLF (\r\n) o cuoi xau chua lenh
        {
            buffer[byteRead-2] = 0;
            return byteRead - 2;
        }
        else
        {
            buffer[0] = 0;
            return 0;
        }
    }
    catch(SocketException&e )
    {
        cerr << e.what() << endl;
        return -1;
    }
}


// viet ham phan tich cu phap
unsigned short SMTPServer::parseCmd(char *sCmdBuf, int len, char * cmd_argv[], int& cmd_argc)
{
    char * arg;
    cmd_argc = 0; // so tham so = 0

    if(len<=0)
        return SERVER_CMD_UNKNOWN;  // do dai cua xau chua lenh > 0

    arg = strtok (sCmdBuf,SMTP_DELIMITER); // phan tich xau ki tu trong sCmdBuf
    while (arg != NULL)
    {
        cmd_argv[cmd_argc++] = arg;
        arg = strtok (NULL, SMTP_DELIMITER);
    }
    int i;
    if(cmd_argc>0)
    {
        for(i = 0; i<this->numCmd; i++)
        {
            if(stricmp(cmd_argv[0],cmdNameList[i].c_str())==0)
                return i ;
        }
    }
    return SERVER_CMD_UNKNOWN;
}

void SMTPServer::startNewSession(TcpSocket* slave)
{
    seed();
    SMTPSession* session = new SMTPSession(slave,conf);
    char cmdBuffer[SERVER_CMD_BUF_LEN];
    int cmdLen;
    char* cmdArgv[SERVER_CMD_ARG_NUM];
    int cmdArgc;
    unsigned short cmdId;
    ostringstream log;
    try
    {
        slave->send(SMTP_GREETING);
        if(logging)
        {
            log << "--- Session start ---\n";
            log << "Remote IP: " << slave->getRemoteAddress() << endl;
            log << "Time: " << endl;
            log << "Server: " << SMTP_GREETING ;
        }
        while(!session->isQuit())
        {
            // Nhan lenh
            cmdBuffer[0] = 0;
            cmdLen = readCmd(slave, cmdBuffer, SERVER_CMD_BUF_LEN);
            if(logging)
            {
                log << "Client: " << cmdBuffer << endl ;
            }
            // Kiem tra lenh
            if(cmdLen <= 0)
                break;
            // Phan tich lenh
            cmdId = parseCmd(cmdBuffer, cmdLen, cmdArgv, cmdArgc );
            // Thuc hien lenh
            doCmd(session,cmdId,cmdArgv,cmdArgc);
            if(logging)
            {
                log << "Server: " << session->getResponse();
            }
        }
        // session finish
        delete session;
        if(logging)
        {
            log << "--- Session end ---\n";
            logMutex->lock();
            if(logFile)
                (*logFile) << log.str();
            else
                cout << log.str();
            logMutex->unlock();
        }

    }
    catch(SocketException&e)
    {
        cerr << e.what() << endl;
        delete session;
    }
}

