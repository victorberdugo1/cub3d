/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtok.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 11:54:30 by victor            #+#    #+#             */
/*   Updated: 2024/11/26 18:47:19 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtok(char *str, const char *delim)
{
	static char	*last;
	char		*start;
	char		*end;

	if (str)
		last = str;
	if (!last)
		return (NULL);
	start = last;
	while (*start && ft_strchr(delim, *start))
		start++;
	if (*start == '\0')
		return (last = NULL, NULL);
	end = start;
	while (*end && !ft_strchr(delim, *end))
		end++;
	if (*end)
	{
		*end = '\0';
		last = end + 1;
	}
	else
		last = NULL;
	return (start);
}
