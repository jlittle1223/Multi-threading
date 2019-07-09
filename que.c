/* compile with  -std=c99 */
/* this is a circular buffer implementation of queue. You can see about it here
 * http://www.csanimated.com/animation.php?t=Circular_buffer */

#include <stdio.h>
#include <stdlib.h>
/* Note: one slot must be left unused for this implementation */
#define QUE_MAX 1024
#define ELE int


ELE _que[QUE_MAX];
int _front = 0, _rear = 0;

void que_error(char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    exit(-1);
}

int que_is_full()
{
    return (_rear + 1) % QUE_MAX == _front; /* this is why one slot is unused */
}

int que_is_empty()
{
    return _front == _rear;
}

void que_enq(ELE v)
{
    if ( que_is_full() )
        que_error("enq on full queue");
    _que[_rear++] = v;
    if ( _rear >= QUE_MAX )
        _rear = 0;
}

ELE que_deq()
{
    if ( que_is_empty() )
        que_error("deq on empty queue");
    ELE ret = _que[_front++];
    if ( _front >= QUE_MAX )
        _front = 0;
    return ret;
}


int main()
{
    for ( int i=0; i<100; ++i )
        que_enq(i);
    while ( !que_is_empty() )
        printf("%d ", que_deq());
    putchar('\n');
}
