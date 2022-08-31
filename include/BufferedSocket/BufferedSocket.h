#pragma once

#include <vector>
#include <stdint.h>
#include <memory>
#include <BufferedSocket/BufferedSocketDefs.h>

class BUFSOCK_EXPORTS BufferedSocket
{
public:
    virtual ~BufferedSocket();

    virtual void setBufferSize(int size) = 0;

    virtual bool connect(const char *address, int port) = 0;
    virtual void disconnect() = 0;

    virtual bool isConnected() const = 0;

    virtual int readData(char *buf, int bufSize) = 0;

    virtual int sendData(const char *data, int length) = 0;

    virtual int readNBytes(char *buffer, int N) = 0;

    virtual int sendNBytes(const char *buffer, int N) = 0;
    virtual int sendNBytes(const unsigned char *buffer, int N) = 0;

    virtual int32_t readInt32() = 0;
    virtual uint32_t readUInt32() = 0;
    virtual int64_t readInt64() = 0;
    virtual uint64_t readUInt64() = 0;

    virtual bool sendInt32(int32_t val) = 0;
    virtual bool sendUInt32(uint32_t val) = 0;
    virtual bool sendInt64(int64_t val) = 0;
    virtual bool sendUInt64(uint64_t val) = 0;

    //virtual std::vector<char> readUntilStr(const char *str, int length);
    
    virtual void requestStopRead() = 0;//stop a blocked recv, only works if select_mode is selected

    virtual void closeSockAndThrowError(const char *errorMsg) = 0;

    virtual void onError(const char *errorMsg) = 0;
};

extern "C"
{
	BUFSOCK_EXPORTS void BufferedSocketStartup();
	BUFSOCK_EXPORTS void BufferedSocketCleanup();
	
	BUFSOCK_EXPORTS BufferedSocket *createBufferedSocketRawPtr(bool select_mode = true);
	BUFSOCK_EXPORTS void deleteBufferedSocketRawPtr(BufferedSocket *bufSock);
}

inline std::shared_ptr<BufferedSocket> createBufferedSocket(bool select_mode = true)
{
	return std::shared_ptr<BufferedSocket>(createBufferedSocketRawPtr(select_mode), deleteBufferedSocketRawPtr);
}
