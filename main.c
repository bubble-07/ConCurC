#include <stdio.h>
#include <stdlib.h>

#define WORDSIZE 4

#define nothing 0
#define expression 1
#define integer 2
#define function 3
#define curlist 4
typedef char type;
typedef struct object {
	type objtype;
	void* objaddr;
} object;

const object globalnothing = {nothing, NULL};

void* fromheap;
int fromheapsize;
void* toheap;
int toheapsize;

typedef struct list {
	object car;
	object cdr;
} list;
void raise(char* exception, object conflict) {
	printf(exception);
	int x = 1 / 0;
	return;
}
object constructexpression(object car, object cdr) {
	object ret;
	ret.objtype = expression;
	ret.objaddr = malloc(sizeof(list));
	list* temppointer = ret.objaddr;
	temppointer->car = car;
	temppointer->cdr = cdr;
	return ret;
}
list getlist(object in) {
	list temp;
	if ((in.objtype == expression) | (in.objtype == curlist)) {
		list temp = *((list*)in.objaddr);
		return temp;
	}
	raise("This object is not a list, but you tried to get a list out of it", in);
	return temp;
}
typedef list (*functionptr)(list);
object constructint(int value) {
	//int is an "unboxed" type. the "objaddr" holds the value directly
	object temp;
	temp.objtype = integer;
	temp.objaddr = (void*) value;
	return temp;
}
int addstrap(list toadd) {
	return (int)toadd.car.objaddr + addstrap(getlist(toadd.cdr));
}
							
void printint(object toprint) {
	if (toprint.objtype == integer) {
		printf("%i", toprint.objaddr);
	}
	return;
}

int main (int argc, const char * argv[]) {
	//Start the heap off with a decent-sized chunk of memory
    printint(constructint(5));
    return 0;
}
