/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:27:02 by victor            #+#    #+#             */
/*   Updated: 2025/04/17 23:59:18 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Checks if the Escape key is pressed. If so, it closes the MLX window,    */
/*   effectively terminating the program.                                     */
/*                                                                            */
/* ************************************************************************** */
static void	check_escape(t_app *app)
{
	if (mlx_is_key_down(app->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(app->mlx);
}

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

static void	update_camera_movement(t_app *app, double delta_time)
{
	double		new_x;
	double		new_y;
	double		speed;

	new_x = app->camera.pos.x;
	new_y = app->camera.pos.y;
	speed = app->camera.move_speed * delta_time;
	if (mlx_is_key_down(app->mlx, MLX_KEY_LEFT_SHIFT))
		speed *= 2;
	get_down(app, &speed);
	jump(app);
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
static void	rotate_camera(t_app *app, double alpha)
{
	double	old_dir_x;
	double	old_plane_x;

	old_dir_x = app->camera.dir.x;
	app->camera.dir.x = app->camera.dir.x * cos(alpha)
		- app->camera.dir.y * sin(alpha);
	app->camera.dir.y = old_dir_x * sin(alpha)
		+ app->camera.dir.y * cos(alpha);
	old_plane_x = app->camera.plane.x;
	app->camera.plane.x = app->camera.plane.x * cos(alpha)
		- app->camera.plane.y * sin(alpha);
	app->camera.plane.y = old_plane_x * sin(alpha)
		+ app->camera.plane.y * cos(alpha);
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
	int32_t	posx;
	int32_t	posz;
	double	alpha;
	double	offset_z;

	mlx_get_mouse_pos(app->mlx, &posx, &posz);
	alpha = (posx - WIDTH / 2) * delta_time / 5;
	offset_z = (posz - HEIGHT / 2) * delta_time * 120;
	rotate_camera(app, alpha);
	if (!(fabs(app->camera.view_z) >= HEIGHT &&
			   	app->camera.view_z * offset_z > 0))
		app->camera.view_z += offset_z;
	mlx_set_mouse_pos(app->mlx, WIDTH/2, HEIGHT/2);
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

	t_app	*app;	
	app = (t_app *)param;
	current_time = mlx_get_time();
	delta_time = current_time - last_time;
	last_time = current_time;
	check_escape(app);
	toggle_doors(app);
	update_door_animation(app, delta_time);
    update_enemies(app, delta_time);
	update_camera_movement(app, delta_time);
	update_camera_rotation(app, delta_time);
}
