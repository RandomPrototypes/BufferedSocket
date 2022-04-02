#include <string.h>
#include <stdio.h>
#include <DataPacket.h>

class DataPacketImpl : public DataPacket
{
public:
    DataPacketImpl();
    virtual ~DataPacketImpl();

    virtual int size();
    virtual const unsigned char *getRawPtr();
    virtual void rewind();

    virtual bool readInt32(int32_t *out);
    virtual bool readUInt32(uint32_t *out);
    virtual bool readInt64(int64_t *out);
    virtual bool readUInt64(uint64_t *out);

    virtual void putInt32(int32_t val);
    virtual void putUInt32(uint32_t val);
    virtual void putInt64(int64_t val);
    virtual void putUInt64(uint64_t val);

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

bool DataPacketImpl::readInt32(int32_t *out)
{
    return readUInt32((uint32_t*)out);
}
bool DataPacketImpl::readUInt32(uint32_t *out)
{
    if(offset + 4 > data.size())
        return false;

    *out = convertBytesToUInt32((unsigned char*)&data[offset]);
    offset += 4;
    return true;
}

bool DataPacketImpl::readInt64(int64_t *out)
{
    return readUInt64((uint64_t*)out);
}

bool DataPacketImpl::readUInt64(uint64_t *out)
{
    if(offset + 8 > data.size())
        return false;

    *out = convertBytesToUInt64((unsigned char*)&data[offset]);
    offset += 8;
    return true;
}

void DataPacketImpl::putInt32(int32_t val)
{
    putUInt32(*(uint32_t*)&val);
}
void DataPacketImpl::putUInt32(uint32_t val)
{
    unsigned char buf[4];
    convertUInt32ToBytes(val, buf);
    putNBytes(buf, 4);
}
void DataPacketImpl::putInt64(int64_t val)
{
    putUInt64(*(uint64_t*)&val);
}
void DataPacketImpl::putUInt64(uint64_t val)
{
    unsigned char buf[8];
    convertUInt64ToBytes(val, buf);
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

int32_t convertBytesToInt32(const unsigned char* ptr)
{
    uint32_t val = convertBytesToUInt32(ptr);
    return *(int32_t*)&val;
}

uint32_t convertBytesToUInt32(const unsigned char* ptr)
{
    return ((uint32_t) ptr[3] << 0)
         | ((uint32_t) ptr[2] << 8)
         | ((uint32_t) ptr[1] << 16)
         | ((uint32_t) ptr[0] << 24);
}

int64_t convertBytesToInt64(const unsigned char* ptr)
{
    uint64_t val = convertBytesToUInt64(ptr);
    return *(int64_t*)&val;
}

uint64_t convertBytesToUInt64(const unsigned char* ptr)
{
    return ((uint64_t) ptr[7] << 0)
         | ((uint64_t) ptr[6] << 8)
         | ((uint64_t) ptr[5] << 16)
         | ((uint64_t) ptr[4] << 24)
         | ((uint64_t) ptr[3] << 32)
         | ((uint64_t) ptr[2] << 40)
         | ((uint64_t) ptr[1] << 48)
         | ((uint64_t) ptr[0] << 56);
}

void convertInt32ToBytes(int32_t val, unsigned char* dst)
{
    convertUInt32ToBytes(*(uint32_t*)&val, dst);
}

void convertUInt32ToBytes(uint32_t val, unsigned char* dst)
{
    dst[0] = (val >> 24) & 0xFF;
    dst[1] = (val >> 16) & 0xFF;
    dst[2] = (val >> 8) & 0xFF;
    dst[3] = val & 0xFF;
}

void convertInt64ToBytes(int64_t val, unsigned char* dst)
{
    convertUInt64ToBytes(*(uint64_t*)&val, dst);
}

void convertUInt64ToBytes(uint64_t val, unsigned char* dst)
{
    dst[0] = (val >> 56) & 0xFF;
    dst[1] = (val >> 48) & 0xFF;
    dst[2] = (val >> 40) & 0xFF;
    dst[3] = (val >> 32) & 0xFF;
    dst[4] = (val >> 24) & 0xFF;
    dst[5] = (val >> 16) & 0xFF;
    dst[6] = (val >> 8) & 0xFF;
    dst[7] = val & 0xFF;
}

}
