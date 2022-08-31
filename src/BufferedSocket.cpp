#include <string.h>
#include <stdio.h>
#include <BufferedSocket/BufferedSocket.h>
#include <BufferedSocket/DataPacket.h>

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

bool isBufferedSocketInitialized = false;
#if defined(USE_WINDOWS_SOCK)
WSADATA wsaData;
#endif

class BufferedSocketImpl : public BufferedSocket
{
public:
    BufferedSocketImpl(bool select_mode);
    virtual ~BufferedSocketImpl();

    virtual void setBufferSize(int size);

    virtual bool connect(const char *address, int port);
    virtual void disconnect();

    virtual bool isConnected() const;

    virtual int readData(char *buf, int bufSize);

    virtual int sendData(const char *data, int length);

    virtual int readNBytes(char *buffer, int N);

    virtual int sendNBytes(const char *buffer, int N);
    virtual int sendNBytes(const unsigned char *buffer, int N);

    virtual int32_t readInt32();
    virtual uint32_t readUInt32();
    virtual int64_t readInt64();
    virtual uint64_t readUInt64();

    virtual bool sendInt32(int32_t val);
    virtual bool sendUInt32(uint32_t val);
    virtual bool sendInt64(int64_t val);
    virtual bool sendUInt64(uint64_t val);

    virtual void requestStopRead();

    //virtual std::vector<char> readUntilStr(const char *str, int length);

    virtual void closeSockAndThrowError(const char *errorMsg);

    virtual void onError(const char *errorMsg);
private:
    void removeAlreadyReadData();

    int interruptable_recv(char *outputBuf, int outputBufSize);
    
    int bufferSize;
    int bufferFilledSize;
    int bufferStartPos;
    char *buffer;

    bool select_mode;
    fd_set read_fd_set;
    volatile bool stopReadRequested;

    #if defined(USE_WINDOWS_SOCK)
        SOCKET sock;
        static WSADATA wsaData;
    #elif defined(USE_BERKELEY_SOCK)
        int sock;
    #endif
};

extern "C"
{
	void BufferedSocketStartup()
	{
		if(!isBufferedSocketInitialized) {
			#if defined(USE_WINDOWS_SOCK)
			WSAStartup(MAKEWORD(2,2),&wsaData);
			#endif
			isBufferedSocketInitialized = true;
		}
	}
	void BufferedSocketCleanup()
	{
		if(isBufferedSocketInitialized) {
			#if defined(USE_WINDOWS_SOCK)
			WSACleanup();
			#endif
			isBufferedSocketInitialized = false;
		}
	}
	
	BufferedSocket *createBufferedSocketRawPtr(bool select_mode)
	{
		return new BufferedSocketImpl(select_mode);
	}
	
	void deleteBufferedSocketRawPtr(BufferedSocket *bufSock)
	{
		delete bufSock;
	}
}

BufferedSocket::~BufferedSocket()
{
}

BufferedSocketImpl::BufferedSocketImpl(bool select_mode)
{
	BufferedSocketStartup();
    this->select_mode = select_mode;
    stopReadRequested = false;
    buffer = NULL;
    setBufferSize(8*1024);
}

BufferedSocketImpl::~BufferedSocketImpl()
{
    delete [] buffer;
}

void BufferedSocketImpl::setBufferSize(int size)
{
    if(buffer != NULL)
    {
        if(bufferSize == size || size < bufferFilledSize)
            return;
        char *buffer2 = new char [size];
        memcpy(buffer2, buffer, bufferFilledSize);
        delete [] buffer;
        buffer = buffer2;
        return ;
    }
    buffer = new char[size];
    bufferSize = size;
    bufferFilledSize = 0;
    bufferStartPos = 0;
}

void BufferedSocketImpl::onError(const char *errorMsg)
{
    printf("%s\n", errorMsg);
}

void BufferedSocketImpl::closeSockAndThrowError(const char *errorMsg)
{
#if defined(USE_WINDOWS_SOCK)
    closesocket(sock);
#else
    close(sock);
#endif
    sock = -1;
    onError(errorMsg);
}


bool BufferedSocketImpl::connect(const char *address, int port)
{
    stopReadRequested = false;
    #if defined(USE_WINDOWS_SOCK)
    sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    sockaddr_in sockAddr;
    sockAddr.sin_family=AF_INET;
    sockAddr.sin_addr.S_un.S_addr=inet_addr(address);
    sockAddr.sin_port=htons(port);
    if(::connect(sock,(SOCKADDR*)&sockAddr,sizeof(SOCKADDR)) < 0)
    {
        closeSockAndThrowError("Failed to connect.");
        return false;
    }
    #elif defined(USE_BERKELEY_SOCK)
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        onError("socket creation failed.");
        return false;
    }

    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr(address);
    sad.sin_port = htons(port);
    
    if (::connect(sock, (struct sockaddr *) &sad, sizeof(sad)) < 0)
    {
        closeSockAndThrowError("Failed to connect.");
        return false;
    }
    #endif
    
    return true;
}

void BufferedSocketImpl::disconnect()
{
#if defined(USE_WINDOWS_SOCK)
    closesocket(sock);
#elif defined(USE_BERKELEY_SOCK)
    close(sock);
#endif
    sock = -1;
}

bool BufferedSocketImpl::isConnected() const
{
    return sock >= 0;
}

void BufferedSocketImpl::requestStopRead()
{
    stopReadRequested = true;
}

int BufferedSocketImpl::interruptable_recv(char *outputBuf, int outputBufSize)
{
    if(select_mode) {
        while(!stopReadRequested) {
            timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            FD_ZERO(&read_fd_set);
            FD_SET(sock, &read_fd_set);
            int ret = select(0, &read_fd_set, NULL, NULL, &timeout);
            if(ret < 0) {
                return ret;
            } else if(ret == 0) {
                continue;
            } else if (FD_ISSET(sock, &read_fd_set)) {
                return recv(sock, outputBuf, outputBufSize, 0);
            }
        }
        return 0;
    } else {
        return recv(sock, outputBuf, outputBufSize, 0);
    }
}

int BufferedSocketImpl::readData(char *outputBuf, int outputBufSize)
{
    if(bufferFilledSize == 0)
    {
        if(outputBufSize > bufferSize)
        {
            int sizeRead = interruptable_recv(outputBuf, outputBufSize);
            if(sizeRead < 0 || (sizeRead == 0 && !stopReadRequested))
                closeSockAndThrowError("recv() failed or connection closed prematurely");
            return sizeRead;
        }
        else 
        {
            int sizeRead = interruptable_recv(buffer, bufferSize);
            if(sizeRead < 0 || (sizeRead == 0 && !stopReadRequested))
            {
                closeSockAndThrowError("recv() failed or connection closed prematurely");
                return sizeRead;
            }
            bufferStartPos = 0;
            bufferFilledSize = sizeRead;
        }
    }

    int sizeRead = std::min(bufferFilledSize, outputBufSize);
    memcpy(outputBuf, buffer+bufferStartPos, sizeRead);
    bufferFilledSize -= sizeRead;
    bufferStartPos += sizeRead;
    if(bufferFilledSize == 0)
        bufferStartPos = 0;
    return sizeRead;
}

int BufferedSocketImpl::sendData(const char *data, int length)
{
    int sizeSent = send(sock, data, length, 0);
    if(sizeSent != length)
        closeSockAndThrowError("send() sent a different number of bytes than expected");
    return sizeSent;
}

void BufferedSocketImpl::removeAlreadyReadData()
{
    if(bufferStartPos > 0)
    {
        memmove(buffer, buffer+bufferStartPos, bufferFilledSize);
        bufferStartPos = 0;
    }
}

/*std::vector<char> BufferedSocket::readUntilStr(const char *str, int length)
{
    std::vector<char> result;
    while(true)
    {
        if(bufferFilledSize == 0)
        {
            bufferStartPos = 0;
            int sizeRead = recv(sock, buffer, bufferSize, 0);
            if(sizeRead <= 0) {
                closeSockAndThrowError("recv() failed or connection closed prematurely");
                return std::vector<char>();
            }
            bufferFilledSize += sizeRead;
        }
        if(buffer[bufferStartPos] != str[0])
        {
            result.push_back(buffer[bufferStartPos]);
            bufferStartPos++;
            bufferFilledSize--;
        } else {
            if(bufferFilledSize < length)
            {
                if(bufferSize - bufferStartPos < length)
                    removeAlreadyReadData();
                while(bufferFilledSize < length)
                {
                    int sizeRead = recv(sock, buffer+bufferStartPos, bufferSize-bufferStartPos, 0);
                    if(sizeRead <= 0) {
                        closeSockAndThrowError("recv() failed or connection closed prematurely");
                        return std::vector<char>();
                    }    
                    bufferFilledSize += sizeRead;
                }
            }

            bool found = true;
            for(int i = 1; i < length; i++)
                if(buffer[bufferStartPos + i] != str[i])
                {
                    found = false;
                    break;
                }
            if(found)
            {
                for(int i = 0; i < length; i++)
                    result.push_back(str[i]);
                bufferStartPos += length;
                bufferFilledSize -= length;
                return result;
            } else {
                result.push_back(buffer[bufferStartPos]);
                bufferStartPos++;
                bufferFilledSize--;
            }
        }        
    }

    return std::vector<char>();
}*/

int BufferedSocketImpl::readNBytes(char *outputBuf, int N)
{
    int total = 0;
    while(total < N)
    {
        int sizeRead = readData(outputBuf + total, N - total);
        if(sizeRead <= 0)
            return sizeRead;
        total += sizeRead;
    }
    return total;
}

int BufferedSocketImpl::sendNBytes(const char *buffer, int N)
{
    int total = 0;
    while(total < N)
    {
        int len = N - total;
        if(len > 10240)
            len = 10240;
        int sizeSent = send(sock, buffer + total, len, 0);
        if(sizeSent <= 0){
            closeSockAndThrowError("send() failed or connection closed prematurely");
            return sizeSent;
        }
        total += len;
    }
    return total;
}

int BufferedSocketImpl::sendNBytes(const unsigned char *buffer, int N)
{
    return sendNBytes(reinterpret_cast<const char*>(buffer), N);
}



int32_t BufferedSocketImpl::readInt32()
{
    uint32_t val = readUInt32();
    return *(int32_t*)&val;
}

uint32_t BufferedSocketImpl::readUInt32()
{
    unsigned char s[4];
    readNBytes((char*)s, 4);
    return convertBytesToUInt32(s);
}

int64_t BufferedSocketImpl::readInt64()
{
    uint64_t val = readUInt64();
    return *(int64_t*)&val;
}

uint64_t BufferedSocketImpl::readUInt64()
{
    unsigned char s[8];
    readNBytes((char*)s, 8);
    return convertBytesToUInt64(s);
}

bool BufferedSocketImpl::sendInt32(int32_t val)
{
    return sendUInt32(*(uint32_t*)&val);
}

bool BufferedSocketImpl::sendUInt32(uint32_t val)
{
    unsigned char s[4];
    convertUInt32ToBytes(val, s);
    return sendNBytes((char*)s, 4) == 4;
}
bool BufferedSocketImpl::sendInt64(int64_t val)
{
    return sendUInt64(*(uint64_t*)&val);
}
bool BufferedSocketImpl::sendUInt64(uint64_t val)
{
    unsigned char s[8];
    convertUInt64ToBytes(val, s);
    return sendNBytes((char*)s, 8) == 8;
}
