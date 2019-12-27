/*
 filename:		help.cpp
 author:		AngieJC
 date:			2019/12/23
 description:	用于输出帮助信息
*/

#include <iostream>

using namespace std;

void help()
{
    cout << "Author:\t\tAngieJC" << endl;
    cout << "Email:\t\thtk90uggk@163.com" << endl;
    cout << "Description:\tCourse design of software engineering" << endl;
    cout << "Github:\t\thttps://github.com/AngieBare/netfish" << endl;
    cout << "\n\n";
    cout << "\033[36m<:)))><" << "\t\t" << "<。)#)))≤" << endl;
    cout << "\t" << "<()>+++<" << "\t" << "<・ )))><<\033[0m" << endl;
    cout << endl << endl << endl;
    cout << "connect to somewhere:\tnf hostname port" << endl;
    cout << "listen for inbound:\tnf -lp port" << endl;
    cout << "l:\tlisten\t\t\tlisten mode, for inbound connects" << endl;
    cout << "p:\tport\t\t\tlocal or remote port number" << endl;
    cout << "e:\tprog\t\t\tinbound program to exec [dangerous!]" << endl;
    cout << "h:\thelp\t\t\tthis cruft" << endl;
    cout << "z:\tzero-I/O\t\tzero-I/O mode [used for scanning]" << endl;
    cout << "a:\tarp-find\t\tfind live host in LAN" << endl;
    cout << "w:\twait\t\t\twait for some time" << endl;
    cout << "t:\ttraceroute\t\ttrace the route from this PC to server" << endl;
    cout << "f:\tFTP\t\t\tdownload or upload files" << endl;
}
