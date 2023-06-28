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
    string mailBox;
public:
    SMTPServerConfig();
    ~SMTPServerConfig();
    void setMailBox(const string& mailBox);
    const string& getMailBox() const;
    bool loadAccountsFromFile(const string& fileName);
    bool isValidEmailAccount(const string& email);
};


#endif // SMTPACCOUNT_H_
