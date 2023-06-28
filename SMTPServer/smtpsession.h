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


#ifndef QUIT
    #define QUIT  "QUIT"
#endif // QUIT


struct SMTPSessionInfo
{
    uint8_t  status;
    string fromAddress;
    string toAddress;
};

class SMTPSession : public Session
{
    SMTPSessionInfo* info;
    string response;
    void reset();
    void saveEmail();
    SMTPServerConfig* getServerConfig(){return (SMTPServerConfig*)conf;}
public:
    SMTPSession(const TcpSocket& slave,ServerConfig* conf);
    ~SMTPSession();
    const string& getResponse() const {return response;}
    void doHelo(string cmd_argv[], int cmd_argc);
    void doQuit(string cmd_argv[], int cmd_argc);
    void doMail(string cmd_argv[], int cmd_argc);
    void doRcpt(string cmd_argv[], int cmd_argc);
    void doRset(string cmd_argv[], int cmd_argc);
    void doData(string cmd_argv[], int cmd_argc);
    void doNoop(string cmd_argv[], int cmd_argc);
    void doUnknown(string cmd_argv[], int cmd_argc);
    // void setCurrCmd(const char* cmdName);
};



#endif // SMTPSESSION_H_INCLUDED
