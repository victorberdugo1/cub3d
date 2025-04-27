/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   textures_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 14:27:08 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 13:29:40 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Frees all loaded MLX textures to avoid memory leaks.                     */
/*                                                                            */
/*   - Checks if each texture exists before calling `mlx_delete_texture()`.   */
/*   - Handles wall textures (NO, SO, WE, EA), door textures, enemy texture,  */
/*     and weapon texture.                                                    */
/*                                                                            */
/* ************************************************************************** */
static void	free_mlx_textures(t_app *app)
{
	if (app->game.tex_no)
		mlx_delete_texture(app->game.tex_no);
	if (app->game.tex_so)
		mlx_delete_texture(app->game.tex_so);
	if (app->game.tex_we)
		mlx_delete_texture(app->game.tex_we);
	if (app->game.tex_ea)
		mlx_delete_texture(app->game.tex_ea);
	if (app->game.tex_door)
		mlx_delete_texture(app->game.tex_door);
	if (app->game.tex_door_w)
		mlx_delete_texture(app->game.tex_door_w);
	if (app->game.tex_enemy)
		mlx_delete_texture(app->game.tex_enemy);
	if (app->game.tex_weapon)
		mlx_delete_texture(app->game.tex_weapon);
}

/* ************************************************************************** */
/*                                                                            */
/*   Safely frees all allocated memory and textures in the game.              */
/*                                                                            */
/*   - Frees loaded MLX textures.                                             */
/*   - Frees texture path strings.                                            */
/*   - Frees dynamic arrays for doors and enemies if they exist.              */
/*                                                                            */
/* ************************************************************************** */
void	safe_free(t_app *app)
{
	free_mlx_textures(app);
	free(app->game.texture_no);
	free(app->game.texture_so);
	free(app->game.texture_we);
	free(app->game.texture_ea);
	free(app->game.texture_door);
	free(app->game.texture_door_w);
	free(app->game.texture_enemy);
	free(app->game.texture_weapon);
	if (app->game.doors)
		free(app->game.doors);
	if (app->game.enemies)
		free(app->game.enemies);
}

/* ************************************************************************** */
/*                                                                            */
/*   Loads main wall textures (North, South, West, East) from PNG files.      */
/*                                                                            */
/*   - Loads using `mlx_load_png()` each path.                                */
/*   - If any texture fails to load, prints error, calls cleanup, returns -1. */
/*                                                                            */
/* ************************************************************************** */
static int	load_main_textures(t_app *app)
{
	t_game	*g;

	g = &app->game;
	g->tex_no = mlx_load_png(g->texture_no);
	g->tex_so = mlx_load_png(g->texture_so);
	g->tex_we = mlx_load_png(g->texture_we);
	g->tex_ea = mlx_load_png(g->texture_ea);
	if (!g->tex_no || !g->tex_so || !g->tex_we || !g->tex_ea)
	{
		printf("Error: Failed to load one or more textures.\n");
		cleanup(app);
		return (-1);
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Loads additional textures (enemies, doors, weapon).                      */
/*                                                                            */
/*   Steps:                                                                   */
/*    - If enemy texture exists, load it. If fails, print error and cleanup.  */
/*    - If door textures exist, load both door types (normal and wall).       */
/*    - Ensure both door textures are loaded if one is present (consistency). */
/*    - If weapon texture exists, load it and set it to the weapon struct.    */
/*                                                                            */
/* ************************************************************************** */
static int	load_extra_textures(t_app *app)
{
	t_game	*g;

	g = &app->game;
	if (g->texture_enemy)
	{
		g->tex_enemy = mlx_load_png(g->texture_enemy);
		if (!g->tex_enemy)
			return (printf("Error: Fail load enemy txt.\n"), cleanup(app), -1);
	}
	if (g->texture_door)
		g->tex_door = mlx_load_png(g->texture_door);
	if (g->texture_door_w)
		g->tex_door_w = mlx_load_png(g->texture_door_w);
	if ((g->tex_door && !g->tex_door_w) || (!g->tex_door && g->tex_door_w))
		return (printf("Error\nBoth door textures\n"), cleanup(app), -1);
	if (g->texture_weapon)
	{
		g->tex_weapon = mlx_load_png(g->texture_weapon);
		if (!g->tex_weapon)
			return (printf("Error\n Weapon txt \n"), cleanup(app), -1);
		app->weapon.texture = g->tex_weapon;
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Loads all textures necessary for the game.                               */
/*                                                                            */
/*   Steps:                                                                   */
/*    - Check if essential texture paths (NO, SO, WE, EA) exist.              */
/*      If missing, print error and terminate MLX immediately.                */
/*    - Load main wall textures.                                              */
/*    - Load extra textures (enemies, doors, weapons).                        */
/*    - If any loading fails, cleanup and return error.                       */
/*                                                                            */
/* ************************************************************************** */
int	load_game_textures(t_app *app)
{
	t_game	*g;

	g = &app->game;
	if (!g->texture_no || !g->texture_so || !g->texture_we || !g->texture_ea)
	{
		printf("Error: Missing texture paths in map file.\n");
		mlx_terminate(app->mlx);
		return (-1);
	}
	if (load_main_textures(app) == -1 || load_extra_textures(app) == -1)
		return (-1);
	return (0);
}
