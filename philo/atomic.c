/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atomic.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clovell <clovell@student.42adel.org.au>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 22:49:19 by clovell           #+#    #+#             */
/*   Updated: 2023/09/20 22:50:14 by clovell          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

t_inta64	inta_init(int64_t val)
{
	return ((t_inta64){PTHREAD_MUTEX_INITIALIZER, val});
}

t_inta64	*inta_add(t_inta64 *a, int64_t v)
{
	pthread_mutex_lock(&a->thread);
	a->value += v;
	pthread_mutex_unlock(&a->thread);
	return (a);
}

t_inta64	*inta_set(t_inta64 *a, int64_t v)
{
	pthread_mutex_lock(&a->thread);
	a->value = v;
	pthread_mutex_unlock(&a->thread);
	return (a);
}

int64_t	inta_get(t_inta64 *a)
{
	int64_t	value;

	pthread_mutex_lock(&a->thread);
	value = a->value;
	pthread_mutex_unlock(&a->thread);
	return (value);
}
