#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <stdio.h>


static DWORD get_process_id(const wchar_t* process_name) {
    DWORD process_id = 0; 
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cout << "[-] Error getting handle snapshot.\n";
        return -1;
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32First(hSnapshot, &pe) != TRUE) {
        std::cout << "[-] Error viewing first entry of snapshot.\n";
        return -1;
    }
    else {
        do {
            if (_wcsicmp(process_name, pe.szExeFile) == 0) {
                std::cout << "[+] Process Found!" << "\n";
                process_id = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    //Cleanup
    CloseHandle(hSnapshot);


    if (process_id == 0) {
        std::cout << "[-] Error getting process id. Check if application is running.\n";
        return -1;
    }
    else {
        std::cout << "[+] Process Id: " << process_id << "\n";
        return process_id;
    }

}

static std::uintptr_t get_module_base(const DWORD process_id, const wchar_t* module_name) {
    std::uintptr_t module_base = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);
    
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return module_base;
    }

    MODULEENTRY32W me;
    me.dwSize = sizeof(me);

    if (Module32First(hSnapshot, &me) != TRUE) {
        std::cout << "[-] Error viewing first entry of snapshot.\n";
        return -1;
    }
    
    if (wcsstr(module_name, me.szModule) != nullptr) {
        module_base = reinterpret_cast<std::uintptr_t>(me.modBaseAddr);
    }
    else {
        do {
            if (wcsstr(module_name, me.szModule) != nullptr) {
                module_base = reinterpret_cast<std::uintptr_t>(me.modBaseAddr);
                break;
            }
        } while (Module32Next(hSnapshot, &me));
    }

    return module_base;
}


int main()
{
    const wchar_t* program_name = L"notepad.exe";
    std::cout << "[+] Starting UserMode.exe...\n";

    //Getting Process Id
    DWORD process_id = get_process_id(program_name);

    //Getting module base address
    uintptr_t module_base_address = get_module_base(process_id, program_name);
}
