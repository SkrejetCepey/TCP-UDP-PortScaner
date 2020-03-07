#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

char knockIP[] = "127.0.0.1";
int firstknockPORT = 0;
int secondknockPORT = 0;
string flagChoose = "";

SOCKET my_sock;
sockaddr_in dest_addr;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

void dataReader()
{
    cout << "TCP or UDP ?" << endl;
    cin >> flagChoose;
    cout << "Enter your IP for knocking: " << endl;
    cin >> knockIP;
    cout << "Input first PORT for knocking: " << endl;
    cin >> firstknockPORT;
    if (firstknockPORT > 65535)
        firstknockPORT = 65535;
    cout << "Input second PORT for knocking: " << endl;
    cin >> secondknockPORT;
    if (secondknockPORT > 65535)
        secondknockPORT = 65535;
    cout << endl;
}

void PortKnockingTCP(int firstKnockPort, int secondKnockPort)
{
    SetConsoleTextAttribute(console, 6);
    cout << "TCP knocker ready!" << endl;
    cout << "Knocking..." << endl;
    SetConsoleTextAttribute(console, 7);
    cout << "---------------" << endl;
    DWORD dwerr;
    while (firstKnockPort <= secondKnockPort)
    {
        if (connect(my_sock, (sockaddr*)&dest_addr, sizeof(dest_addr)))
        {
            dwerr = WSAGetLastError();
            printf("Connect error %d\n", dwerr);
            if (dwerr == 10049)
            {
                SetConsoleTextAttribute(console, FOREGROUND_RED);
                cout << "Invalid IP" << endl;
                SetConsoleTextAttribute(console, 7);
                break;
            }

            SetConsoleTextAttribute(console, FOREGROUND_RED);
            cout << firstKnockPort << " port close." << endl;
            SetConsoleTextAttribute(console, 7);
            cout << "---------------" << endl;

            firstKnockPort++;
            dest_addr.sin_port = htons(firstKnockPort);
        }
        else
        {
            dwerr = WSAGetLastError();
            if (dwerr != 0)
            {
                printf("Connect error %d\n", dwerr);
                if (dwerr == 10049)
                {
                    SetConsoleTextAttribute(console, FOREGROUND_RED);
                    cout << "Invalid IP" << endl;
                    SetConsoleTextAttribute(console, 7);
                    break;
                }
            }

            SetConsoleTextAttribute(console, FOREGROUND_GREEN);
            cout << firstKnockPort << " port open." << endl;
            SetConsoleTextAttribute(console, 7);
            cout << "---------------" << endl;

            firstKnockPort++;
            dest_addr.sin_port = htons(firstKnockPort);

            closesocket(my_sock);
            my_sock = socket(AF_INET, SOCK_STREAM, 0);
        }
    }
    if (firstKnockPort > secondKnockPort)
    {
        SetConsoleTextAttribute(console, 6);
        cout << "TCP knocker: the process is complete!" << endl;
        cout << endl << "All ports was knocked successful!" << endl;
        SetConsoleTextAttribute(console, 7);
    }
}

void PortKnockingUDP(int firstKnockPort, int secondKnockPort)
{
    SetConsoleTextAttribute(console, 6);
    cout << "UDP knocker ready!" << endl;
    cout << "Knocking..." << endl;
    SetConsoleTextAttribute(console, 7);
    cout << "---------------" << endl;
    my_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (my_sock < 0)
    {
        SetConsoleTextAttribute(console, FOREGROUND_RED);
        printf("Socket() error %d\n", WSAGetLastError());
        SetConsoleTextAttribute(console, 7);
        return;
    }
    while (firstKnockPort <= secondKnockPort)
    {
        char buff[10 * 1014];
        if (WSAStartup(0x202, (WSADATA*)&buff[0]))
        {
            SetConsoleTextAttribute(console, FOREGROUND_RED);
            std::printf("WSAStartup error: %d\n", WSAGetLastError());
            SetConsoleTextAttribute(console, 7);
            break;
        }

        dest_addr.sin_port = htons(firstKnockPort);

        int OptVal = 500;
        int OptLen = sizeof(OptVal);

        sendto(my_sock, &buff[0], strlen(&buff[0]), 0, (sockaddr*)&dest_addr, sizeof(dest_addr));
        setsockopt(my_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&OptVal, OptLen);

        sockaddr_in server_addr;
        int server_addr_size = sizeof(server_addr);

        int n = recvfrom(my_sock, &buff[0], sizeof(buff) - 1, 0, (sockaddr*)&server_addr, &server_addr_size);

        //Sleep(1000);

        if (n == SOCKET_ERROR)
        {
            SetConsoleTextAttribute(console, FOREGROUND_RED);
            cout << firstKnockPort << " port close." << endl;
            SetConsoleTextAttribute(console, 7);
            cout << "----------" << endl;
            WSAGetLastError();
            firstKnockPort++;
        }
        else
        {
            SetConsoleTextAttribute(console, FOREGROUND_GREEN);
            cout << firstKnockPort << " port open." << endl;
            SetConsoleTextAttribute(console, 7);
            cout << "----------" << endl;
            firstKnockPort++;
        }
        if (firstKnockPort > secondKnockPort)
        {
            SetConsoleTextAttribute(console, 6);
            cout << "UDP knocker: the process is complete!" << endl;
            cout << endl << "All ports was knocked successful!" << endl;
            SetConsoleTextAttribute(console, 7);
        }
    }
}

int main(int argc, char* argv[])
{
    char buff[1024];
    printf("TCP/UDP PORT SCANER\n");

    dataReader();

    if (WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        SetConsoleTextAttribute(console, FOREGROUND_RED);
        printf("WSAStart error %d\n", WSAGetLastError());
        SetConsoleTextAttribute(console, 7);
        return -1;
    }

    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0)
    {
        SetConsoleTextAttribute(console, FOREGROUND_RED);
        printf("Socket() error %d\n", WSAGetLastError());
        SetConsoleTextAttribute(console, 7);
        return -1;
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(firstknockPORT);
    HOSTENT* hst;

    if (inet_addr(knockIP) != INADDR_NONE)
        dest_addr.sin_addr.s_addr = inet_addr(knockIP);
    else
        if (hst = gethostbyname(knockIP))
        {
            ((unsigned long*)&dest_addr.sin_addr)[0] = ((unsigned long**)hst->h_addr_list)[0][0];
        }
        else
        {
            SetConsoleTextAttribute(console, FOREGROUND_RED);
            printf("Invalid address %s\n", knockIP);
            SetConsoleTextAttribute(console, 7);
            closesocket(my_sock);
            WSACleanup();
            return -1;
        }
    while (flagChoose!="quit")
    {
        if (flagChoose == "TCP")
        {
            PortKnockingTCP(firstknockPORT, secondknockPORT);
            flagChoose = "quit";
        }
        else if (flagChoose == "UDP")
        {
            PortKnockingUDP(firstknockPORT, secondknockPORT);
            flagChoose = "quit";
        }
        else
        {
            cout << "Your choose about TCP/UDP uncorrect, pls try again![or 'quit' for exit]" << endl;
            cin >> flagChoose;
        }
    }
    cin.get();
    return 0;
}