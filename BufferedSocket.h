#pragma once

#if defined(_WIN32) || defined(WIN32)
    #define USE_WINDOWS
    #include <WinSock2.h>
#elif defined(linux) || defined(__linux__)
    #define USE_LINUX
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif

#include <string>
#include <vector>

class DataPacket
{
public:
    std::vector<unsigned char> data;
    int offset;

    DataPacket();
    int size();
    const unsigned char *getRawPtr();
    void rewind();

    static int32_t convertToInt32(const unsigned char* ptr);
    static uint32_t convertToUInt32(const unsigned char* ptr);
    static int64_t convertToInt64(const unsigned char* ptr);
    static uint64_t convertToUInt64(const unsigned char* ptr);

    static void convertInt32ToBytes(int32_t val, unsigned char* dst);
    static void convertUInt32ToBytes(u_int32_t val, unsigned char* dst);
    static void convertInt64ToBytes(int64_t val, unsigned char* dst);
    static void convertUInt64ToBytes(u_int64_t val, unsigned char* dst);

    bool readInt32(int32_t *out);
    bool readUInt32(u_int32_t *out);
    bool readInt64(int64_t *out);
    bool readUInt64(u_int64_t *out);

    void putInt32(int32_t val);
    void putUInt32(u_int32_t val);
    void putInt64(int64_t val);
    void putUInt64(u_int64_t val);

    void putNBytes(const unsigned char* buf, int N);
    void putNBytes(const char* buf, int N);
};

class BufferedSocket
{
public:
    BufferedSocket();
    ~BufferedSocket();
    BufferedSocket(const BufferedSocket&) = delete;
    void setBufferSize(int size);

    static void startup();
    static void cleanup();

    bool connect(std::string address, int port);
    void disconnect();

    bool isConnected() const;

    int readData(char *buf, int bufSize);

    int sendData(const char *data, int length);

    int readNBytes(char *buffer, int N);

    int sendNBytes(const char *buffer, int N);
    int sendNBytes(const unsigned char *buffer, int N);

    int32_t readInt32();
    u_int32_t readUInt32();
    int64_t readInt64();
    u_int64_t readUInt64();

    bool sendInt32(int32_t val);
    bool sendUInt32(u_int32_t val);
    bool sendInt64(int64_t val);
    bool sendUInt64(u_int64_t val);

    std::vector<char> readUntilStr(const char *str, int length);

    void closeSockAndThrowError(std::string errorMsg);

    void onError(std::string errorMsg);
private:
    void removeAlreadyReadData();
    
    int bufferSize;
    int bufferFilledSize;
    int bufferStartPos;
    char *buffer;

    #if defined(USE_WINDOWS)
        SOCKET sock;
        static WSADATA wsaData;
    #elif defined(USE_LINUX)
        int sock;
    #endif
};