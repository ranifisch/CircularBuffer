#include "SharedMemoryRegion.h"


SharedMemoryRegion::SharedMemoryRegion(bool is_writer)
{
	HANDLE hMapFile = NULL;
	char* pBuf;
	m_length = 1000;
	if (is_writer)
	{

		hMapFile = CreateFileMapping(
			INVALID_HANDLE_VALUE,    // use paging file
			NULL,                    // default security
			PAGE_READWRITE,          // read/write access
			0,                       // maximum object size (high-order DWORD)
			m_length,                // maximum object size (low-order DWORD)
			"wtf");                 // name of mapping object
	}
	else
	{
		OpenFileMapping(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			"wtf");
	}
	
	m_start  = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, m_length);
}

int SharedMemoryRegion::getLength()
{
	return m_length;
}

char* SharedMemoryRegion::getStart()
{
	return m_start;
}

