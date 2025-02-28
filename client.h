#ifndef CLIENT_H
#define CLIENT_H 

#include <pthread.h>
	
#define MAXLINE 1024

int init_client(void);

extern const int POLLING_RATE_HZ;

extern pthread_mutex_t game_state_mutex;

#endif // !CLIENT_H
