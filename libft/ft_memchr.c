/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 11:26:52 by vberdugo          #+#    #+#             */
/*   Updated: 2024/07/07 01:40:32 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*z;
	size_t			con;

	z = (unsigned char *)s;
	con = 0;
	while (con < n)
	{
		if (*z == (unsigned char)c)
			return ((void *)z);
		z++;
		con++;
	}
	return (NULL);
}
