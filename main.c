#include <stdio.h>
#include <windows.h>


typedef struct _BP_ENTRY {  // Create a struct for breakpoint entry to save the addr & the original byte
    LPVOID address;
    BYTE original;
} BP_ENTRY;

BP_ENTRY en = {0};  // init the struct with empty values but sized

char newText[] = "pwned";

LONG CALLBACK VehHandler(EXCEPTION_POINTERS *info) {
    if(info->ExceptionRecord->ExceptionCode != EXCEPTION_BREAKPOINT) 
    return EXCEPTION_CONTINUE_SEARCH;

    LPVOID rip = (LPVOID)info->ContextRecord->Rip;
    if(rip != en.address)
    return EXCEPTION_CONTINUE_SEARCH;

    printf("[VEH] Breakpoint hit at %p\n", rip);

    DWORD oldProtect;           // save the old protection
    if(!VirtualProtect(en.address, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) { // force the rxw on the address
        printf("Error VP, %lu", GetLastError());
        return 1;
    } 
    memcpy(en.address, &en.original, 1);    // restore original byte to the address

    VirtualProtect(en.address, 1, oldProtect, &oldProtect); // reset the old protection on the address

    info->ContextRecord->Rdx = (DWORD64)newText;    // replace the text by the new text (pwned), we need cast DWORD64 bcs its RDX so a pointer/register

    info->ContextRecord->Rip = (DWORD64)en.address;

    return EXCEPTION_CONTINUE_EXECUTION;

}

int main() {


    AddVectoredExceptionHandler(1, VehHandler);


    HMODULE hUser32 = LoadLibraryA("user32.dll");   // load the user32.dll in our program and get a handle of the module
    if(!hUser32) {
        printf("Error LoadLibraryA %lu", GetLastError());
        return 1;
    }

    PVOID originalAddr = GetProcAddress(hUser32, "MessageBoxA");    // get the original address of MessageBoxA for the process of the user32.dll, 
    if(!originalAddr) {
        printf("Error GetProcAddr %lu", GetLastError());
        return 1;
    }
    en.address = originalAddr;  // init the struct address is the original Address

    memcpy(&en.original, originalAddr, 1);        // copy the original byte for restore the original func later

    BYTE int3 = 0xCC;               // 0xCC is the int3 breakpoint instruction
    DWORD oldProtect;           // save the old protection
    if(!VirtualProtect(originalAddr, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) { // force the rxw on the address
        printf("Error VP, %lu", GetLastError());
        return 1;
    } 
    memcpy(originalAddr, &int3, 1);               // copy the int3 instruction on the 1st byte of the address
    printf("[BREAKPOINT] INT3 at: %p\n", originalAddr);

        MessageBoxA(
        NULL,
        "Hello",
        "Title",
        MB_OK
    );



    printf("[BREAKPOINT] Done: %p\n", en.address);

    RemoveVectoredExceptionHandler(VehHandler);
    return 0;
}
