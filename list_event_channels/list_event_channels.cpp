#include <windows.h>
#include <stdio.h>
#include <winevt.h>
#include <vector>
#include <string>
#include <iostream>

#pragma comment(lib, "wevtapi.lib")


static void print_error_code() {
    DWORD lastError = GetLastError();
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        lastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&lpMsgBuf,
        0, NULL);
    std::wcout << L"Error Code: " << lastError << L" - " << (LPWSTR)lpMsgBuf << std::endl;
}



EVT_HANDLE get_evt_enumr() {
    // Get a handle to an enumerator that contains all the names of the channels registered on the computer.
    // DOC: https://learn.microsoft.com/en-us/windows/win32/api/winevt/nf-winevt-evtopenchannelenum
    EVT_HANDLE hChannels = EvtOpenChannelEnum(
        NULL,              // A NULL SESSION WILL RESULT IN LOOKING FOR LOGS LOCALLY, NOT REMOTELY
        0                  // PER DOCS, THE flags MUST BE 0
        );
    if (hChannels == NULL) {
        std::wcout << L"ERROR: EvtOpenChannelEnum function failed." << std::endl;
        print_error_code();
        exit(1);
    }
    return hChannels;
}


void get_xpaths(EVT_HANDLE* evtHandleEnumr) {
    // MS DOCUMENTATION:
    // https://learn.microsoft.com/en-us/windows/win32/api/winevt/nf-winevt-evtnextchannelpath
    std::vector<std::string> xpaths;
    DWORD chanNameBufferSize;
    DWORD bufferSizeUsed;
    DWORD lastError;
    BOOL returnedStatus;
    while (true) {
        chanNameBufferSize = 0;
        bufferSizeUsed = 0;
        returnedStatus = EvtNextChannelPath(     // returnedStatus SHOULD ALWAYS BE TURE UNLESS AN UNEXPECTED ERROR
            *evtHandleEnumr,
            chanNameBufferSize,
            NULL,                               // BY SETTING TO NULL, bufferSizeUsed WILL BE SET TO THE NEEDED BUFFER SIZE FOR THE RETURNED NAME OF THE CHANNEL
            &bufferSizeUsed);                      // TAKES A POINTER TO THE DWORD
        lastError = GetLastError();
        if (lastError == ERROR_NO_MORE_ITEMS) {
            // printf("Completed enum\n");
            break;
        }
        if (!returnedStatus && lastError != ERROR_INSUFFICIENT_BUFFER) {
            std::cerr << "ERROR: Unexpected error while enumerating event log channel names." << std::endl;
            print_error_code();
            exit(1);
        }
        chanNameBufferSize = bufferSizeUsed;
        // AFTER RUNNING EvtNextChannelPath WITH NULL bufferSizeUsed IS SET TO THE BUFFER SIZE NEEDED FOR THE CHAN NAME STRING
        LPWSTR chanNameBuffer = (LPWSTR)malloc(bufferSizeUsed * sizeof(WCHAR));
        returnedStatus = EvtNextChannelPath(
            *evtHandleEnumr,
            chanNameBufferSize,
            chanNameBuffer,
            &bufferSizeUsed);
        wprintf(L"%s\n", chanNameBuffer);
        free(chanNameBuffer);                                       // DELETE THE BUFFER SO THE NEXT BUFFER CAN BE ALLOCATED WITH THE CORRECT SIZE
        //getchar();     //FOR TESTING OUTPUT
    }
}


int main(void) {
    EVT_HANDLE evtHandleEnumr = get_evt_enumr();
    get_xpaths(&evtHandleEnumr);
}

