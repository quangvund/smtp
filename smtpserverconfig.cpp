#include <fstream>
#include <cstring>
#include "smtpserverconfig.h"


SMTPAccount::SMTPAccount()
{
}
SMTPAccount::SMTPAccount(string username):Account(username)
{

}

SMTPAccount::~SMTPAccount()
{
}

string SMTPAccount::getEmailAddress() const
{
    return this->username;
}


SMTPServerConfig::SMTPServerConfig()
{
}

SMTPServerConfig::~SMTPServerConfig()
{
}


bool SMTPServerConfig::isValidEmailAccount(const string& email)
{
    return isValidUser(email);
}

void SMTPServerConfig::setConnTimeout(unsigned short timeout)
{
    connTimeout = timeout;
}

void SMTPServerConfig::setMailBox(const string& mailBox)
{
    this->mailBox = mailBox;
}
const string& SMTPServerConfig::getMailBox() const
{
    return mailBox;
}
void SMTPServerConfig::loadServerConfig(const string& confFileName)
{

}

void SMTPServerConfig::loadAccountsFromFile(const string& fileName)
{
    ifstream f(fileName,ios::in);
    if(!f.good())
    {
        throw ServerConfigException("can not open SMTP account list file\n");
    }
    else
    {
        char str[128];
        while(!f.eof())
        {
            f.getline(str,128);
            if(strlen(str)>0)
            {
                SMTPAccount* acc = new SMTPAccount();
                acc->setUserName(str);
                addAccount(acc);
            }
        }
        f.close();
    }
}
