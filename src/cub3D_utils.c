/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 19:49:25 by victor            #+#    #+#             */
/*   Updated: 2025/04/16 20:38:35 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

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
/*   Initializes the t_app structure, setting default values and processing   */
/*   the map lines to extract game and camera data.                           */
/*                                                                            */
/* ************************************************************************** */
int	init_app_struct(t_app *app, char **lines, int line_count)
{
	ft_memset(app, 0, sizeof(*app));
	ft_memset(app->game.floor_color, -1, sizeof(app->game.floor_color));
	ft_memset(app->game.ceiling_color, -1, sizeof(app->game.ceiling_color));
	process_lines(lines, line_count, &app->game, &app->camera);
	return (0);
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

	if (app->game.tex_no)
		mlx_delete_texture(app->game.tex_no);
	if (app->game.tex_so)
		mlx_delete_texture(app->game.tex_so);
	if (app->game.tex_we)
		mlx_delete_texture(app->game.tex_we);
	if (app->game.tex_ea)
		mlx_delete_texture(app->game.tex_ea);
	free(app->game.texture_no);
	free(app->game.texture_so);
	free(app->game.texture_we);
	free(app->game.texture_ea);
	i = 0;
	while (i < app->game.map_height)
	{
		free(app->game.map[i]);
		i = i + 1;
	}
	ft_free(app->game.map);
	mlx_terminate(app->mlx);
}
