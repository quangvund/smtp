#ifndef SMTPACCOUNT_H_
#define SMTPACCOUNT_H_
#include "serverconfig.h"

class SMTPAccount: public Account
{
private:
public:
    SMTPAccount();
    SMTPAccount(string username);
    ~SMTPAccount();
    string getEmailAddress() const;
    //const string& getDomain() const;
};

class SMTPServerConfig: public ServerConfig
{
private:
    unsigned short connTimeout = 0;
    string mailBox;
public:
    SMTPServerConfig();
    ~SMTPServerConfig();
    void setConnTimeout(unsigned short timeout);
    void setMailBox(const string& mailBox);
    const string& getMailBox() const;
    void loadAccountsFromFile(const string& fileName);
    void loadServerConfig(const string& confFileName);
    bool isValidEmailAccount(const string& email);
};


#endif // SMTPACCOUNT_H_
