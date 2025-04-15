/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 11:38:28 by vberdugo          #+#    #+#             */
/*   Updated: 2024/06/26 09:55:48 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	little_len;
	size_t	i;
	size_t	j;

	little_len = ft_strlen(little);
	i = 0;
	if (little_len == 0)
		return ((char *)big);
	if (len < little_len)
		return (NULL);
	while (big[i] && i <= len - little_len)
	{
		j = 0;
		while (little[j] && big[i + j] == little[j])
			j++;
		if (little[j] == '\0')
			return ((char *)&big[i]);
		i++;
	}
	return (NULL);
}
