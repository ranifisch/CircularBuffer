#include "CircularBuffer.h"

int main()
{
	CircularBuffer * circularBuffer = new CircularBuffer(true);

	char d_w[10] = "xxccccxxx";
	MsgHeader x;
	x.id = 30;
	x.len = 10;

	printf("%d\n", circularBuffer->sizeLeft());
	circularBuffer->Write(x, d_w);
	printf("%d\n", circularBuffer->sizeLeft());

	char d_r[200] = { 0 };

	circularBuffer->Read(d_r);


	getchar();

}