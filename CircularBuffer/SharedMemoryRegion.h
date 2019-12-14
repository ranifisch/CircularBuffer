#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>


class SharedMemoryRegion
{
private:
	int m_length;
	char* m_start;
public:
	SharedMemoryRegion(bool is_writer=false);
	
	int getLength();
	char* getStart();
};