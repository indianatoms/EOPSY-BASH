#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define N	5
#define LEFT	( id + N - 1 ) % N
#define RIGHT	( id + 1 ) % N

#define THINKING    1
#define HUNGRY      2
#define EATING      3
#define FOOD_LIMIT	4
#define EATING_TIME	1
#define THINKING_TIME	2
#define	up		pthread_mutex_unlock
#define	down		pthread_mutex_lock

pthread_mutex_t m; 		//internal mutex initialized to 1
pthread_mutex_t s[N];   //array of mutexes initialized to 0
pthread_t p[N];
int	state[N];

void test(int id) {
    //check if philo is hungry and if yes set it to eating
	if( state[id] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING )
	{
		state[id] = EATING;
		up(&s[id]); //unlock its mutex in order to allow grabing forks
	}
}

void grab_forks(int id)
{
	down(&m);//lock the internal mutex in order to disallow more than one process es doing same thing
		state[id] = HUNGRY;
		test(id);
	up(&m);
	down(&s[id]); //lock the motex of followed id, will be unlocked after puting  away forks
}

void put_away_forks(int id)
{
	down(&m);//lock the internal mutex in order to disallow more than one process es doing same thing
		state[id] = THINKING;
		test(LEFT);
		test(RIGHT);
	up(&m);// unlock internal mutex
}

void *philocycle(int id){
	int meals_left=FOOD_LIMIT;
	printf("p%i comes to the table.\n", id);
	while(meals_left){
		printf("%i is thinking.\n", id);
		sleep(THINKING_TIME);
		grab_forks(id);
		meals_left--;
		printf("%i is eating the meal no. %d.\n", id,(FOOD_LIMIT-meals_left));
		sleep(EATING_TIME);
		put_away_forks(id);
	}
	printf("p%i ate it all.\n", id);
}

int main()
{
	pthread_mutex_init(&m, NULL);//internal mutex initialization
	up(&m);//unlocked at init

		for (int i = 0 ; i < N ; i++)
        {
            p[i] = THINKING;//all philosopfers thinkig
            pthread_mutex_init(&s[i], NULL);//block all mutex cause all are thinking
            printf("P%i is thinking\n",i);
            down(&s[i]);
        }

for (int i = 0 ; i < N ; i++)
		pthread_create(&p[i], NULL, (void *)philocycle, (void *)i);//create a thread

	for (int i = 0 ; i < N ; i++)
		pthread_join(p[i], NULL);//destroy the thuread

	for (int i = 0 ; i < N ; i++)
		pthread_mutex_destroy(&s[i]);//destroy mutexes



    return 0;
}
