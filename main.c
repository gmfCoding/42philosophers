#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <inttypes.h>
#include <limits.h>

#include "philosophers.h"


void	*routine(void *ptr)
{
	t_philo *const philo = ptr;

	while (inta_get(philo->wait) == 1)
		usleep(50);
	if (philo->id % 2 == 0)
		change_state(philo, E_THINK);
	if (philo->id % 2 == 0)
		usleep(philo->tte - 1000);
	while (inta_get(philo->cancel) == 0)
	{
		change_state(philo, E_THINK);
		pthread_mutex_lock(philo->left);
		change_state(philo, E_GRAB);
		pthread_mutex_lock(philo->right);
		change_state(philo, E_GRAB);
		inta_set(&philo->tsle, gettime_now());
		change_state(philo, E_EAT);
		usleep(philo->tte);
		inta_add(&philo->eaten, 1);
		pthread_mutex_unlock(philo->right);
		pthread_mutex_unlock(philo->left);
		change_state(philo, E_SLEEP);
		usleep(philo->tts);
	}
	return (NULL);
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
		args->ttd = ft_atoi(argv[1]) * 1000;
		args->tte = ft_atoi(argv[2]) * 1000;
		args->tts = ft_atoi(argv[3]) * 1000;
	}
	if (args->cycles < 0 || args->count <= 0 || args->ttd <= 60)
		return (true);
	if (args->tte <= 60 * 1000 || args->ttd <= 60 * 1000)
		return (true);
	if (args->tts <= 60 * 1000) // Should these be 60?
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
	t_args	args;
	int32_t	i;
	
//	signal(SIGINT, sigUsr1Handler);
	if (initialise(argc - 1, ++argv, &args))
		return (0);
	if (args.count <= 1)
		return (0);
	philos = construct(args);
	i = 0;
	//routine(&philos[0]);
	while (i < args.count)
	{
		pthread_create(&philos[i].thread, NULL, &routine, &philos[i]);
		i++;
	}
	while (--i >= 0)
	{
		philos[i].time = gettime_now();
		inta_set(&philos[i].tsle, philos[i].time);
	}
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

int	foreach_philo(t_philo *start, size_t size, void *ctx, t_onphilo_fn f)
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
	
	(void)i;
	if (inta_get(philo->cancel) == 1)
		return (E_CANCEL);
	if (inta_get(&philo->eaten) < wd->min_eat)
		wd->min_eat = inta_get(&philo->eaten);
	if ((inta_get(&philo->tsle) + philo->ttd) < gettime_now())
	{
		printf("%ld: %ld + %ld < %ld\n",philo->id, inta_get(&philo->tsle), philo->ttd, gettime_now());
		inta_set(philo->cancel, 1);
		change_state(philo, E_DIE);
		inta_set(&philo->dead, true);
		return (E_CANCEL);
	}
	return (E_CONTINUE);
}

void wait_death(t_philo *philos, t_args args)
{
	t_waitdeathctx	ctx;
	
	while (inta_get(philos[0].cancel) == 0)
	{
		ctx = (t_waitdeathctx){INT16_MAX};
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
