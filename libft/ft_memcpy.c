/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 10:30:52 by vberdugo          #+#    #+#             */
/*   Updated: 2024/07/06 23:25:03 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*tmp;
	size_t			i;

	tmp = (unsigned char *)dest;
	i = 0;
	while (i < n)
	{
		tmp[i] = ((unsigned char *)src)[i];
		i++;
	}
	return (dest);
}
