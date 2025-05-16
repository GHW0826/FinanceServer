// EchoClient_Ex.cpp
#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <iostream>
#include <istream>
#include <thread>
#include <string>
#include <atomic>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

static const char* SERVER_IP = "127.0.0.1";
static const int SERVER_PORT = 9000;
static const int BUF_SIZE = 1024;

LPFN_CONNECTEX g_pConnectEx = NULL;

enum class IOType { IO_NONE, IO_CONNECT, IO_RECV, IO_SEND };

struct OverlappedEx {
    OVERLAPPED  overlapped;
    SOCKET      sock;
    WSABUF      wsaBuf;
    char        buffer[BUF_SIZE];
    IOType      ioType;
};

HANDLE g_iocp = NULL;
std::atomic<bool> g_running{ true };

bool InitWinSock();
bool LoadConnectEx(SOCKET s);
void WorkerThread();

// ---------------- Functions ----------------

bool InitWinSock() 
{
    WSADATA wsa;
    // 윈속 초기화 함수 호출
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "WSAStartup failed.\n";
        return false;
    }
    return true;
}

bool LoadConnectEx(SOCKET s) {
    GUID guidConnectEx = WSAID_CONNECTEX;
    DWORD bytes = 0;
    int ret = WSAIoctl(
        s,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &guidConnectEx, sizeof(guidConnectEx),
        &g_pConnectEx, sizeof(g_pConnectEx),
        &bytes, NULL, NULL
    );
    if (ret == SOCKET_ERROR) {
        std::cout << "WSAIoctl for ConnectEx failed.\n";
        return false;
    }
    return true;
}

// I/O Completion 처리
void WorkerThread() {
    while (g_running) {
        DWORD dwBytes = 0;
        ULONG_PTR key = 0;
        LPOVERLAPPED pOv = nullptr;

        BOOL ret = GetQueuedCompletionStatus(g_iocp, &dwBytes, &key, &pOv, INFINITE);
        if (!ret || dwBytes == 0) {
            if (!g_running) break;
            if (pOv) {
                OverlappedEx* ovEx = (OverlappedEx*)pOv;
                closesocket(ovEx->sock);
                delete ovEx;
            }
            continue;
        }
        OverlappedEx* ovEx = (OverlappedEx*)pOv;

        switch (ovEx->ioType) {
        case IOType::IO_CONNECT:
        {
            std::cout << "[Client] ConnectEx complete. dwBytes=" << dwBytes << "\n";
            // SO_UPDATE_CONNECT_CONTEXT 필수
            setsockopt(ovEx->sock, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);

            // 이제 Recv를 걸어둠
            OverlappedEx* ovRecv = new OverlappedEx;
            ZeroMemory(ovRecv, sizeof(*ovRecv));
            ovRecv->ioType = IOType::IO_RECV;
            ovRecv->sock = ovEx->sock;
            ovRecv->wsaBuf.buf = ovRecv->buffer;
            ovRecv->wsaBuf.len = BUF_SIZE;
            DWORD flags = 0;
            int rr = WSARecv(ovEx->sock, &ovRecv->wsaBuf, 1, NULL, &flags, (LPOVERLAPPED)ovRecv, NULL);
            if (rr == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                std::cout << "WSARecv error after connect.\n";
                closesocket(ovEx->sock);
                delete ovRecv;
            }
            // connect overlapped free
            delete ovEx;
        }
        break;
        case IOType::IO_RECV:
        {
            std::cout << "[Client] Received " << dwBytes << " bytes: ";
            std::string msg(ovEx->buffer, dwBytes);
            std::cout << msg << "\n";

            // 다시 recv
            ZeroMemory(&ovEx->overlapped, sizeof(ovEx->overlapped));
            ovEx->ioType = IOType::IO_RECV;
            ovEx->wsaBuf.buf = ovEx->buffer;
            ovEx->wsaBuf.len = BUF_SIZE;
            DWORD flags = 0;
            int rr = WSARecv(ovEx->sock, &ovEx->wsaBuf, 1, NULL, &flags, (LPOVERLAPPED)ovEx, NULL);
            if (rr == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                std::cout << "WSARecv error.\n";
                closesocket(ovEx->sock);
                delete ovEx;
            }
        }
        break;
        case IOType::IO_SEND:
        {
            std::cout << "[Client] Send complete: " << dwBytes << " bytes\n";
            // overlapped 재사용 X -> free? or reuse
            delete ovEx;
        }
        break;
        default:
            closesocket(ovEx->sock);
            delete ovEx;
            break;
        }
    }
}



int main() {
    if (!InitWinSock()) 
        return -1;

    // 기본 소켓 하나 생성
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cout << "socket() failed.\n";
        return -1;
    }

    // ConnectEx 함수 포인터 획득
    if (!LoadConnectEx(sock)) {
        return -1;
    }

    // IOCP 생성 + 소켓 등록
    g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (!g_iocp) {
        std::cout << "CreateIoCompletionPort failed.\n";
        return -1;
    }
    CreateIoCompletionPort((HANDLE)sock, g_iocp, 0, 0);

    // Non-blocking 소켓 준비
    u_long on = 1;
    ioctlsocket(sock, FIONBIO, &on);

    // 연결할 서버 주소
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // OverlappedEx
    OverlappedEx* ovConnect = new OverlappedEx;
    ZeroMemory(ovConnect, sizeof(*ovConnect));
    ovConnect->sock = sock;
    ovConnect->ioType = IOType::IO_CONNECT;

    BOOL ret = g_pConnectEx(sock,
        (SOCKADDR*)&serverAddr, sizeof(serverAddr),
        NULL, 0, NULL,
        (LPOVERLAPPED)ovConnect
    );
    if (!ret && WSAGetLastError() != WSA_IO_PENDING) {
        std::cout << "ConnectEx failed.\n";
        return -1;
    }

    // 워커 스레드 생성
    std::thread worker(WorkerThread);

    std::cout << "Client connecting to " << SERVER_IP << ":" << SERVER_PORT << "...\n";
    std::cout << "Press Enter to send data, type 'quit' to exit.\n";

    while (true) {
        std::string msg;
        getline(std::cin, msg);
        if (msg == "quit") break;

        // OverlappedEx for send
        OverlappedEx* ovSend = new OverlappedEx;
        ZeroMemory(ovSend, sizeof(*ovSend));
        ovSend->sock = sock;
        ovSend->ioType = IOType::IO_SEND;
        ovSend->wsaBuf.buf = ovSend->buffer;
        ovSend->wsaBuf.len = (ULONG)msg.size();
        memcpy(ovSend->buffer, msg.data(), msg.size());

        DWORD sendBytes = 0;
        int sRet = WSASend(sock, &ovSend->wsaBuf, 1, &sendBytes, 0, (LPOVERLAPPED)ovSend, NULL);
        if (sRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
            std::cout << "WSASend error.\n";
            closesocket(sock);
            break;
        }
    }

    g_running = false;
    // dummy post
    PostQueuedCompletionStatus(g_iocp, 0, 0, NULL);

    worker.join();

    closesocket(sock);
    CloseHandle(g_iocp);
    WSACleanup();

    return 0;
}
