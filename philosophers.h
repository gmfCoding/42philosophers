/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clovell <clovell@student.42adel.org.au>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 14:57:45 by clovell           #+#    #+#             */
/*   Updated: 2023/09/20 21:58:24 by clovell          ###   ########.fr       */
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
	int32_t	ttd;
	int32_t	tte;
	int32_t	tts;
	int32_t	cycles;
};

struct s_philo
{
	pthread_t		thread;
	pthread_t		observer;
	t_inta64		*wait;
	t_inta64		*cancel;
	t_action		prev_action;

	t_fork	*left;
	t_fork	*right;

	int64_t id;
	t_inta64	dead;
	t_inta64	tsle;
	t_inta64	eaten;
	int64_t time;

	int64_t tte;
	int64_t tts;
	int64_t ttd;
	int16_t	maxeat;
};

/* Generates a new list of intialised philosophers */
t_philo	*construct(t_args args);

void	change_state(t_philo *philo, t_action action);
int64_t gettime_now(void);
int32_t	ft_atoi(char *str);

/* atomic.c */
t_inta64	inta_init(int64_t val);
t_inta64	*inta_add(t_inta64 *a, int64_t v);
t_inta64	*inta_set(t_inta64 *a, int64_t v);
int64_t		inta_get(t_inta64 *a);

#endif
