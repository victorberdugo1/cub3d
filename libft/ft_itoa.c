/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/29 12:51:55 by victor            #+#    #+#             */
/*   Updated: 2024/07/08 12:25:03 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

static int	dig_len(int n)
{
	int	length;

	length = 0;
	if (n <= 0)
		length = 1;
	while (n != 0)
	{
		n /= 10;
		length++;
	}
	return (length);
}

char	*ft_itoa(int n)
{
	char	*res;
	int		dig;
	long	nbr;

	nbr = n;
	dig = dig_len(n);
	res = (char *)malloc(sizeof(char) * (dig + 1));
	if (!res)
		return (NULL);
	res[dig] = '\0';
	if (nbr < 0)
	{
		res[0] = '-';
		nbr = -nbr;
	}
	else if (nbr == 0)
		res[0] = '0';
	while (nbr > 0)
	{
		res[--dig] = nbr % 10 + '0';
		nbr /= 10;
	}
	return (res);
}
