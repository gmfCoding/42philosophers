#include "philosophers.h"

t_inta64 *inta_add(t_inta64 *a, int64_t v)
{
	pthread_mutex_lock(&a->thread);
	a->value += v;
	pthread_mutex_unlock(&a->thread);
	return (a);
}

t_inta64 *inta_set(t_inta64 *a, int64_t v)
{
	pthread_mutex_lock(&a->thread);
	a->value = v;
	pthread_mutex_unlock(&a->thread);
	return (a);
}

int64_t		inta_get(t_inta64 *a)
{
	int64_t	value;
	pthread_mutex_lock(&a->thread);
	value = a->value;
	pthread_mutex_unlock(&a->thread);
	return (value);
}
