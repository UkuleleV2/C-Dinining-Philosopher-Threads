#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#define N	5
#define LEFT	( i + N - 1 ) % N
#define RIGHT	( i + 1 ) % N

#define THINKING 0
#define HUNGRY 1
#define EATING 2
 
int state[N];
int phil[N] = { 0, 1, 2, 3, 4 };
pthread_mutex_t mutex;
pthread_mutex_t s[N]; 
static volatile sig_atomic_t interrupt_flag = 0;
static void set_interrupt_flag(int signo) 
{
	interrupt_flag = 1;
}
void test( int i )
{
	if( state[i] == HUNGRY
		&& state[LEFT] != EATING
		&& state[RIGHT] != EATING)
	{
		state[i] = EATING;
        printf("[Philosopher %d] I am taking fork %d and %d\n",
                      i + 1, LEFT + 1, i + 1);
        printf("[Philosopher %d] I am eating\n", i + 1);
        sleep(2);
		pthread_mutex_unlock(&s[i]);
	}
}
void grab_forks( int i )
{
	pthread_mutex_lock( &mutex );
    state[i] = HUNGRY;
    test( i );
	pthread_mutex_unlock( &mutex );
	pthread_mutex_lock( &s[i] );
}

void put_away_forks( int i )
{
    pthread_mutex_lock( &mutex );
    state[i] = THINKING;
    printf("[Philosopher %d] I am putting fork %d and %d down\n",
    i + 1, LEFT + 1, i + 1);
    printf("[Philosopher %d] I am thinking\n", i + 1);
    test( LEFT );
    test( RIGHT );
    pthread_mutex_unlock( &mutex );
}


 
void* philospher(void* num)
{
    while (1) 
    {
        int* i = num;
        sleep(1);
        grab_forks(*i);
        put_away_forks(*i);
        if(interrupt_flag) 
		{
            pthread_mutex_destroy(&s[*i]);
            pthread_mutex_destroy(&mutex);
            exit(0);
		}
    }
}
 
int main()
{
    signal(SIGINT, set_interrupt_flag);
    pthread_t thread_id[N];
    // initialize mutex
    pthread_mutex_init(&mutex, NULL);
 
    for (int i = 0; i < N; i++)
    {
        pthread_mutex_init (&s[i], NULL);
        pthread_mutex_lock (&s[i]);
    }
        
    for (int i = 0; i < N; i++) 
    {
        pthread_create(&thread_id[i], NULL,
                       philospher, &phil[i]); 
        printf("[Philosopher %d] I am thinking\n", i + 1);
    }
 
    for (int i = 0; i < N; i++)
        pthread_join(thread_id[i], NULL);
}
