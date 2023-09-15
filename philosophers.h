/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clovell <clovell@student.42adel.org.au>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 14:57:45 by clovell           #+#    #+#             */
/*   Updated: 2023/09/15 16:47:26 by clovell          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

/* atomic integer struct */
typedef struct s_int64_atomic t_inta64;
struct s_int64_atomic
{
	pthread_mutex_t	thread;
	int64_t			value;
};


typedef pthread_mutex_t t_fork;
typedef struct s_philo	t_philo;
typedef struct s_args	t_args;
typedef enum e_action	t_action;

enum e_action
{
	E_EAT = 0,
	E_SLEEP,
	E_GRAB,
	E_THINK,
	E_DIE,
};

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
	t_inta64		*wait;
	t_inta64		*cancel;

	t_fork	*left;
	t_fork	*right;

	int64_t id;
	bool	dead;
	int64_t	tsle;
	int16_t	eaten;
	int64_t time;

	int64_t eat;
	int64_t sleep;
	int64_t starve;
	int16_t	maxeat;
};


/* atomic.c */
t_inta64	*inta_add(t_inta64 *a, int64_t v);
t_inta64	*inta_set(t_inta64 *a, int64_t v);
int64_t		inta_get(t_inta64 *a);

#endif
