/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:27:02 by victor            #+#    #+#             */
/*   Updated: 2025/04/16 10:55:40 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

/* ************************************************************************** */
/*                                                                            */
/*   Updates the camera's position based on user input.                       */
/*                                                                            */
/*   - Moves forward (W) or backward (S) along the camera's direction vector. */
/*   - Moves left (A) or right (D) perpendicular to the direction vector.     */
/*   - Prevents movement if a collision is detected.                          */
/*                                                                            */
/*   The movement logic works as follows:                                     */
/*   - The direction vector (dir.x, dir.y) represents where the camera is     */
/*     facing.                                                                */
/*   - Moving forward (W) means adding this vector scaled by speed.           */
/*   - Moving backward (S) means subtracting this vector scaled by speed.     */
/*   - Moving sideways (A/D) requires a perpendicular movement:               */
/*     - Left (A) is along the negative perpendicular vector (-dir.y, dir.x)  */
/*     - Right (D) is along the positive perpendicular vector (dir.y, -dir.x) */
/*                                                                            */
/* ************************************************************************** */
static void	wrap_around_position(double *x, double *y, t_app *app)
{
	const int	in_bounds = (*x >= 0 && *x < app->game.map_width
			&& *y >= 0 && *y < app->game.map_height);

	if (!in_bounds)
	{
		*x = fmod(*x, app->game.map_width);
		*y = fmod(*y, app->game.map_height);
		if (*x < 0)
			*x += app->game.map_width;
		if (*y < 0)
			*y += app->game.map_height;
	}
}

static void	update_camera_movement(t_app *app, double delta_time)
{
	double	new_x;
	double	new_y;
	double	speed;

	new_x = app->camera.pos.x;
	new_y = app->camera.pos.y;
	speed = app->camera.move_speed * delta_time;
	if (mlx_is_key_down(app->mlx, MLX_KEY_LEFT_SHIFT))
		speed *= 2;
	new_x += (mlx_is_key_down(app->mlx, MLX_KEY_W) - mlx_is_key_down
			(app->mlx, MLX_KEY_S)) * app->camera.dir.x * speed;
	new_y += (mlx_is_key_down(app->mlx, MLX_KEY_W) - mlx_is_key_down
			(app->mlx, MLX_KEY_S)) * app->camera.dir.y * speed;
	new_x += (mlx_is_key_down(app->mlx, MLX_KEY_A) - mlx_is_key_down
			(app->mlx, MLX_KEY_D)) * app->camera.dir.y * speed;
	new_y += (mlx_is_key_down(app->mlx, MLX_KEY_D) - mlx_is_key_down
			(app->mlx, MLX_KEY_A)) * app->camera.dir.x * speed;
	wrap_around_position(&new_x, &new_y, app);
	app->camera.pos.x = new_x;
	app->camera.pos.y = new_y;
}

/* ************************************************************************** */
/*                                                                            */
/*   Rotates the camera left or right by a given angle.                       */
/*                                                                            */
/*   - Uses a standard 2D rotation matrix:                                    */
/*       [ cos(theta)  -sin(theta) ]                                          */
/*       [ sin(theta)   cos(theta) ]                                          */
/*   - Applies this rotation to both the direction vector and the camera      */
/*     plane to maintain correct rendering of perspective.                    */
/*                                                                            */
/*   The rotation logic works as follows:                                     */
/*   - The `dir` vector represents where the camera is facing.                */
/*   - The `plane` vector determines how much of the world is visible         */
/*     (used for raycasting projection).                                      */
/*   - Applying the rotation matrix rotates both vectors around the origin.   */
/*                                                                            */
/* ************************************************************************** */
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

/* ************************************************************************** */
/*                                                                            */
/*   Detects if the left or right arrow keys are pressed and rotates the      */
/*   camera accordingly.                                                      */
/*                                                                            */
/*   - Left Arrow (←) rotates counterclockwise (negative rotation).           */
/*   - Right Arrow (→) rotates clockwise (positive rotation).                 */
/*                                                                            */
/* ************************************************************************** */
static void	update_camera_rotation(t_app *app, double delta_time)
{
	double	rotation;

	if (mlx_is_key_down(app->mlx, MLX_KEY_LEFT))
	{
		rotation = -app->camera.rot_speed * delta_time;
		rotate_camera(app, rotation);
	}
	else if (mlx_is_key_down(app->mlx, MLX_KEY_RIGHT))
	{
		rotation = app->camera.rot_speed * delta_time;
		rotate_camera(app, rotation);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Handles camera movement and rotation each frame.                         */
/*                                                                            */
/* ************************************************************************** */
void	move_camera(void *param)
{
	static double	last_time = 0;
	double			current_time;
	double			delta_time;
	t_app			*app;

	app = (t_app *)param;
	current_time = mlx_get_time();
	delta_time = current_time - last_time;
	last_time = current_time;
	if (mlx_is_key_down(app->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(app->mlx);
	update_camera_movement(app, delta_time);
	update_camera_rotation(app, delta_time);
}
