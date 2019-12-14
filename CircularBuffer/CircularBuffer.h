#include "SharedMemoryRegion.h"

struct SharedQueueHeader{
	int front;
	int rear;
	bool busy;
};

struct MsgHeader
{
	int id;
	int len;
};


class CircularBuffer
{
public:
	CircularBuffer(bool is_writer=false);
	bool Write(MsgHeader& msgHeader, char* data);
	bool Read(char* data);
	int sizeLeft();

private:
	SharedQueueHeader *m_shared_queue_header;
	SharedMemoryRegion *m_shared_memory_region;
	int m_len;

	bool isEmpty();
	bool isFull();

	void getNextMsgHeader(MsgHeader &msgHeader);

};