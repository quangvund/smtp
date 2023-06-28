#include <iostream>
#include "servercli.h"

using namespace std;

int main()
{
    cout << "Hello SMTP TCP/IP world!" << endl;
    ServerCLI cli;
    cli.run();
    return 0;
}
