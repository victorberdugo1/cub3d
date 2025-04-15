/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/29 23:54:43 by victor            #+#    #+#             */
/*   Updated: 2024/08/24 13:53:44 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "libft.h"

int	ft_putstr_fd(char *s, int fd)
{
	if (s == NULL)
		s = "(null)";
	if (s)
		write(fd, s, ft_strlen(s));
	return (ft_strlen(s));
}
