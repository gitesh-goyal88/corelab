// Cross-platform HTTP/1.1 server (Winsock on Windows, POSIX on Linux/macOS).
// Serves static HTML/JS/CSS files and registered C++ API routes.
#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#define IS_VALIDSOCK(s) ((s) != INVALID_SOCKET)
#define CLOSESOCK(s) closesocket(s)
#define SEND_FLAGS 0
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
typedef int socket_t;
#define IS_VALIDSOCK(s) ((s) >= 0)
#define CLOSESOCK(s) close(s)
#define SEND_FLAGS MSG_NOSIGNAL
#endif

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

using namespace std;

// Function pointer type for API route handlers
typedef string (*RouteHandler)(const string& query);

class HttpServer {
public:
    HttpServer(int portNumber, const string& webRoot) {
        serverPort = portNumber;
        rootFolder = webRoot;
#ifdef _WIN32
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    }

    ~HttpServer() {
#ifdef _WIN32
        WSACleanup();
#endif
    }

    void api(const string& path, RouteHandler handler) {
        routeTable[path] = handler;
    }

    void run() {
#ifndef _WIN32
        // Ignore SIGPIPE so a client closing early does not terminate the server
        signal(SIGPIPE, SIG_IGN);
#endif
        socket_t serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        int reuseFlag = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseFlag, sizeof reuseFlag);

        int firstPort = serverPort;
        int boundPort = -1;
        for (int port = firstPort; port < firstPort + 10; ++port) {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            addr.sin_port = htons((unsigned short)port);
            if (::bind(serverSocket, (sockaddr*)&addr, sizeof addr) == 0) {
                boundPort = port;
                break;
            }
        }

        if (boundPort < 0 || listen(serverSocket, 64) < 0) {
            cerr << "CoreLab could not start: ports " << firstPort << "-" << firstPort + 9 << " are all busy.\n";
            return;
        }
        if (boundPort != firstPort) {
            cout << "Port " << firstPort << " is busy. Using port " << boundPort << " instead.\n";
        }
        cout << "CoreLab is running.  Open  http://localhost:" << boundPort << "  in your browser.  (Ctrl+C to stop)" << endl;

        for (;;) {
            socket_t clientSocket = accept(serverSocket, nullptr, nullptr);
            if (!IS_VALIDSOCK(clientSocket)) {
                continue;
            }
            try {
                thread worker(clientWorker, this, clientSocket);
                worker.detach();
            } catch (...) {
                CLOSESOCK(clientSocket);
            }
        }
    }

private:
    int serverPort;
    string rootFolder;
    map<string, RouteHandler> routeTable;
    mutex routeMutex;

    static void clientWorker(HttpServer* serverObj, socket_t clientSocket) {
        serverObj->serveClient(clientSocket);
    }

    static string getMimeType(const string& filePath) {
        size_t dotPos = filePath.find_last_of('.');
        string ext = (dotPos == string::npos) ? "" : filePath.substr(dotPos + 1);
        if (ext == "html") return "text/html; charset=utf-8";
        if (ext == "css")  return "text/css; charset=utf-8";
        if (ext == "js")   return "application/javascript; charset=utf-8";
        if (ext == "svg")  return "image/svg+xml";
        return "application/octet-stream";
    }

    static void sendResponse(socket_t clientSocket, int statusCode, const string& contentType, const string& body, bool noCache) {
        string statusText = "Internal Server Error";
        if (statusCode == 200) statusText = "OK";
        else if (statusCode == 404) statusText = "Not Found";

        string response = "HTTP/1.1 " + to_string(statusCode) + " " + statusText +
                          "\r\nContent-Type: " + contentType +
                          "\r\nContent-Length: " + to_string(body.size()) +
                          (noCache ? "\r\nCache-Control: no-store" : "") +
                          "\r\nConnection: close\r\n\r\n" + body;

        size_t sentBytes = 0;
        while (sentBytes < response.size()) {
            int written = send(clientSocket, response.data() + sentBytes, (int)(response.size() - sentBytes), SEND_FLAGS);
            if (written <= 0) {
                break;
            }
            sentBytes += written;
        }
    }

    void serveClient(socket_t clientSocket) {
        try {
#ifdef _WIN32
            DWORD timeoutMs = 3000;
            setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof timeoutMs);
#else
            timeval timeoutVal{3, 0};
            setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeoutVal, sizeof timeoutVal);
#endif
            string requestText;
            char buffer[2048];
            while (requestText.find("\r\n\r\n") == string::npos && requestText.size() < 16384) {
#ifdef _WIN32
                int bytesRead = recv(clientSocket, buffer, sizeof buffer, 0);
#else
                ssize_t bytesRead = read(clientSocket, buffer, sizeof buffer);
#endif
                if (bytesRead <= 0) {
                    break;
                }
                requestText.append(buffer, bytesRead);
            }

            istringstream reqStream(requestText);
            string method, path;
            reqStream >> method >> path;
            if (path.empty() || path[0] != '/') {
                CLOSESOCK(clientSocket);
                return;
            }

            string query;
            size_t qPos = path.find('?');
            if (qPos != string::npos) {
                query = path.substr(qPos + 1);
                path = path.substr(0, qPos);
            }

            map<string, RouteHandler>::iterator it = routeTable.find(path);
            if (it != routeTable.end()) {
                string body;
                try {
                    lock_guard<mutex> guard(routeMutex);
                    body = it->second(query);
                } catch (const exception& ex) {
                    sendResponse(clientSocket, 500, "application/json", "{\"error\":\"internal error\"}", true);
                    cerr << "request error: " << ex.what() << "\n";
                    CLOSESOCK(clientSocket);
                    return;
                }
                sendResponse(clientSocket, 200, "application/json; charset=utf-8", body, true);
            } else {
                if (path == "/") {
                    path = "/index.html";
                }
                if (path.find("..") != string::npos || path.find('%') != string::npos) {
                    sendResponse(clientSocket, 404, "text/plain", "Not found", false);
                    CLOSESOCK(clientSocket);
                    return;
                }
                ifstream fileStream(rootFolder + path, ios::binary);
                if (!fileStream) {
                    sendResponse(clientSocket, 404, "text/plain", "Not found", false);
                } else {
                    stringstream fileBuf;
                    fileBuf << fileStream.rdbuf();
                    sendResponse(clientSocket, 200, getMimeType(path), fileBuf.str(), true);
                }
            }
        } catch (...) {
        }
        CLOSESOCK(clientSocket);
    }
};
