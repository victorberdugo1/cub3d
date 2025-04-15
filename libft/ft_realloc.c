/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 16:05:25 by victor            #+#    #+#             */
/*   Updated: 2025/03/28 12:42:15 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_block_size(void *ptr)
{
	t_block	*block;

	if (!ptr)
		return (0);
	block = ((t_block *)ptr) - 1;
	return (block->size);
}

void	*ft_malloc(size_t size)
{
	t_block	*block;

	block = malloc(sizeof(t_block) + size);
	if (!block)
		return (NULL);
	block->size = size;
	return ((void *)(block + 1));
}

void	ft_free(void *ptr)
{
	if (ptr)
		free(((t_block *)ptr) - 1);
}

void	*ft_realloc(void *ptr, size_t new_size)
{
	t_block	*new_block;
	size_t	old_size;
	size_t	copy_size;

	if (new_size == 0)
	{
		ft_free(ptr);
		return (NULL);
	}
	if (ptr == NULL)
		return (ft_malloc(new_size));
	old_size = ft_block_size(ptr);
	new_block = malloc(sizeof(t_block) + new_size);
	if (!new_block)
		return (NULL);
	new_block->size = new_size;
	if (old_size < new_size)
		copy_size = old_size;
	else
		copy_size = new_size;
	ft_memcpy(new_block + 1, ptr, copy_size);
	ft_free(ptr);
	return ((void *)(new_block + 1));
}
