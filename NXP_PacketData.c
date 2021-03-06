#include <MKL25Z4.H>
#define MASK(x) (1UL << (x))
#define Q_SIZE (32)

typedef struct
{
	unsigned char Data[Q_SIZE];
	unsigned int Head; // points to oldest data element
	unsigned int Tail; // points to next free space
	unsigned int Size; // quantity of elements in queue
}
volatile Q_T; /*the entire data structure is defined as volatile because it will be shared between ISRs and regular code.*/

Q_T tx_q, rx_q;

void Q_Init(Q_T * q)
{
	unsigned int i;
	for (i=0; i<Q_SIZE; i++)
	q->Data[i] = 0;  // to simplify our lives when debugging
	q->Head = 0;
	q->Tail = 0;
	q->Size = 0;
}

int Q_Empty(Q_T * q) 
{
	return q->Size == 0;
}

int Q_Full(Q_T * q) 
{
return q->Size == Q_SIZE;
}


int Q_Enqueue(Q_T * q, unsigned char d)
{
	if (!Q_Full(q))
	{
		q->Data[q->Tail++] = d;
		q->Tail %= Q_SIZE;
		q->Size++;
		return 1; // success
	}else
	return 0; // FAILLUREE
}

unsigned char Q_Dequeue(Q_T * q) 
{
	// Must check to see if queue is empty before dequeueing
	unsigned char t=0;
	if (!Q_Empty(q)) 
		{
			t = q->Data[q->Head];
			q->Data[q->Head++] = 0;			// to simplify debugging
			q->Head %= Q_SIZE;
		  q->Size--;
		}
		return t;
}
