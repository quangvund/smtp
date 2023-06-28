#include <fstream>
#include <iostream>
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


void SMTPServerConfig::setMailBox(const string& mailBox)
{
    this->mailBox = mailBox;
}

const string& SMTPServerConfig::getMailBox() const
{
    return mailBox;
}


bool SMTPServerConfig::loadAccountsFromFile(const string& fileName)
{
    ifstream f(fileName,ios::in);
    if(!f.good())
    {
        cerr << "Error: cannot open SMTP account list file" << endl;
        return false;
    }
    else
    {
        string str;
        while(!f.eof())
        {
            getline(f,str);
            if(str.length()>0)
            {
                SMTPAccount* acc = new SMTPAccount();
                acc->setUserName(str);
                addAccount(acc);
            }
        }
        f.close();
        return true;
    }

}
