#include "CircularBuffer.h"


CircularBuffer::CircularBuffer(bool is_writer)
{
	m_shared_memory_region = new SharedMemoryRegion(is_writer);
	m_len = m_shared_memory_region->getLength();
	if (is_writer)
	{
		m_shared_queue_header = new SharedQueueHeader();
		m_shared_queue_header->rear = sizeof(SharedQueueHeader);
		m_shared_queue_header->front = m_shared_queue_header->rear;
		m_shared_queue_header->busy = false;
		memcpy(m_shared_memory_region->getStart(), m_shared_queue_header, sizeof(SharedQueueHeader));
	}
	else
	{
		memcpy(m_shared_queue_header, m_shared_memory_region->getStart(), sizeof(SharedQueueHeader));
	}
}

int CircularBuffer::sizeLeft()
{
	int rear = m_shared_queue_header->rear;
	int front = m_shared_queue_header->front;
	if (rear > front){
		return (m_len - (rear - front) - sizeof(SharedQueueHeader));
	}
	else if (front > rear)
	{
		return (front - rear);
	}
	else
	{
		return m_len - sizeof(SharedQueueHeader);
	}
}

bool CircularBuffer::isEmpty()
{
	return m_shared_queue_header->front == m_shared_queue_header->rear;
}

bool CircularBuffer::isFull()
{
	int rear = m_shared_queue_header->rear;
	int front = m_shared_queue_header->front;
	MsgHeader msgHeader;
	getNextMsgHeader(msgHeader);
	if (rear > front)
	{
		return (front + sizeof(MsgHeader)+msgHeader.len) == rear;
	}
	else
	{
		return (front + sizeof(MsgHeader)+msgHeader.len + sizeof(SharedQueueHeader)) % m_len == rear;
	}
}

bool CircularBuffer::Write(MsgHeader& msgHeader, char* data)
{
	int rear = m_shared_queue_header->rear;
	int front = m_shared_queue_header->front;
	char *shared_mem_start = m_shared_memory_region->getStart();

	if (msgHeader.len + sizeof(msgHeader) > sizeLeft())
	{
		printf("Not enough space to enter this msg...\n");
		return false;
	}

	int total_len = sizeof(MsgHeader) + msgHeader.len;

	char full_data[2000];

	memcpy(full_data, &msgHeader, sizeof(MsgHeader));
	memcpy(full_data + sizeof(MsgHeader), data, msgHeader.len);


	if (rear + total_len < m_len)	// covers the case of front > rear
	{
		memcpy(shared_mem_start + rear, full_data, total_len);
		m_shared_queue_header->rear += total_len;
	}
	else
	{
		int first_part_len = m_len - rear;
		int second_part_len = total_len - first_part_len;

		memcpy(shared_mem_start + rear, full_data, first_part_len);
		memcpy(shared_mem_start + sizeof(SharedQueueHeader), full_data + first_part_len, second_part_len);
		m_shared_queue_header->rear = sizeof(SharedQueueHeader)+second_part_len;
	}
}

void CircularBuffer::getNextMsgHeader(MsgHeader &msgHeader){
	int front = m_shared_queue_header->front;
	char *shared_mem_start = m_shared_memory_region->getStart();
	char headerData[100];

	if (front + sizeof(MsgHeader) < m_len) // header is not splitted
	{
		memcpy(&msgHeader, shared_mem_start + front, sizeof(MsgHeader));		
	}
	else
	{
		int first_part_len = m_len - front;
		int second_part_len = sizeof(MsgHeader)-first_part_len;
		memcpy(headerData, shared_mem_start + front, first_part_len);
		memcpy(headerData + first_part_len, shared_mem_start + sizeof(SharedQueueHeader), second_part_len);
		memcpy(&msgHeader, headerData, sizeof(MsgHeader));
	}
}

bool CircularBuffer::Read(char* data)
{
	int rear = m_shared_queue_header->rear;
	int front = m_shared_queue_header->front;
	char *shared_mem_start = m_shared_memory_region->getStart();
	MsgHeader msgHeader;

	char headerData[100];

	if (isEmpty())
	{
		printf("SharedMemory is empty...\n");
		return false;
	}

	// read header in both cases
	
	if (front + sizeof(MsgHeader) < m_len) // header is not splitted
	{
		memcpy(&msgHeader, shared_mem_start + front, sizeof(MsgHeader));
		front += sizeof(MsgHeader);
	}
	else
	{
		int first_part_len = m_len - front;
		int second_part_len = sizeof(MsgHeader)-first_part_len;

		memcpy(headerData, shared_mem_start + front, first_part_len);
		memcpy(headerData + first_part_len, shared_mem_start + sizeof(SharedQueueHeader), second_part_len);

		front = sizeof(SharedQueueHeader)+second_part_len;

		memcpy(&msgHeader, headerData, sizeof(MsgHeader));
	}

	// read the data now...

	int data_len = msgHeader.len;

	// normal scenario, easy copy.
	if (front + data_len < m_len)
	{
		memcpy(data, shared_mem_start + front, data_len);
		m_shared_queue_header->front = front + data_len;
	}
	else // splitted data
	{
		int first_part_len = m_len - front;
		int second_part_len = data_len - first_part_len;
		memcpy(data, shared_mem_start + front, first_part_len);
		memcpy(data + first_part_len, shared_mem_start + sizeof(SharedQueueHeader), second_part_len);
		m_shared_queue_header->front = sizeof(SharedQueueHeader)+second_part_len;
	}

	return true;
}

