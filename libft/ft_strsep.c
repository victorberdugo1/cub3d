/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strsep.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 16:19:43 by victor            #+#    #+#             */
/*   Updated: 2024/12/11 16:38:34 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strsep(char **stringp, const char *delim, int *in_q, char *q_char)
{
	char	*start;
	char	*p;

	start = *stringp;
	p = *stringp;
	if (!start)
		return (NULL);
	while (*p)
	{
		if ((*p == '"' || *p == '\'') && (*in_q == 0 || *p == *q_char))
		{
			*in_q = !(*in_q);
			if (*in_q)
				*q_char = *p;
			else
				*q_char = '\0';
		}
		else if (!*in_q && ft_strchr(delim, *p))
		{
			return (*p++ = '\0', *stringp = p, start);
		}
		p++;
	}
	return (*stringp = NULL, start);
}
