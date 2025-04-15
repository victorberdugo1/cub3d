/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 10:30:06 by vberdugo          #+#    #+#             */
/*   Updated: 2024/07/06 23:20:41 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*st;

	st = (unsigned char *)s;
	while (n > 0)
	{
		*st++ = (unsigned char)c;
		n--;
	}
	return (s);
}
