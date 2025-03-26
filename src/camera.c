/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:27:02 by victor            #+#    #+#             */
/*   Updated: 2025/03/26 20:38:32 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

static void	check_escape(t_app *app)
{
	if (mlx_is_key_down(app->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(app->mlx);
}

static void	update_camera_movement(t_app *app)
{
	double	new_x;
	double	new_y;
	double	speed;

	new_x = app->camera.pos.x;
	new_y = app->camera.pos.y;
	speed = app->camera.move_speed;
	new_x += (mlx_is_key_down(app->mlx, MLX_KEY_W)
			- mlx_is_key_down(app->mlx, MLX_KEY_S))
		* app->camera.dir.x * speed;
	new_y += (mlx_is_key_down(app->mlx, MLX_KEY_W)
			- mlx_is_key_down(app->mlx, MLX_KEY_S))
		* app->camera.dir.y * speed;
	new_x += (mlx_is_key_down(app->mlx, MLX_KEY_A)
			- mlx_is_key_down(app->mlx, MLX_KEY_D))
		* app->camera.dir.y * speed;
	new_y += (mlx_is_key_down(app->mlx, MLX_KEY_D)
			- mlx_is_key_down(app->mlx, MLX_KEY_A))
		* app->camera.dir.x * speed;
	if (!collides(&app->game, new_x, new_y))
	{
		app->camera.pos.x = new_x;
		app->camera.pos.y = new_y;
	}
}

static void	rotate_camera(t_app *app, double rotation)
{
	double	old_dir_x;
	double	old_plane_x;

	old_dir_x = app->camera.dir.x;
	app->camera.dir.x = app->camera.dir.x * cos(rotation)
		- app->camera.dir.y * sin(rotation);
	app->camera.dir.y = old_dir_x * sin(rotation)
		+ app->camera.dir.y * cos(rotation);
	old_plane_x = app->camera.plane.x;
	app->camera.plane.x = app->camera.plane.x * cos(rotation)
		- app->camera.plane.y * sin(rotation);
	app->camera.plane.y = old_plane_x * sin(rotation)
		+ app->camera.plane.y * cos(rotation);
}

static void	update_camera_rotation(t_app *app)
{
	double	rotation;

	if (mlx_is_key_down(app->mlx, MLX_KEY_LEFT))
	{
		rotation = -app->camera.rot_speed;
		rotate_camera(app, rotation);
	}
	else if (mlx_is_key_down(app->mlx, MLX_KEY_RIGHT))
	{
		rotation = app->camera.rot_speed;
		rotate_camera(app, rotation);
	}
}

void	move_camera(void *param)
{
	t_app	*app;

	app = (t_app *)param;
	check_escape(app);
	update_camera_movement(app);
	update_camera_rotation(app);
}
