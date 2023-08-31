#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#include "philosophers.h"

typedef struct s_philo	t_philo;
typedef struct s_args	t_args;

struct s_args
{
	int32_t	count;
	int32_t	starvation_time;
	int32_t	consumption_time;
	int32_t	sleeping_time;
	int32_t	cycles;
};

struct s_philo
{
	pthread_mutex_t stop;
	pthread_t		thread;
	t_fork	*left;
	t_fork	*right;

	int64_t id;
	bool	dead;

	int64_t eat;
	int64_t sleep;
	int64_t starve;
};


static	print_state(t_philo *philo, char *action)
{
	printf("%d %d %s\n", gettime_now() / 1000, philo->id, action);
}

void	observer(t_philo *philo)
{
	while (1)
	{
		pthread_mutex_lock(philo->stop);
		if (philo->tsle + philo->starve < gettime_now())
		{
			print_state(philo, "died");
			philo->dead = true;
			pthread_mutex_unlock(philo->stop);
			break ;
		}
		pthread_mutex_unlock(philo->stop);
	}
}


void	routine(t_philo *philo)
{
	philo->tsle = gettime_now();
	while (1)
	{
		print_state(philo, "is thinking");
		if (philo->left == philo->right)
			break;
		pthread_mutex_lock(philo->left);	
		print_state(philo, "has taken a fork");
		pthread_mutex_lock(philo->right);	
		print_state(philo, "has taken a fork");
		pthread_mutex_lock(philo->stop);
		if (philo->dead || philo->tsle + philo->starve < gettime_now())
			break;
		philo->tsle = gettime_now();
		pthread_mutex_unlock(philo->stop);
		print_state(philo, "is eating");
		usleep(philo->eat);
		print_state(philo, "is sleeping");
		usleep(philo->sleep);
	}
}

t_philo *construct(int count, t_args args)
{
	t_philo		*philos;
	t_fork		*forks;
	int			i;

	forks = calloc(count, sizeof(t_fork));
	philos = calloc(count, sizeof(t_philo));
	i = 0;
	while (i < count)
	{
		pthread_mutex_init(&forks[i], NULL);
		philos[i].id = i + 1;
		philos[i].starve = args.starvation_time;
		philos[i].eat = args.consumption_time;
		philos[i].sleep = args.sleeping_time;
		philos[i].left = &forks[i];
		philos[i].right = &forks[(i + 1) % count];
		philos[i].next = &philos[(i + 1) % count];
		i++;
	}
	return (&philos[0]);
}

t_args	initialise(int32_t argc, char **argv)
{
	t_args args;

	if (argc <= 1)
		return (NULL);
	if (argc == 5)
		args->cycles = ft_atoi(argv[4]);
	if (argc >= 4)
	{
		args->count = ft_atoi(argv[0]);
		args->starvation_time = ft_atoi(argv[1]) * 1000;
		args->consumption_time = ft_atoi(argv[2]) * 1000;
		args->sleeping_time = ft_atoi(argv[3]) * 1000;
	}
	return (args);
}

int main(int argc, char **argv)
{
	t_philo	*philos;
	t_philo *current;
	t_args	args;
	int32_t	i;

	args = initialise(argc - 1, ++argv);
	philos = construct(args->count, args);
	i = 0;
	while (i < args->count)
	{
		pthread_create(&philos[i].thread, NULL, &routine, &philos[i]);
		pthread_create(&philos[i].observer, NULL, &observer, &philos[i]);
		i++;
	}
	i = 0;
	while (i < args->count)
	{
		current = &philos[i++];
		pthread_join(current->thread, NULL);
	}
}
