/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   state.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clovell <clovell@student.42adel.org.au>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:56:31 by clovell           #+#    #+#             */
/*   Updated: 2023/09/20 22:00:00 by clovell          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"
#include <stdio.h>

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

void	change_state(t_philo *philo, t_action action)
{
	static t_inta64		has_died = {PTHREAD_MUTEX_INITIALIZER, 0};
	const char *const	msg = get_statemsg(action);

	if (philo->prev_action == action)
		return ;
	philo->prev_action = action;
	if (inta_get(&has_died) == 1 || inta_get(philo->cancel) != 0)
	{
		inta_set(&philo->dead, true);
		return ;
	}
	printf("%ld %ld %s\n", (gettime_now() - philo->time) / 1000, philo->id, msg);
	if (action == E_DIE)
	{
		inta_set(&has_died, 1);
		if (inta_get(&philo->dead) == true)
			return ;
	}
}

