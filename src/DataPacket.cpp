#include <string.h>
#include <stdio.h>
#include <BufferedSocket/DataPacket.h>

class DataPacketImpl : public DataPacket
{
public:
    DataPacketImpl();
    virtual ~DataPacketImpl();

    virtual int size();
    virtual const unsigned char *getRawPtr();
    virtual void rewind();

    virtual bool readInt16(int16_t *out, bool big_endian = true);
    virtual bool readUInt16(uint16_t *out, bool big_endian = true);
    virtual bool readInt32(int32_t *out, bool big_endian = true);
    virtual bool readUInt32(uint32_t *out, bool big_endian = true);
    virtual bool readInt64(int64_t *out, bool big_endian = true);
    virtual bool readUInt64(uint64_t *out, bool big_endian = true);

    virtual void putInt16(int16_t val, bool big_endian = true);
    virtual void putUInt16(uint16_t val, bool big_endian = true);
    virtual void putInt32(int32_t val, bool big_endian = true);
    virtual void putUInt32(uint32_t val, bool big_endian = true);
    virtual void putInt64(int64_t val, bool big_endian = true);
    virtual void putUInt64(uint64_t val, bool big_endian = true);

    virtual void putNBytes(const unsigned char* buf, int N);
    virtual void putNBytes(const char* buf, int N);
private:
	int offset;
	std::vector<unsigned char> data;
};

DataPacket::~DataPacket()
{
}

DataPacketImpl::DataPacketImpl()
{
    offset = 0;
}

DataPacketImpl::~DataPacketImpl()
{
}

void DataPacketImpl::rewind()
{
    offset = 0;
}

bool DataPacketImpl::readInt16(int16_t *out, bool big_endian)
{
    return readUInt16((uint16_t*)out, big_endian);
}
bool DataPacketImpl::readUInt16(uint16_t *out, bool big_endian)
{
    if(offset + 2 > data.size())
        return false;

    *out = convertBytesToUInt16((unsigned char*)&data[offset], big_endian);
    offset += 2;
    return true;
}

bool DataPacketImpl::readInt32(int32_t *out, bool big_endian)
{
    return readUInt32((uint32_t*)out, big_endian);
}
bool DataPacketImpl::readUInt32(uint32_t *out, bool big_endian)
{
    if(offset + 4 > data.size())
        return false;

    *out = convertBytesToUInt32((unsigned char*)&data[offset], big_endian);
    offset += 4;
    return true;
}

bool DataPacketImpl::readInt64(int64_t *out, bool big_endian)
{
    return readUInt64((uint64_t*)out, big_endian);
}

bool DataPacketImpl::readUInt64(uint64_t *out, bool big_endian)
{
    if(offset + 8 > data.size())
        return false;

    *out = convertBytesToUInt64((unsigned char*)&data[offset], big_endian);
    offset += 8;
    return true;
}

void DataPacketImpl::putInt16(int16_t val, bool big_endian)
{
    putUInt16(*(uint16_t*)&val, big_endian);
}
void DataPacketImpl::putUInt16(uint16_t val, bool big_endian)
{
    unsigned char buf[2];
    convertUInt16ToBytes(val, buf, big_endian);
    putNBytes(buf, 2);
}
void DataPacketImpl::putInt32(int32_t val, bool big_endian)
{
    putUInt32(*(uint32_t*)&val, big_endian);
}
void DataPacketImpl::putUInt32(uint32_t val, bool big_endian)
{
    unsigned char buf[4];
    convertUInt32ToBytes(val, buf, big_endian);
    putNBytes(buf, 4);
}
void DataPacketImpl::putInt64(int64_t val, bool big_endian)
{
    putUInt64(*(uint64_t*)&val, big_endian);
}
void DataPacketImpl::putUInt64(uint64_t val, bool big_endian)
{
    unsigned char buf[8];
    convertUInt64ToBytes(val, buf, big_endian);
    putNBytes(buf, 8);
}

void DataPacketImpl::putNBytes(const unsigned char* buf, int N)
{
    int start = data.size();
    data.resize(start+N);
    for(int i = 0; i < N; i++)
        data[start+i] = buf[i];
}

void DataPacketImpl::putNBytes(const char* buf, int N)
{
    putNBytes(reinterpret_cast<const unsigned char *>(buf), N);
}

int DataPacketImpl::size()
{
    return data.size();
}

const unsigned char *DataPacketImpl::getRawPtr()
{
    return &data[0];
}

extern "C"
{

DataPacket *createDataPacketRawPtr()
{
	return new DataPacketImpl();
}

void deleteDataPacketRawPtr(DataPacket *packet)
{
	delete packet;
}

int16_t convertBytesToInt16(const unsigned char* ptr, bool big_endian)
{
    uint16_t val = convertBytesToUInt16(ptr, big_endian);
    return *(int16_t*)&val;
}

uint16_t convertBytesToUInt16(const unsigned char* ptr, bool big_endian)
{
    if(big_endian) {
        return ((uint16_t) ptr[1] << 0)
            | ((uint16_t) ptr[0] << 8);
    } else {
        return ((uint16_t) ptr[0] << 0)
            | ((uint16_t) ptr[1] << 8);
    }
}

int32_t convertBytesToInt32(const unsigned char* ptr, bool big_endian)
{
    uint32_t val = convertBytesToUInt32(ptr, big_endian);
    return *(int32_t*)&val;
}

uint32_t convertBytesToUInt32(const unsigned char* ptr, bool big_endian)
{
    if(big_endian) {
        return ((uint32_t) ptr[3] << 0)
            | ((uint32_t) ptr[2] << 8)
            | ((uint32_t) ptr[1] << 16)
            | ((uint32_t) ptr[0] << 24);
    } else {
        return ((uint32_t) ptr[0] << 0)
            | ((uint32_t) ptr[1] << 8)
            | ((uint32_t) ptr[2] << 16)
            | ((uint32_t) ptr[3] << 24);
    }
}

int64_t convertBytesToInt64(const unsigned char* ptr, bool big_endian)
{
    uint64_t val = convertBytesToUInt64(ptr, big_endian);
    return *(int64_t*)&val;
}

uint64_t convertBytesToUInt64(const unsigned char* ptr, bool big_endian)
{
    if(big_endian) {
        return ((uint64_t) ptr[7] << 0)
            | ((uint64_t) ptr[6] << 8)
            | ((uint64_t) ptr[5] << 16)
            | ((uint64_t) ptr[4] << 24)
            | ((uint64_t) ptr[3] << 32)
            | ((uint64_t) ptr[2] << 40)
            | ((uint64_t) ptr[1] << 48)
            | ((uint64_t) ptr[0] << 56);
    } else {
        return ((uint64_t) ptr[0] << 0)
            | ((uint64_t) ptr[1] << 8)
            | ((uint64_t) ptr[2] << 16)
            | ((uint64_t) ptr[3] << 24)
            | ((uint64_t) ptr[4] << 32)
            | ((uint64_t) ptr[5] << 40)
            | ((uint64_t) ptr[6] << 48)
            | ((uint64_t) ptr[7] << 56);
    }
}

void convertInt16ToBytes(int16_t val, unsigned char* dst, bool big_endian)
{
    convertUInt16ToBytes(*(uint16_t*)&val, dst, big_endian);
}

void convertUInt16ToBytes(uint16_t val, unsigned char* dst, bool big_endian)
{
    if(big_endian) {
        dst[0] = (val >> 8) & 0xFF;
        dst[1] = val & 0xFF;
    } else {
        dst[1] = (val >> 8) & 0xFF;
        dst[0] = val & 0xFF;
    }
}

void convertInt32ToBytes(int32_t val, unsigned char* dst, bool big_endian)
{
    convertUInt32ToBytes(*(uint32_t*)&val, dst, big_endian);
}

void convertUInt32ToBytes(uint32_t val, unsigned char* dst, bool big_endian)
{
    if(big_endian) {
        dst[0] = (val >> 24) & 0xFF;
        dst[1] = (val >> 16) & 0xFF;
        dst[2] = (val >> 8) & 0xFF;
        dst[3] = val & 0xFF;
    } else {
        dst[3] = (val >> 24) & 0xFF;
        dst[2] = (val >> 16) & 0xFF;
        dst[1] = (val >> 8) & 0xFF;
        dst[0] = val & 0xFF;
    }
}

void convertInt64ToBytes(int64_t val, unsigned char* dst, bool big_endian)
{
    convertUInt64ToBytes(*(uint64_t*)&val, dst, big_endian);
}

void convertUInt64ToBytes(uint64_t val, unsigned char* dst, bool big_endian)
{
    if(big_endian) {
        dst[0] = (val >> 56) & 0xFF;
        dst[1] = (val >> 48) & 0xFF;
        dst[2] = (val >> 40) & 0xFF;
        dst[3] = (val >> 32) & 0xFF;
        dst[4] = (val >> 24) & 0xFF;
        dst[5] = (val >> 16) & 0xFF;
        dst[6] = (val >> 8) & 0xFF;
        dst[7] = val & 0xFF;
    } else {
        dst[7] = (val >> 56) & 0xFF;
        dst[6] = (val >> 48) & 0xFF;
        dst[5] = (val >> 40) & 0xFF;
        dst[4] = (val >> 32) & 0xFF;
        dst[3] = (val >> 24) & 0xFF;
        dst[2] = (val >> 16) & 0xFF;
        dst[1] = (val >> 8) & 0xFF;
        dst[0] = val & 0xFF;
    }
}

}
