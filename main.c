#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct s_philo	t_philo;
typedef struct s_args	t_args;
typedef pthread_mutex_t t_fork;

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
	pthread_t		observer;
	t_fork	*left;
	t_fork	*right;

	int64_t id;
	bool	dead;
	int64_t	tsle;
	int64_t eat;
	int64_t sleep;
	int64_t starve;
};

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
	static int64_t begin;
	int64_t us;
	struct timeval time;

	if (start != 0)
		begin = gettime_now(0);
	gettimeofday(&time, NULL);
	us = time.tv_sec * 	1000000.0;
	us += time.tv_usec;
	return (us - begin);	
}

static void	print_state(t_philo *philo, bool wait, char *action)
{
	if (wait)
	{
		pthread_mutex_lock(&philo->stop);
		if (philo->dead)
			return ;
		pthread_mutex_unlock(&philo->stop);
	}
	printf("%ld %ld %s\n", gettime_now(0) / 1000, philo->id, action);
}

void	*observer(void *ptr)
{
	t_philo *const philo = ptr;

	while (1)
	{
		pthread_mutex_lock(&philo->stop);
		if (philo->tsle + philo->starve < gettime_now(0))
		{
			print_state(philo, false, "died");
			philo->dead = true;
			pthread_mutex_unlock(&philo->stop);
			break ;
		}
		pthread_mutex_unlock(&philo->stop);
	}
	return (NULL);
}


void	*routine(void *ptr)
{
	t_philo *const philo = ptr;

	philo->tsle = gettime_now(0);
	while (1)
	{
		print_state(philo, true, "is thinking");
		if (philo->left == philo->right)
			break;
		pthread_mutex_lock(philo->left);	
		print_state(philo, true, "has taken a fork");
		pthread_mutex_lock(philo->right);	
		print_state(philo, true, "has taken a fork");
		pthread_mutex_lock(&philo->stop);
		if (philo->dead || philo->tsle + philo->starve < gettime_now(0))
			break;
		philo->tsle = gettime_now(0);
		pthread_mutex_unlock(&philo->stop);
		print_state(philo, true, "is eating");
		usleep(philo->eat);
		pthread_mutex_unlock(philo->right);
		pthread_mutex_unlock(philo->left);
		print_state(philo, true, "is sleeping");
		usleep(philo->sleep);
	}
	return (NULL);
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
	philos = construct(args.count, args);
	i = 0;
	gettime_now(1);
	//routine(&philos[0]);
	while (i < args.count)
	{
		pthread_create(&philos[i].thread, NULL, &routine, &philos[i]);
		pthread_create(&philos[i].observer, NULL, &observer, &philos[i]);
		i++;
	}
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
