#include <pthread.h>
#include <stdio.h>

char *hello;
char *bye;
char * test;
_Atomic int changes = 0;

void *routine2(void *ptr)
{
	while (1)
	{
		test = hello;
		changes++;
	}
	return (NULL);
}
void *routine(void *ptr)
{
	while (1)
	{
		test = bye;
		changes++;
	}
	return (NULL);
}
#include <stdlib.h>

void	*observer(void *ptr)
{
	while (1)
		printf("%d\n", changes);
	return NULL;
}

int	main()
{
	hello = malloc(2);
	bye = alloca(2);
	printf("%ld\n", hello - bye);
	hello[0] = 'H';
	hello[1] = '\0';
	bye[0] = 'B';
	bye[1] = '\0';
	test = hello;
	pthread_t thread2;
	pthread_create(&thread2, NULL, &routine2, NULL);
	pthread_t thread;
	pthread_create(&thread, NULL, &routine, NULL);
	pthread_t obs;
	pthread_create(&obs, NULL, &observer, NULL);
	while (1)
	{
		char *s = test;
		if (s != hello && s != bye)
			printf("test bad\n%s", s);
	}return (0);
}
