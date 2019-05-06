#include "kill.h"


bool kill::kills(std::string name)
{
    const std::string program(name);

    if (!this->terminateProcess(program)) {
        return false;
    }

    return true;
}


std::string kill::WChar2Ansi(LPCWSTR pwszSrc)
{
    int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

    if (nLen <= 0) return std::string("");

    char* pszDst = new char[nLen];
    if (NULL == pszDst) return std::string("");

    WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
    pszDst[nLen - 1] = 0;

    std::string strTemp(pszDst);
    delete[] pszDst;

    return strTemp;
}

bool kill::traverseProcesses(std::map<std::string, int> &_nameID)
{
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cout << "CreateToolhelp32Snapshot Error!" << std::endl;;
        return false;
    }

    BOOL bResult = Process32First(hProcessSnap, &pe32);

    int num(0);

    while (bResult)
    {
        std::string name = this->WChar2Ansi(pe32.szExeFile);
        int id = pe32.th32ProcessID;

        //std::cout << "[" << ++num << "] : " <<"Process Name:"
        //  << name << "  " << "ProcessID:" << id<< std::endl;

        _nameID.insert(std::pair<string, int>(name, id)); //字典存储
        bResult = Process32Next(hProcessSnap, &pe32);
    }

    CloseHandle(hProcessSnap);

    return true;
}

bool kill::terminateProcess(const std::string _name)
{
    DWORD dwId;

    std::map<std::string, int> nameID;

    if (!this->traverseProcesses(nameID)) { //变量进程
        cout << "not find Processes Error!" << endl;
    }

    dwId = nameID[_name];

    BOOL bRet = FALSE;

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwId);
    if (hProcess != NULL)
        bRet = TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);

    if (bRet) {
        std::cout << "Terminate Process Success!" << std::endl;
        this->terminateProcess(_name);
    }
    else {
        std::cout << "Terminate Process Error!" << std::endl;
        return true;
    }
    return true;
}
