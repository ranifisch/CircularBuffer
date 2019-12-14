#include "CircularBuffer.h"

int main()
{
	CircularBuffer * circularBuffer = new CircularBuffer(true);

	printf("size of shared_queue_header: %d\n", sizeof(SharedQueueHeader));
	printf("size of msg_header: %d\n", sizeof(MsgHeader));

	printf("%d\n", circularBuffer->sizeLeft());

	for (int j = 0; j < 100; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			char d_w[15] = "xxccccxxxxxxxx";
			MsgHeader x;
			x.id = 30;
			x.len = 15;
			circularBuffer->Write(x, d_w);
			printf("%d\n", circularBuffer->sizeLeft());
		}

		for (int i = 0; i < 3; i++)
		{
			char d_r[15];
			MsgHeader x;
			circularBuffer->Read(d_r);
			printf("%d\n", circularBuffer->sizeLeft());
			printf("%s\n", d_r);
		}
	}

	getchar();

}