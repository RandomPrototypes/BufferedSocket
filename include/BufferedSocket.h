#pragma once

#ifndef BUFSOCK_EXPORTS
# if (defined _WIN32 || defined WINCE || defined __CYGWIN__) && defined(BUFFEREDSOCKET_EXPORTS)
#   define BUFSOCK_EXPORTS __declspec(dllexport)
# elif defined __GNUC__ && __GNUC__ >= 4 && (defined(BUFFEREDSOCKET_EXPORTS) || defined(__APPLE__))
#   define BUFSOCK_EXPORTS __attribute__ ((visibility ("default")))
# else
# define BUFSOCK_EXPORTS
# endif
#endif


#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
    #define USE_WINDOWS_SOCK
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#elif defined(linux) || defined(__linux__) || defined(__APPLE__) || defined(__unix)
    #define USE_BERKELEY_SOCK
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
#endif

#include <string>
#include <vector>
#include <stdint.h>

class BUFSOCK_EXPORTS DataPacket
{
public:
    DataPacket();
    ~DataPacket();
    int size();
    const unsigned char *getRawPtr();
    void rewind();

    static int32_t convertToInt32(const unsigned char* ptr);
    static uint32_t convertToUInt32(const unsigned char* ptr);
    static int64_t convertToInt64(const unsigned char* ptr);
    static uint64_t convertToUInt64(const unsigned char* ptr);

    static void convertInt32ToBytes(int32_t val, unsigned char* dst);
    static void convertUInt32ToBytes(uint32_t val, unsigned char* dst);
    static void convertInt64ToBytes(int64_t val, unsigned char* dst);
    static void convertUInt64ToBytes(uint64_t val, unsigned char* dst);

    bool readInt32(int32_t *out);
    bool readUInt32(uint32_t *out);
    bool readInt64(int64_t *out);
    bool readUInt64(uint64_t *out);

    void putInt32(int32_t val);
    void putUInt32(uint32_t val);
    void putInt64(int64_t val);
    void putUInt64(uint64_t val);

    void putNBytes(const unsigned char* buf, int N);
    void putNBytes(const char* buf, int N);
private:
    std::vector<unsigned char> data;
    int offset;
};

class BUFSOCK_EXPORTS BufferedSocket
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
    uint32_t readUInt32();
    int64_t readInt64();
    uint64_t readUInt64();

    bool sendInt32(int32_t val);
    bool sendUInt32(uint32_t val);
    bool sendInt64(int64_t val);
    bool sendUInt64(uint64_t val);

    //std::vector<char> readUntilStr(const char *str, int length);

    void closeSockAndThrowError(std::string errorMsg);

    void onError(std::string errorMsg);
private:
    void removeAlreadyReadData();
    
    int bufferSize;
    int bufferFilledSize;
    int bufferStartPos;
    char *buffer;

    #if defined(USE_WINDOWS_SOCK)
        SOCKET sock;
        static WSADATA wsaData;
    #elif defined(USE_BERKELEY_SOCK)
        int sock;
    #endif
};
