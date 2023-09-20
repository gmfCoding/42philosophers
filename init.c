/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clovell <clovell@student.42adel.org.au>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 22:17:03 by clovell           #+#    #+#             */
/*   Updated: 2023/09/20 22:47:48 by clovell          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include "philosophers.h"

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
	if (args->tts <= 60 * 1000)
		return (true);
	return (false);
}

t_philo	*construct(t_args args)
{
	t_philo		*philos;
	t_fork		*forks;
	int			i;
	t_inta64	*mcancel;
	int64_t		time;

	time = gettime_now();
	mcancel = malloc(sizeof (t_inta64) * 2);
	mcancel[0] = inta_init(0);
	mcancel[1] = inta_init(1);
	forks = calloc(args.count, sizeof(t_fork));
	philos = calloc(args.count, sizeof(t_philo));
	i = -1;
	while (++i < args.count)
	{
		pthread_mutex_init(&forks[i], NULL);
		philos[i] = (t_philo){
			.id = i + 1, .prev_action = E_DIE, .time = time,
			.cancel = &mcancel[0], .wait = &mcancel[1],
			.tte = args.tte, .tts = args.tts, .ttd = args.ttd,
			.left = &forks[i], .right = &forks[(i + 1) % args.count],
			.tsle = inta_init(0), .eaten = inta_init(0),
		};
	}
	return (&philos[0]);
}
