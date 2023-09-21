/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   state.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clovell <clovell@student.42adel.org.au>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:56:31 by clovell           #+#    #+#             */
/*   Updated: 2023/09/21 16:15:16 by clovell          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"
#include <stdio.h>
#include <unistd.h>

const char	*get_statemsg(t_action action)
{
	const char *const	msg[] = {
	[E_DIE] = "died",
	[E_GRAB] = "has taken a fork",
	[E_SLEEP] = "is sleeping",
	[E_EAT] = "is eating",
	[E_THINK] = "is thinking",
	};

	return (msg[action]);
}

void	change_state(t_philo *ph, t_action action)
{
	static t_inta64		has_died = {PTHREAD_MUTEX_INITIALIZER, 0};
	const char *const	msg = get_statemsg(action);
	const int64_t		timestamp = (gettime_now() - ph->time) / 1000;
	char				*clear;

	clear = "                                                          ";
	if (ph->prev_action == action)
		return ;
	ph->prev_action = action;
	if (inta_get(&has_died) == 1 || inta_get(ph->cancel) != 0)
	{
		inta_set(&ph->dead, true);
		return ;
	}
	if (ph->print)
		printf("%lld %lld %s\n", timestamp, ph->id, msg);
	else
		printf("%lld %lld %s \r%s\r", timestamp, ph->id, msg, clear);
	if (action == E_DIE)
	{
		inta_set(&has_died, 1);
		if (inta_get(&ph->dead) == true)
			return ;
	}
}

void	cycle(t_philo *philo)
{
	change_state(philo, E_THINK);
	pthread_mutex_lock(philo->left);
	change_state(philo, E_GRAB);
	pthread_mutex_lock(philo->right);
	change_state(philo, E_GRAB);
	inta_set(&philo->tsle, gettime_now());
	change_state(philo, E_EAT);
	ft_usleep(philo->tte - PH_STO);
	inta_add(&philo->eaten, 1);
	pthread_mutex_unlock(philo->right);
	pthread_mutex_unlock(philo->left);
	change_state(philo, E_SLEEP);
	ft_usleep(philo->tts);
}

void	*routine(void *ptr)
{
	t_philo *const	philo = ptr;

	while (inta_get(philo->wait) == 1)
		usleep(50);
	if (philo->id % 2 != (PH_UEF != 0))
		change_state(philo, E_THINK);
	if (philo->id % 2 != (PH_UEF != 0))
		ft_usleep(philo->tte - 1000);
	while (philo->left != philo->right && inta_get(philo->cancel) == 0)
		cycle(philo);
	return (NULL);
}
