/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clovell <clovell@student.42adel.org.au>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 22:41:16 by clovell           #+#    #+#             */
/*   Updated: 2023/09/21 11:15:17 by clovell          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <inttypes.h>
#include <limits.h>

#include "philosophers.h"

int	main(int argc, char **argv)
{
	t_philo	*philos;
	t_args	args;
	int32_t	i;

	if (initialise(argc - 1, ++argv, &args))
		return (0);
	philos = construct(args);
	i = 0;
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
	cleanup(philos);
	return (0);
}

t_eachres	on_each_philo(t_philo *philo, int i, void *ctx)
{
	t_waitdeathctx *const	wd = ctx;

	(void)i;
	if (inta_get(philo->cancel) == 1)
		return (E_CANCEL);
	if (inta_get(&philo->eaten) < wd->min_eat)
		wd->min_eat = inta_get(&philo->eaten);
	if ((inta_get(&philo->tsle) + philo->ttd) < gettime_now())
	{
		change_state(philo, E_DIE);
		inta_set(philo->cancel, 1);
		inta_set(&philo->dead, true);
		return (E_CANCEL);
	}
	return (E_CONTINUE);
}

t_eachres	on_wait_thread_exit(t_philo *philo, int i, void *ctx)
{
	(void)i;
	(void)ctx;
	pthread_join(philo->thread, NULL);
	return (E_CONTINUE);
}

void	wait_death(t_philo *philos, t_args args)
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
	foreach_philo(philos, args.count, NULL, on_wait_thread_exit);
}
