/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 11:12:29 by vberdugo          #+#    #+#             */
/*   Updated: 2024/07/08 12:36:07 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

static int	count_c(const char *s, char c)
{
	int	count;
	int	in_substr;

	count = 0;
	in_substr = 0;
	while (*s != '\0')
	{
		if (*s == c)
			in_substr = 0;
		else if (!in_substr)
		{
			in_substr = 1;
			count++;
		}
		s++;
	}
	return (count);
}

static int	find_s(const char *s, char c, int *i)
{
	int	start;

	while (s[*i] != '\0' && s[*i] == c)
		(*i)++;
	start = *i;
	while (s[*i] != '\0' && s[*i] != c)
		(*i)++;
	return (start);
}

static void	free_splt(char **splt, int j)
{
	while (j > 0)
	{
		free(splt[j - 1]);
		j--;
	}
	free(splt);
}

char	**ft_split(char const *s, char c)
{
	char	**splt;
	int		i;
	int		j;
	int		start;

	i = 0;
	j = -1;
	if (s == NULL)
		return (NULL);
	splt = (char **)ft_calloc((count_c(s, c) + 1), sizeof(char *));
	if (splt == NULL)
		return (NULL);
	while (++j < count_c(s, c))
	{
		start = find_s(s, c, &i);
		splt[j] = (char *)ft_calloc((i - start + 1), sizeof(char));
		if (splt[j] == NULL)
		{
			free_splt(splt, j);
			return (NULL);
		}
		ft_strlcpy(splt[j], &s[start], i - start + 1);
	}
	return (splt);
}
