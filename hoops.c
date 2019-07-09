#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#define MAX_PLAYERS 10000

struct shot
{

    int player_num;
    int points;

};


int Time = 1;
int N = 1;
int num_current_players = 0;

int scores[MAX_PLAYERS];



pthread_mutex_t waiting_mutex;
pthread_cond_t waiting_cond;



/* Note: one slot must be left unused for this implementation */
#define QUE_MAX 1024
#define ELE struct shot


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

void enque(ELE v)
{
    if ( que_is_full() )
        que_error("enq on full queue");
    _que[_rear++] = v;
    if ( _rear >= QUE_MAX )
        _rear = 0;
}

ELE deque()
{
    if ( que_is_empty() )
        que_error("deq on empty queue");
    ELE ret = _que[_front++];
    if ( _front >= QUE_MAX )
        _front = 0;
    return ret;
}



void* player_thread(void* args)
{
    
    ++num_current_players;

    int player_num = (int) args;

    //printf("creating player %d\n", player_num);
    //wait for all players
    
    pthread_mutex_lock(&waiting_mutex);
    

    while (num_current_players < N)
    {
        
        pthread_cond_wait(&waiting_cond, &waiting_mutex);
    }

    pthread_mutex_unlock(&waiting_mutex);

    pthread_cond_broadcast(&waiting_cond);

    //Everyone is ready now

    printf("Player %d has started playing!\n", player_num);

    for (int i = 0; i < Time; ++i)
    {
        sleep(1);
        //shoot hoop

        int points = 1;
        while (points == 1)
            points = rand() % 4;    // 0 to 3. reroll on a 1
        
        printf("Player %d shoots for %d points!\n", player_num, points);

        struct shot single_shot;
        single_shot.player_num = player_num;
        single_shot.points = points;

        enque(single_shot);
    }

    pthread_exit(NULL);

}



void evaluate_queue()
{

    while (!que_is_empty())
    {
        struct shot player_shot = deque();
        int player_num = player_shot.player_num;
        int points = player_shot.points;

        scores[player_num] += points;       //add the points from the shot to that player's score

        printf("Player %d -> Score %d\n", player_num, points);

    }

}


void print_scores()
{

    printf("Total Scores:\n");
    for (int i = 0; i < N; ++i)
    {
        int score = scores[i];
        printf("Player %d Score: %d\n", i, score);
    }

}




int main(int argc, char* argv[])
{

    if (argc != 3)
    {
        perror("Wrong number of arguments to main");
        exit(-1);
    }

    N = atoi(argv[1]);              // Number of players
    Time = atoi(argv[2]);           // Amount of time to play (in seconds)

    
    srand(time(0));

   

     // spawn all threads
    pthread_t threads[N];

    for (long i = 0; i < N; ++i)
    {
        //printf("attempting to spawn thread number %d\n", i);
        int rc;
        if ( rc = pthread_create(&threads[i], NULL, player_thread, (void*) i) )
        {
            fprintf(stderr, "Error creating thread number %d. rc = %d\n", i, rc);
            exit(-1);
        }
        
    }



    // start time
    
    // end time    

    for (long i = 0; i < N; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    

    // evaluate queue
    evaluate_queue();

    // evaluate player scores
    print_scores();

    pthread_exit(NULL);

    return 0;
}



