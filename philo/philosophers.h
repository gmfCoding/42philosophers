/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clovell <clovell@student.42adel.org.au>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 14:57:45 by clovell           #+#    #+#             */
/*   Updated: 2023/09/21 16:13:49 by clovell          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <stddef.h>
# include <pthread.h>
# include <stdint.h>
# include <stdbool.h>

/* Unique End Fork, 
 * When not 0 it will create a new fork at the end of the table,
 * instead of reusing the first fork.
 * This will allow a simulation with a single philosopher, to,
 * be correctly simulated without dying, for debugging perposes.
 
 	It will also have the effect of removing "thinking ladders",
	caused by odd amount of philsophers.
 */
# ifndef PH_UEF
#  define PH_UEF 0
# endif

/* Simulation time offset (us), ideally this value would be:
 		 cycle_time - (tte + tts + ttt)
	But calculating or even approximating it is really difficult.
	Due to the unpredictable nature of ttt, (time to think).
*/
# ifndef PH_STO
#  define PH_STO 600
# endif

/* atomic integer struct */
typedef struct s_int64_atomic	t_inta64;

struct s_int64_atomic
{
	pthread_mutex_t	thread;
	int64_t			value;
};

typedef pthread_mutex_t			t_fork;
typedef struct s_philo			t_philo;
typedef struct s_args			t_args;
typedef enum e_action			t_action;

typedef struct s_waitdeathctx	t_waitdeathctx;
typedef enum e_eachres			t_eachres;

typedef t_eachres				(*t_onphilo_fn)(t_philo *, int i, void *ctx);

enum	e_eachres
{
	E_CANCEL,
	E_CONTINUE,
};

struct	s_waitdeathctx
{
	int16_t	min_eat;
};

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
	pthread_t	thread;
	t_inta64	*wait;
	t_inta64	*cancel;
	t_action	prev_action;

	t_fork		*left;
	t_fork		*right;

	int64_t		id;
	t_inta64	dead;
	t_inta64	tsle;
	t_inta64	eaten;
	int64_t		time;
	bool		print;

	int64_t		tte;
	int64_t		tts;
	int64_t		ttd;
	int64_t		rte;
	int16_t		maxeat;
};

/* init.c */
/* Create a new philosophers */
t_philo		create_philo(int i, t_inta64 *mcancel, t_fork *forks, t_args args);

/* Reads input arguments and stores them in t_args.
 * RETURN VALUES:
 *	true if arguments are invalid (exit the program)
 */
bool		initialise(int32_t argc, char **argv, t_args *args);

/* Generates a new list of intialised philosophers */
t_philo		*construct(t_args args);

/* state.c */

/* Prints the state of a philosophers
 * In the format of: timestamp id action
 * Auto prevents printing the same action twice or more in a row.
 */
void		change_state(t_philo *philo, t_action action);
void		cycle(t_philo *philo);
void		*routine(void *ptr);

/* time.c */
int64_t		gettime_now(void);
int			ft_usleep(size_t milliseconds);

/* utils.c */
/* Converts a string to an integer */
int32_t		ft_atoi(char *str);

/* Runs the function `f` for size amount of philos located at start */
int			foreach_philo(
				t_philo *start,
				size_t size,
				void *ctx,
				t_onphilo_fn f);

/* Free all forks, mutexes and philosophers */
void		cleanup(t_philo *philos);

/* Manages philosophrse starvation and eating count
 * Returns when all threads have closed.
 */
void		wait_death(t_philo *philos, t_args args);

/* measure.c */
/* Runs a single simulation of a single philo (with two forks) 
 * to find the runtime accumulation overhead */
int64_t		getrunexectime(void);

/* atomic.c */
t_inta64	inta_init(int64_t val);
t_inta64	*inta_add(t_inta64 *a, int64_t v);
t_inta64	*inta_set(t_inta64 *a, int64_t v);
int64_t		inta_get(t_inta64 *a);

#endif
