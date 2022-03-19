#include "../ass.h"
IBios* interface;

extern void* heap;

struct s_node
{
    void *data;
    struct s_node *prev;
    struct s_node *next;
} *front=NULL, *back=NULL;
typedef struct s_node node;
void* dequeue( )
{
    node *oldfront = front;
    front = front->next;
    return oldfront->data;
}

// Queues a position at the back
void enqueue( void* data )
{
   node *newnode = (node*) malloc( sizeof( node ) );
   newnode->data = data;

   if( front == NULL && back == NULL )
       front = back = newnode;
   else
   {
       back->next = newnode;
       newnode->prev = back;
       back = newnode;
   }
}

void WaitForKey()
{
	while (INP_KEYIN == 0) { vbl(); }
}

#define WAIT \
	TEXT->SetTextColor(0, 4); \
	printf("[MORE]\n"); \
	TEXT->SetTextColor(0, 12); \
	WaitForKey();

#define END \
	TEXT->SetTextColor(0, 4); \
	printf("[END]\n"); \

int main(void)
{
	heap = (void*)0x03000000;
	MISC->SetTextMode(SMODE_240);
	TEXT->SetTextColor(0, 4);
	TEXT->ClearScreen();
	TEXT->SetCursorPosition(0, 0);

	TEXT->SetTextColor(0, 12);
	printf("The heap is at $%X, but we haven't done anything yet.\n", heap);
	void* a = malloc(16);
	printf("Starting off small: a = malloc(16) returns $%X, heap is now $%X.\n", a, heap);
	free(a);
	printf("free(a): a is $%X (now invalid), heap is $%X.\n", a, heap);
	a = malloc(16);
	printf("Repeat: a = malloc(16) returns $%X, heap is now $%X.\n", a, heap);
	free(a);
	printf("free(a): a is $%X (now invalid), heap is $%X.\n", a, heap);
	a = malloc(0x4B000);
	printf("Enough for high-256: 640*480 = 307200 or $4B000.\na = malloc(that) returns $%X, heap is now $%X.\n", a, heap);
	free(a);
	printf("free(a): a is $%X (now invalid), heap is $%X.\n", a, heap);
	a = malloc(0x4B000);
	printf("Repeat: a = malloc(0x4B000) returns $%X, heap is now $%X.\n", a, heap);
	free(a);
	printf("free(a): a is $%X (now invalid), heap is $%X.\n", a, heap);
	WAIT;
	printf("Doing SEVERAL large mallocs at once!\n");
	a = malloc(0x9600);
	printf("Enough for low-16: 320*240/2 = 38400 or $9600.\na = malloc(that) returns $%X, heap is now $%X.\n", a, heap);
	void *b = malloc(0x12C00);
	printf("Enough for low-256: 320*240 = 76800 or $12C00.\nb = malloc(that) returns $%X, heap is now $%X.\n", b, heap);
	void *c = malloc(0x25800);
	printf("Enough for high-16: 640*480/2 = 153600 or $25800.\nc = malloc(that) returns $%X, heap is now $%X.\n", c, heap);
	void *d = malloc(0x4B000);
	printf("Enough for high-256: 640*480 = 307200 or $4B000.\nd = malloc(that) returns $%X, heap is now $%X.\n", d, heap);
	free(a);
	printf("free(a): a is $%X (now invalid), heap is $%X.\n", a, heap);
	free(b);
	printf("free(b): b is $%X (now invalid), heap is $%X.\n", b, heap);
	free(c);
	printf("free(c): c is $%X (now invalid), heap is $%X.\n", c, heap);
	free(d);
	printf("free(d): d is $%X (now invalid), heap is $%X.\n", d, heap);
	WAIT;
	d = malloc(0x4B000);
	printf("Enough for high-256: 640*480 = 307200 or $4B000.\nd = malloc(that) returns $%X, heap is now $%X.\n", d, heap);
	WAIT;
	printf("Adding 16 nodes in a linked list...");
	for (int i = 0; i < 16; i++)
	{
		printf("%i... ", i);
		enqueue((void*)i);
	}
	printf("\nHeap is $%X.\n", heap);
	WAIT;
	printf("Adding 16 more nodes...");
	for (int i = 16; i < 32; i++)
	{
		printf("%i... ", i);
		enqueue((void*)i);
	}
	printf("\nHeap is $%X.\n", heap);
	WAIT;
	printf("Removing 8 nodes...");
	for (int i = 0; i < 8; i++)
	{
		int j = (int)dequeue();
		printf("%i... ", j);
	}
	printf("\nHeap is $%X.\n", heap);
	WAIT;
	printf("Adding 16 more nodes...");
	for (int i = 32; i < 48; i++)
	{
		printf("%i... ", i);
		enqueue((void*)i);
	}
	printf("\nHeap is $%X.\n", heap);
	WAIT;
	printf("Removing 8 nodes...");
	for (int i = 0; i < 8; i++)
	{
		int j = (int)dequeue();
		printf("%i... ", j);
	}
	printf("\nHeap is $%X.\n", heap);
	WAIT;
	printf("Freeing entire list...");
	while(front)
	{
		node *n = front;
		front = front->next;
		free(n);
	}
	printf("\nHeap is $%X.\n", heap);

	TEXT->SetTextColor(0, 4); printf("[END]\n"); TEXT->SetTextColor(0, 12);
}
