#ifndef KILL_H
#define KILL_H

#include <QCoreApplication>
#include <iostream>
#include <string>
//
#include <windows.h>
#include <TlHelp32.h>

using namespace std;
class kill
{
public:
    bool kills(std::string name);
    std::string WChar2Ansi(LPCWSTR pwszSrc);
    bool traverseProcesses(std::map<std::string, int>& _nameID);
    bool terminateProcess(const std::string _name);


};

#endif // KILL_H
