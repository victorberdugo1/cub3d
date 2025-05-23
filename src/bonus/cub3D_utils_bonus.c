/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D_utils_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 19:49:25 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 12:04:31 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Closes the MLX window.                                                   */
/*                                                                            */
/* ************************************************************************** */
void	close_window(void *param)
{
	t_app	*app;

	app = (t_app *)param;
	mlx_close_window(app->mlx);
}

/* ************************************************************************** */
/*                                                                            */
/*   Loads the map from a file and stores its lines in a dynamically          */
/*   allocated array. Returns -1 on failure, 0 on success.                    */
/*                                                                            */
/* ************************************************************************** */
int	load_map(char *filename, char ***lines, int *line_count)
{
	int	ret;

	ret = read_lines(filename, lines, line_count);
	if (ret == -1)
	{
		printf("Error: Failed to load map %s\n", filename);
		return (-1);
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Frees the memory allocated for storing the map lines.                    */
/*                                                                            */
/* ************************************************************************** */
void	free_map_lines(char **lines, int line_count)
{
	int	i;

	i = 0;
	while (i < line_count)
	{
		free(lines[i]);
		i = i + 1;
	}
	ft_free(lines);
}

/* ************************************************************************** */
/*                                                                            */
/*   Frees all dynamically allocated resources, including textures,           */
/*   map data, and MLX-related objects.                                       */
/*                                                                            */
/* ************************************************************************** */
void	cleanup(t_app *app)
{
	int	i;

	safe_free(app);
	i = 0;
	while (i < app->game.map_height)
	{
		free(app->game.map[i]);
		i++;
	}
	ft_free(app->game.map);
	mlx_terminate(app->mlx);
}
