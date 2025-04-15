/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 00:25:12 by victor            #+#    #+#             */
/*   Updated: 2024/08/06 12:42:53 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "libft.h"

int	ft_putnbr_fd(int n, int fd)
{
	long	num;
	int		count;

	num = n;
	count = 0;
	if (num < 0)
	{
		count += ft_putchar_fd('-', fd);
		num = -num;
	}
	if (num >= 10)
	{
		count += ft_putnbr_fd((int)(num / 10), fd);
	}
	count += ft_putchar_fd(num % 10 + '0', fd);
	return (count);
}
