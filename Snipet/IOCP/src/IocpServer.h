#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>  // AcceptEx, ConnectEx 등
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif
#include <Windows.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>

// 설정
static const int SERVER_PORT = 9000;
static const int BUF_SIZE = 1024;
static const int WORKER_COUNT = 2;  // 간단히 2개 스레드로

// 확장 함수 포인터
LPFN_ACCEPTEX      g_pAcceptEx = NULL;
LPFN_GETACCEPTEXSOCKADDRS g_pGetAcceptExSockaddrs = NULL;

// IoType 구분
enum class IOType { IO_NONE, IO_ACCEPT, IO_RECV, IO_SEND };

// Overlapped 확장 구조
struct OverlappedEx {
    OVERLAPPED overlapped;
    SOCKET     sock;
    WSABUF     wsaBuf;
    char       buffer[BUF_SIZE];
    IOType     ioType;
};

// 전역
SOCKET g_listenSock = INVALID_SOCKET;
HANDLE g_iocp = NULL;
std::atomic_bool g_running{ true };

// 함수 선언
bool InitWinSock();
bool BindAndListen();
bool LoadAcceptExFunction();
bool PostAccept();
void WorkerThread();
int IocpTest();

int IocpTest() {
    if (!InitWinSock()) {
        return -1;
    }
    if (!BindAndListen()) {
        return -1;
    }
    if (!LoadAcceptExFunction()) {
        return -1;
    }

    // IOCP 생성
    // CreateIoCompletionPort: IO 포트를 생성하거나 소켓을 기존 IO 포트에 연결
    // 이 부분은 IOCP 생성
    g_iocp = CreateIoCompletionPort(
        INVALID_HANDLE_VALUE,   // 기존 포트에 연결시 파일핸들, 포트 생성시 INVALID_HANDLE_VALUE
        NULL,                   // 기존 포트 핸들 or NULL
        0,                      // 완료 키 (해당 핸들로부터 완료 통지가 올 때 식별용으로 받는 사용자 정의 값)
        0                       // 동시 실행 스레드 수
    );
    if (!g_iocp) {
        std::cout << "CreateIoCompletionPort failed. err=" << GetLastError() << "\n";
        return -1;
    }
    // 리슨 소켓을 IOCP와 연결(CompletionKey=0)
    CreateIoCompletionPort((HANDLE)g_listenSock, g_iocp, 0, 0);

    // 워커 스레드 실행
    std::vector<std::thread> workers;
    for (int i = 0; i < WORKER_COUNT; i++) {
        workers.emplace_back(WorkerThread);
    }

    // AcceptEx를 1개만 미리 걸어둠 (실무에선 여러 번)
    PostAccept();

    std::cout << "Echo Server started on port " << SERVER_PORT << ". Press Enter to quit.\n";
    getchar();

    g_running = false;
    // 깨어나도록 dummy post
    for (int i = 0; i < WORKER_COUNT; i++) {
        // IOCP에 인위적으로 완료 패킷을 넣어 워커 스레드를 깨웁니다. 실제 I/O가 아닌 "신호 전달" 용도로도 많이 사용.
        // IOCP에 "완료된 작업"처럼 이벤트를 넣음.
        // GetQueuedCompletionStatus()가 이 정보를 꺼내 감지.
        PostQueuedCompletionStatus(
            g_iocp,     // 대상 IOCP
            0,          // 전송된 바이트 수
            0,          // 완료 키
            nullptr     // OVERLAPPED 구조체 포인터
        );
    }

    for (auto& th : workers) {
        th.join();
    }

    closesocket(g_listenSock);
    CloseHandle(g_iocp);
    WSACleanup();
    return 0;
}

#define IN
#define OUT


bool InitWinSock() 
{
    WSADATA wsa;
    // 윈속 초기화 함수 호출
    if (WSAStartup(MAKEWORD(2, 2), OUT &wsa) != 0) {
        std::cout << "WSAStartup failed.\n";
        return false;
    }
    return true;
}

bool BindAndListen() 
{
    // listen 소켓 생성 (클라이언트 접속을 대기하는 서버 소켓)
    // listen 소켓은 클라이언트가 들어오는 걸 감지만 하지, 실제 데이터 통신은 안 함.
    // 실제 데이터 통신은 별도 accept socket이 맡음.
    g_listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listenSock == INVALID_SOCKET) {
        std::cout << "socket() failed.\n";
        return false;
    }

    SOCKADDR_IN servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SERVER_PORT);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // listen 소켓 바인드(소켓을 특정 IP 주소 + 포트에 연결)
    // 내 컴퓨터의 "모든 IP(0.0.0.0)" + "포트 SERVER_PORT"으로 들어오는 연결은 listen 소켓을 통해 받겠다는 의미
    if (bind(g_listenSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        std::cout << "bind failed.\n";
        return false;
    }

    // 소켓이 이제부터 클라이언트의 연결 요청을 대기하겠다는 선언
    // 커널 레벨에서 연결 요청을 받아줄 큐가 생김
    if (listen(g_listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "listen failed.\n";
        return false;
    }
    return true;
}

bool LoadAcceptExFunction() {
    // AcceptEx는 확장 함수 -> WSAIoctl로 함수 포인터 구해야 함
    GUID guidAcceptEx = WSAID_ACCEPTEX;
    GUID guidGetAcceptSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    DWORD bytes = 0;

    // WSAIoctl: 소켓의 모드를 제어하는 함수
    int ret = WSAIoctl(
        g_listenSock,                       // 소켓 식별자
        SIO_GET_EXTENSION_FUNCTION_POINTER, // 함수가 수행하는 연산 제어 코드
        &guidAcceptEx,                      // 입력 버퍼에 대한 포인터
        sizeof(guidAcceptEx),               // 입력 버퍼의 크기를 알리는 수치
        OUT &g_pAcceptEx,                   // 출력 버퍼에 대한 포인터 (OUT)
        sizeof(g_pAcceptEx),                // 출력 버퍼의 크기를 알리는 수치
        &bytes,                             // 출력시 실제로 받아내는 바이트 수
        NULL,                               // WSAOVERLAPPED 구조체의 포인터 (비동기 소켓에서는 무시)
        NULL                                // 연산이 완료 되었을 때 호출될 완료루틴(completion routine)의 포인터(넌 - 오버랩 소켓에서는 무시)
    );
    if (ret == SOCKET_ERROR) {
        std::cout << "WSAIoctl for AcceptEx failed.\n";
        return false;
    }

    bytes = 0;
    // WSAIoctl: 소켓의 모드를 제어하는 함수
    ret = WSAIoctl(g_listenSock,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &guidGetAcceptSockAddrs, sizeof(guidGetAcceptSockAddrs),
        &g_pGetAcceptExSockaddrs, sizeof(g_pGetAcceptExSockaddrs),
        &bytes, NULL, NULL);
    if (ret == SOCKET_ERROR) {
        std::cout << "WSAIoctl for GetAcceptExSockaddrs failed.\n";
        return false;
    }

    return true;
}

// Overlapped AcceptEx post
bool PostAccept() {

    // Accept 소켓 준비
    SOCKET acceptSock = socket(AF_INET, SOCK_STREAM, 0);
    if (acceptSock == INVALID_SOCKET) {
        std::cout << "acceptSock create failed.\n";
        return false;
    }

    // OverlappedEx 구조 할당
    OverlappedEx* ov = new OverlappedEx;
    ZeroMemory(ov, sizeof(*ov));
    ov->sock = acceptSock;
    ov->ioType = IOType::IO_ACCEPT;

    // AcceptEx buffer는 (>=2 * (sizeof(sockaddr_in) + 16)) 필요
    // 여기서는 그냥 OverlappedEx::buffer를 사용
    DWORD bytesReceived = 0;
    BOOL ret = g_pAcceptEx(g_listenSock, acceptSock,
        ov->buffer, 0,
        sizeof(SOCKADDR_IN) + 16,
        sizeof(SOCKADDR_IN) + 16,
        &bytesReceived,
        (LPOVERLAPPED)ov);
    if (!ret && WSAGetLastError() != WSA_IO_PENDING) {
        std::cout << "AcceptEx failed.\n";
        closesocket(acceptSock);
        delete ov;
        return false;
    }
    return true;
}

// 워커 스레드: GetQueuedCompletionStatus로 I/O 완료처리
void WorkerThread() {
    while (g_running) {
        DWORD dwBytes = 0;
        ULONG_PTR key = 0;
        LPOVERLAPPED pOv = nullptr;

        // I/O 완료 통지를 대기 및 수신합니다. 완료된 I/O가 발생하면 정보를 받아옴.
        // 완료된 비동기 I/O 작업이 발생할 때까지 대기.
        // 작업이 완료되면 : 전송된 바이트 수, 연결된 키, OVERLAPPED 구조체를 반환
        BOOL ret = GetQueuedCompletionStatus(
            g_iocp,     // IOCP 핸들
            &dwBytes,   // 완료된 바이트 수
            &key,       // 연결 시 등록한 CompletionKey 반환
            &pOv,       // 완료된 OVERLAPPED 구조체 포인터
            INFINITE    // 대기 시간 (INFINITE 권장)
        );
        if (!ret || dwBytes == 0) {
            if (!g_running) break;
            if (pOv) {
                // 오류 or 연결 끊김
                OverlappedEx* ovEx = (OverlappedEx*)pOv;
                closesocket(ovEx->sock);
                delete ovEx;
            }
            continue;
        }

        OverlappedEx* ovEx = (OverlappedEx*)pOv;
        switch (ovEx->ioType) {
        case IOType::IO_ACCEPT:
        {
            // AcceptEx 완료
            // 소켓 세팅
            SOCKET clientSock = ovEx->sock;
            std::cout << "[Worker] Accept complete.\n";

            // 소켓 옵션 설정(예: setsockopt, SO_UPDATE_ACCEPT_CONTEXT)
            setsockopt(clientSock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&g_listenSock, sizeof(g_listenSock));

            // 소켓을 IOCP에 등록(CompletionKey = 소켓 or 0)
            CreateIoCompletionPort((HANDLE)clientSock, g_iocp, 0, 0);

            // 클라이언트 주소 확인
            SOCKADDR_IN* localAddr = nullptr;
            SOCKADDR_IN* remoteAddr = nullptr;
            int localLen = 0, remoteLen = 0;
            g_pGetAcceptExSockaddrs(ovEx->buffer, 0,
                sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
                (LPSOCKADDR*)&localAddr, &localLen,
                (LPSOCKADDR*)&remoteAddr, &remoteLen);
            std::cout << "[Worker] Client IP=" << inet_ntoa(remoteAddr->sin_addr)
                << " Port=" << ntohs(remoteAddr->sin_port) << "\n";

            // 다음 AcceptEx 미리 걸기
            PostAccept();

            // 이제 클라이언트에게서 데이터 수신을 비동기로 요청
            OverlappedEx* recvOv = new OverlappedEx;
            ZeroMemory(recvOv, sizeof(*recvOv));
            recvOv->ioType = IOType::IO_RECV;
            recvOv->sock = clientSock;
            recvOv->wsaBuf.buf = recvOv->buffer;
            recvOv->wsaBuf.len = BUF_SIZE;

            DWORD flags = 0;
            if (WSARecv(clientSock, &recvOv->wsaBuf, 1, NULL, &flags, (LPWSAOVERLAPPED)recvOv, NULL) == SOCKET_ERROR) {
                int err = WSAGetLastError();
                if (err != WSA_IO_PENDING) {
                    std::cout << "WSARecv error in Accept step.\n";
                    closesocket(clientSock);
                    delete recvOv;
                }
            }
            // AcceptOv는 더 이상 불필요
            delete ovEx;
        }
        break;
        case IOType::IO_RECV:
        {
            // 수신 완료 -> Echo 응답
            std::cout << "[Worker] Recv " << dwBytes << " bytes\n";
            // Echo -> WSASend
            ovEx->ioType = IOType::IO_SEND;
            ovEx->wsaBuf.len = dwBytes;

            DWORD sendBytes = 0;
            if (WSASend(ovEx->sock, &ovEx->wsaBuf, 1, &sendBytes, 0, (LPWSAOVERLAPPED)ovEx, NULL) == SOCKET_ERROR) {
                int err = WSAGetLastError();
                if (err != WSA_IO_PENDING) {
                    std::cout << "WSASend error.\n";
                    closesocket(ovEx->sock);
                    delete ovEx;
                }
            }
        }
        break;
        case IOType::IO_SEND:
        {
            // 송신 완료 -> 다시 recv 대기
            std::cout << "[Worker] Send done " << dwBytes << " bytes\n";
            ovEx->ioType = IOType::IO_RECV;
            ovEx->wsaBuf.len = BUF_SIZE;
            ovEx->wsaBuf.buf = ovEx->buffer;
            DWORD flags = 0;
            if (WSARecv(ovEx->sock, &ovEx->wsaBuf, 1, NULL, &flags, (LPWSAOVERLAPPED)ovEx, NULL) == SOCKET_ERROR) {
                int err = WSAGetLastError();
                if (err != WSA_IO_PENDING) {
                    closesocket(ovEx->sock);
                    delete ovEx;
                }
            }
        }
        break;
        default:
            std::cout << "[Worker] Unknown IOType\n";
            closesocket(ovEx->sock);
            delete ovEx;
            break;
        }
    }
    // 스레드 종료
}
