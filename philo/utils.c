/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clovell <clovell@student.42adel.org.au>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:35:35 by clovell           #+#    #+#             */
/*   Updated: 2023/09/20 23:25:00 by clovell          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdlib.h>
#include "philosophers.h"

static bool	ft_isspace(char c)
{
	bool	istab;

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

int	foreach_philo(t_philo *start, size_t size, void *ctx, t_onphilo_fn f)
{
	size_t	i;

	i = 0;
	while (i < size)
	{
		if (f(&start[i], i, ctx) == E_CANCEL)
			return (i);
		i++;
	}
	return (i);
}

void	cleanup(t_philo *philos)
{
	free(philos[0].cancel);
	free(philos[0].left);
	free(philos);
}
