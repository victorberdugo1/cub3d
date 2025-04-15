/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 11:43:00 by vberdugo          #+#    #+#             */
/*   Updated: 2024/07/07 01:24:12 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	dst_len;
	size_t	src_len;
	size_t	total_len;
	size_t	i;

	dst_len = ft_strlen(dst);
	src_len = ft_strlen(src);
	total_len = dst_len + src_len;
	if (size <= dst_len)
		return (size + src_len);
	i = dst_len;
	while (*src && i < size - 1)
	{
		dst[i++] = *src;
		src++;
	}
	dst[i] = '\0';
	return (total_len);
}
