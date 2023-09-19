#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <inttypes.h>
#include <limits.h>

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
	static t_inta64		has_died = {PTHREAD_MUTEX_INITIALIZER, 0};
	const char *const	msg = get_statemsg(action);

	if (philo->prev_action == action)
		return ;
	philo->prev_action = action;
	if (inta_get(&has_died) == 1 || inta_get(philo->cancel) != 0)
	{
		pthread_mutex_lock(&philo->stop);
		philo->dead = true;
		pthread_mutex_unlock(&philo->stop);
		return ;
	}
	printf("%ld %ld %s\n", (gettime_now(0) - philo->time) / 1000, philo->id, msg);
	if (action == E_DIE)
	{
		inta_set(&has_died, 1);
		pthread_mutex_lock(&philo->stop);
		if (philo->dead)
			return ;
		pthread_mutex_unlock(&philo->stop);
	}
}

void	*observer(void *ptr)
{
	t_philo *const philo = ptr;

	while (inta_get(philo->wait) == 1)
		usleep(10);
	while (1)
	{
		usleep(5);
	}
	printf("Destroyed Observer thread: %"PRId64"\n", philo->id);
	return (NULL);
}

void	*routine(void *ptr)
{
	t_philo *const philo = ptr;
	while (inta_get(philo->wait) == 1)
		usleep(50);
	if (philo->id % 2 == 0)
		print_state(philo, E_THINK);
	if (philo->id % 2 == 0)
		usleep(philo->eat - 1000);
	while (inta_get(philo->cancel) == 0)
	{
		print_state(philo, E_THINK);
		pthread_mutex_lock(philo->left);
		print_state(philo, E_GRAB);
		pthread_mutex_lock(philo->right);
		print_state(philo, E_GRAB);
		pthread_mutex_lock(&philo->stop);
		philo->tsle = gettime_now(0) - philo->time;
		print_state(philo, E_EAT);
		philo->eaten++;
		pthread_mutex_unlock(&philo->stop);
		usleep(philo->eat);
		pthread_mutex_unlock(philo->right);
		pthread_mutex_unlock(philo->left);
		print_state(philo, E_SLEEP);
		usleep(philo->sleep);
	}
	pthread_mutex_unlock(philo->right);
	pthread_mutex_unlock(philo->left);
	printf("Destroyed Routine thread: %"PRId64"\n", philo->id); // REMOVE
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

	int64_t time = gettime_now(0);
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
		philo->time = time;
		philo->tsle = time;
		philo->eaten = 0;
		philo->prev_action = E_DIE;
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
	else
		args->cycles = 0;
	if (argc >= 4)
	{
		args->count = ft_atoi(argv[0]);
		args->starvation_time = ft_atoi(argv[1]) * 1000;
		args->consumption_time = ft_atoi(argv[2]) * 1000;
		args->sleeping_time = ft_atoi(argv[3]) * 1000;
	}
	if (args->count <= 0 || args->starvation_time <= 60)
		return (true);
	if (args->consumption_time <= 60 * 1000 || args->starvation_time <= 60 * 1000)
		return (true);
	if (args->sleeping_time <= 60 * 1000) // Should these be 60?
		return (true);
	return (false);
}

/*#include <dlfcn.h>*/
/*#include <stdio.h>*/
/*#include <unistd.h>*/
/*#include <signal.h>*/

/*void sigUsr1Handler(int sig)*/
/*{*/
/*    fprintf(stderr, "Exiting on SIGUSR1\n");*/
/*    void (*_mcleanup)(void);*/
/*    _mcleanup = (void (*)(void))dlsym(RTLD_DEFAULT, "_mcleanup");*/
/*    if (_mcleanup == NULL)*/
/*         fprintf(stderr, "Unable to find gprof exit hook\n");*/
/*    else _mcleanup();*/
/*    _exit(0);*/
/*}*/

void wait_death(t_philo *philos, t_args args);

int main(int argc, char **argv)
{
	t_philo	*philos;
	t_philo *current;
	t_args	args;
	int32_t	i;
	t_philo	*philo;
	
//	signal(SIGINT, sigUsr1Handler);
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
		i++;
	}	
	gettime_now(1);
	inta_set(philos[0].wait, 0);
	wait_death(philos, args);
	return (0);
}

typedef enum e_eachres t_eachres;

enum	e_eachres
{
	E_CANCEL,
	E_CONTINUE,
};
typedef struct s_waitdeathctx	t_waitdeathctx;
struct	s_waitdeathctx
{
	int16_t	min_eat;
};

typedef t_eachres (*t_onphilo_fn)(t_philo *, int i, void *ctx);

int	foreach_philo(t_philo *start, int size, void *ctx, t_onphilo_fn f)
{
	size_t	i;
	
	i = 0;
	while (i < size)
	{
		if (f(&start[i], i, ctx) == E_CANCEL)
			return (i);
		i++;
	}
	return (i);
}

t_eachres	on_each_philo(t_philo *philo, int i, void *ctx)
{
	t_waitdeathctx	*const wd = ctx;

	if (inta_get(philo->cancel) == 1)
		return (E_CANCEL);
	pthread_mutex_lock(&philo->stop);
	if (philo->eaten < wd->min_eat)
		wd->min_eat = philo->eaten;
	if ((philo->tsle + philo->starve) < (gettime_now(0) - philo->time))
	{
		inta_set(philo->cancel, 1);
		pthread_mutex_unlock(&philo->stop);
		print_state(philo, E_DIE);
		pthread_mutex_lock(&philo->stop);
		philo->dead = true;
		pthread_mutex_unlock(&philo->stop);
		return E_CANCEL;
	}
	pthread_mutex_unlock(&philo->stop);
}

void wait_death(t_philo *philos, t_args args)
{
	t_philo			*philo;
	size_t			i;
	int				min_eat;
	t_waitdeathctx	ctx;
	
	while (inta_get(philos[0].cancel) == 0)
	{
		ctx = (t_waitdeathctx){INT16_MAX};
		i = 0;
		foreach_philo(philos, args.count, &ctx, on_each_philo);
		if (args.cycles > 0 && ctx.min_eat >= args.cycles)
			inta_set(philos[0].cancel, 1);
		usleep(5000);
	}
}


const char* __asan_default_options() { 
	// REMOVE BEFORE EVAL
	return "detect_leaks=0";
	//return "";
}
