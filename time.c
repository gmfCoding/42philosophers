/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clovell <clovell@student.42adel.org.au>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:56:03 by clovell           #+#    #+#             */
/*   Updated: 2023/09/20 21:58:13 by clovell          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <sys/time.h>
#include <inttypes.h>
#include <stddef.h>

int64_t gettime_now(void)
{
	int64_t us;
	struct timeval time;

	gettimeofday(&time, NULL);
	us = time.tv_sec * 	1000000.0;
	us += time.tv_usec;
	return (us);
}

