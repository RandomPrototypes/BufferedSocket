#pragma once

#include <vector>
#include <stdint.h>
#include <memory>
#include <BufferedSocket/BufferedSocketDefs.h>

class BUFSOCK_EXPORTS DataPacket
{
public:
    virtual ~DataPacket();

    virtual int size() = 0;
    virtual const unsigned char *getRawPtr() = 0;
    virtual void rewind() = 0;

    virtual bool readInt16(int16_t *out, bool big_endian = true) = 0;
    virtual bool readUInt16(uint16_t *out, bool big_endian = true) = 0;
    virtual bool readInt32(int32_t *out, bool big_endian = true) = 0;
    virtual bool readUInt32(uint32_t *out, bool big_endian = true) = 0;
    virtual bool readInt64(int64_t *out, bool big_endian = true) = 0;
    virtual bool readUInt64(uint64_t *out, bool big_endian = true) = 0;

    virtual void putInt16(int16_t val, bool big_endian = true) = 0;
    virtual void putUInt16(uint16_t val, bool big_endian = true) = 0;
    virtual void putInt32(int32_t val, bool big_endian = true) = 0;
    virtual void putUInt32(uint32_t val, bool big_endian = true) = 0;
    virtual void putInt64(int64_t val, bool big_endian = true) = 0;
    virtual void putUInt64(uint64_t val, bool big_endian = true) = 0;

    virtual void putNBytes(const unsigned char* buf, int N) = 0;
    virtual void putNBytes(const char* buf, int N) = 0;
};

extern "C"
{
	BUFSOCK_EXPORTS DataPacket *createDataPacketRawPtr();
	BUFSOCK_EXPORTS void deleteDataPacketRawPtr(DataPacket *packet);
	
	BUFSOCK_EXPORTS int16_t convertBytesToInt16(const unsigned char* ptr, bool big_endian = true);
	BUFSOCK_EXPORTS uint16_t convertBytesToUInt16(const unsigned char* ptr, bool big_endian = true);
	BUFSOCK_EXPORTS int32_t convertBytesToInt32(const unsigned char* ptr, bool big_endian = true);
	BUFSOCK_EXPORTS uint32_t convertBytesToUInt32(const unsigned char* ptr, bool big_endian = true);
	BUFSOCK_EXPORTS int64_t convertBytesToInt64(const unsigned char* ptr, bool big_endian = true);
	BUFSOCK_EXPORTS uint64_t convertBytesToUInt64(const unsigned char* ptr, bool big_endian = true);

	BUFSOCK_EXPORTS void convertInt16ToBytes(int16_t val, unsigned char* dst, bool big_endian = true);
	BUFSOCK_EXPORTS void convertUInt16ToBytes(uint16_t val, unsigned char* dst, bool big_endian = true);
	BUFSOCK_EXPORTS void convertInt32ToBytes(int32_t val, unsigned char* dst, bool big_endian = true);
	BUFSOCK_EXPORTS void convertUInt32ToBytes(uint32_t val, unsigned char* dst, bool big_endian = true);
	BUFSOCK_EXPORTS void convertInt64ToBytes(int64_t val, unsigned char* dst, bool big_endian = true);
	BUFSOCK_EXPORTS void convertUInt64ToBytes(uint64_t val, unsigned char* dst, bool big_endian = true);
}

inline std::shared_ptr<DataPacket> createDataPacket()
{
	return std::shared_ptr<DataPacket>(createDataPacketRawPtr(), deleteDataPacketRawPtr);
}
