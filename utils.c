/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clovell <clovell@student.42adel.org.au>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:35:35 by clovell           #+#    #+#             */
/*   Updated: 2023/09/20 22:00:27 by clovell          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdlib.h>
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
	i = 0;
	while (i < args.count)
	{
		pthread_mutex_init(&forks[i], NULL);
		philos[i] = (t_philo){
		   	.id = i + 1,
			.cancel = &mcancel[0], .wait = &mcancel[1],
			.tte = args.tte, .tts = args.tts, .ttd = args.ttd, 
			.left = &forks[i], .right = &forks[(i + 1) % args.count],
			.tsle = inta_init(0), .eaten = inta_init(0),
			.prev_action = E_DIE,
			.time = time,
		};
		i++;
	}
	return (&philos[0]);
}
