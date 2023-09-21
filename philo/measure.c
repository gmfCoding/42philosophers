#include "philosophers.h"

int64_t	getrunexectime(void)
{
	t_inta64		mcancel[2];
	t_philo			philo;
	t_fork			forks[2] = {
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER
	};
	t_args	args;
	int64_t			exec;

	args = (t_args){2, 400, 100 * 1000, 100 * 1000, 0};
	mcancel[0] = inta_init(0);
	mcancel[1] = inta_init(1);
	philo = create_philo(0, mcancel, forks, args);
	philo.print = false;
	philo.time = gettime_now();
	inta_set(&philo.tsle, philo.time);
	exec = gettime_now();
	while (args.cycles < 30)
	{
		cycle(&philo);
		args.cycles++;
	}	
	exec = (gettime_now() - exec) - 30 * 200000;//- (args.tte + args.tts);
	return (exec / 30);
}
