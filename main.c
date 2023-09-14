#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <inttypes.h>

#include "philosophers.h"

static bool	ft_isspace(char c)
{
	bool istab;

	istab = (c == '\t' || c == '\v');
	return (c == '\n' || c == '\f' || c == '\r' || c == ' ' || istab);
}

int32_t	ft_atoi(char *str)
{
	int32_t	negative;
	int32_t	accum;

	accum = 0;
	negative = 1;
	while (ft_isspace(*str))
			str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			negative = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		accum = accum * 10 + (*str - '0');
		str++;
	}
	return (accum * negative);
}

int64_t gettime_now(int64_t start)
{
	int64_t us;
	struct timeval time;

	gettimeofday(&time, NULL);
	us = time.tv_sec * 	1000000.0;
	us += time.tv_usec;
	return (us);
}

const char *get_statemsg(t_action action)
{
	const	char* const	msg[] = 
	{
		[E_DIE] = "died",
		[E_GRAB] = "has taken a fork",
		[E_SLEEP] = "is sleeping",
		[E_EAT] = "is eating",
		[E_THINK] = "is thinking",
	};

	return (msg[action]);
}

static void	print_state(t_philo *philo, t_action action)
{
	static pthread_mutex_t		lock = PTHREAD_MUTEX_INITIALIZER;
	static bool			has_died = false;
	const char* const	msg = get_statemsg(action);

	pthread_mutex_lock(&lock);
	if (has_died)
	{
		pthread_mutex_lock(&philo->stop);
		philo->dead = true;
		pthread_mutex_unlock(&philo->stop);
		pthread_mutex_unlock(&lock);
		return ;
	}
	printf("%ld %ld %s\n", (gettime_now(0) - philo->time) / 1000, philo->id, msg);
	if (action == E_DIE)
	{
		has_died = true;
		pthread_mutex_lock(&philo->stop);
		if (philo->dead)
		{
			pthread_mutex_unlock(&lock);
			return ;
		}
		pthread_mutex_unlock(&philo->stop);
	}
	pthread_mutex_unlock(&lock);
}

void	*observer(void *ptr)
{
	t_philo *const philo = ptr;

	while (inta_get(philo->wait) == 1)
		usleep(10);
	while (1)
	{
		pthread_mutex_lock(&philo->stop);
		if ((philo->tsle + philo->starve) < (gettime_now(0) - philo->time))
		{
			inta_set(philo->cancel, 1);
			pthread_mutex_unlock(&philo->stop);
			print_state(philo, E_DIE);
			pthread_mutex_lock(&philo->stop);
			philo->dead = true;
			pthread_mutex_unlock(&philo->stop);
			break ;
		}
		pthread_mutex_unlock(&philo->stop);
		usleep(5);
	}
	printf("Destroyed Observer thread: %"PRId64"\n", philo->id);
	return (NULL);
}

void	*routine(void *ptr)
{
	t_philo *const philo = ptr;
	
	while (inta_get(philo->wait) == 1)
		usleep(10);
	if (philo->id % 2 == 0)
		usleep(1000);
	philo->tsle = gettime_now(0);
	while (1)
	{
		if (philo->nobias)
		{
			print_state(philo, E_THINK);
			pthread_mutex_lock(philo->left);	
			print_state(philo, E_GRAB);
			pthread_mutex_lock(philo->right);	
			print_state(philo, E_GRAB);
		}
		philo->nobias = true;
		pthread_mutex_lock(&philo->stop);
		if (inta_get(philo->cancel) || philo->tsle + philo->starve < gettime_now(0))
			break;
		philo->tsle = gettime_now(0) - philo->time;
		pthread_mutex_unlock(&philo->stop);
		print_state(philo, E_EAT);
		usleep(philo->eat);
		pthread_mutex_unlock(philo->right);
		pthread_mutex_unlock(philo->left);
		print_state(philo, E_SLEEP);
		usleep(philo->sleep);
	}
	pthread_mutex_unlock(philo->right);
	pthread_mutex_unlock(philo->left);
	printf("Destroyed Routine thread: %"PRId64"\n", philo->id);
	return (NULL);
}

t_philo *construct(int count, t_args args)
{
	t_philo		*philos;
	t_philo		*philo;
	t_fork		*forks;
	int			i;
	t_inta64	*mcancel;

	mcancel = malloc(sizeof (t_inta64) * 2);
	mcancel[0] = (t_inta64){PTHREAD_MUTEX_INITIALIZER, 0};
	mcancel[1] = (t_inta64){PTHREAD_MUTEX_INITIALIZER, 1};
	forks = calloc(count, sizeof(t_fork));
	philos = calloc(count, sizeof(t_philo));
	i = 0;
	while (i < count)
	{
		philo = &philos[i];
		pthread_mutex_init(&forks[i], NULL);
		philo->cancel = &mcancel[0];
		philo->wait	= &mcancel[1];
		philo->id = i + 1;
		philo->starve = args.starvation_time;
		philo->eat = args.consumption_time;
		philo->sleep = args.sleeping_time;
		philo->left = &forks[i];
		philo->right = &forks[(i + 1) % count];
		philo->nobias = !(i % 2 == 0);
		philo->time = gettime_now(0);
		if (i % 2 == 0)
		{
			print_state(philo, E_THINK);
			pthread_mutex_lock(philo->left);	
			print_state(philo, E_GRAB);
			pthread_mutex_lock(philo->right);	
			print_state(philo, E_GRAB);	
		}
		i++;
	}
	return (&philos[0]);
}

bool	initialise(int32_t argc, char **argv, t_args *args)
{
	if (argc <= 1)
		return (true);
	if (argc == 5)
		args->cycles = ft_atoi(argv[4]);
	if (argc >= 4)
	{
		args->count = ft_atoi(argv[0]);
		args->starvation_time = ft_atoi(argv[1]) * 1000;
		args->consumption_time = ft_atoi(argv[2]) * 1000;
		args->sleeping_time = ft_atoi(argv[3]) * 1000;
	}
	return (false);
}

int main(int argc, char **argv)
{
	t_philo	*philos;
	t_philo *current;
	t_args	args;
	int32_t	i;

	if (initialise(argc - 1, ++argv, &args))
		return (0);
	if (args.count <= 1)
		return (0);
	philos = construct(args.count, args);
	i = 0;
	//routine(&philos[0]);
	while (i < args.count)
	{
		pthread_create(&philos[i].thread, NULL, &routine, &philos[i]);
		pthread_create(&philos[i].observer, NULL, &observer, &philos[i]);
		i++;
	}
	gettime_now(1);
	inta_set(philos[0].wait, 0);
	i = 0;
	while (i < args.count)
	{
		current = &philos[i++];
		pthread_join(current->thread, NULL);
	}
	return (0);
}

const char* __asan_default_options() { 
	// REMOVE BEFORE EVAL
	return "detect_leaks=0";
	//return "";
}
