#ifndef SMTPSESSION_H_INCLUDED
#define SMTPSESSION_H_INCLUDED
#include <vector>
#include "tcpserversocket.h"
#include "session.h"
#include "smtpserverconfig.h"

#define STATUS_INIT     0
#define STATUS_FROM     1
#define STATUS_TO       2


// cac lenh trong ban tin SMTP request
#define HELO  "HELO"
#define MAIL  "MAIL"
#define RCPT  "RCPT"
#define RSET  "RSET"
#define DATA  "DATA"
#define NOOP  "NOOP"
#define VRFY  "VRFY"

#ifndef QUIT
    #define QUIT  "QUIT"
#endif // QUIT


struct SMTPSessionInfo
{
    uint8_t  status;
    string fromAddress;
    vector<string> toAddress;
};

class SMTPSession : public Session
{
    SMTPSessionInfo* info;
    string response;
    void reset();
    void saveEmail();
    SMTPServerConfig* getServerConfig(){return (SMTPServerConfig*)conf;}
public:
    SMTPSession(TcpSocket* slave,ServerConfig* conf);
    ~SMTPSession();
    const string& getResponse() const {return response;}
    void doHelo(char* cmd_argv[], int cmd_argc);
    void doQuit(char* cmd_argv[], int cmd_argc);
    void doMail(char* cmd_argv[], int cmd_argc);
    void doRcpt(char* cmd_argv[], int cmd_argc);
    void doRset(char* cmd_argv[], int cmd_argc);
    void doData(char* cmd_argv[], int cmd_argc);
    void doVrfy(char* cmd_argv[], int cmd_argc);
    void doNoop(char* cmd_argv[], int cmd_argc);
    void doUnknown(char* cmd_argv[], int cmd_argc);
    // void setCurrCmd(const char* cmdName);
};



#endif // SMTPSESSION_H_INCLUDED
