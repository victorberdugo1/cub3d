/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aescande <aescande@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 22:25:21 by aescande          #+#    #+#             */
/*   Updated: 2025/04/26 12:51:19 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Handles crouching when holding the left control key.                     */
/*                                                                            */
/*   - Raises the camera when starting to crouch.                             */
/*   - Lowers the camera back when releasing the key.                         */
/*   - Halves the movement speed while crouching.                             */
/*                                                                            */
/* ************************************************************************** */
void	get_down(t_app *app, double *speed)
{
	static int	is_down = 0;

	if (mlx_is_key_down(app->mlx, MLX_KEY_LEFT_CONTROL) && !is_down)
	{
		app->cam.view_z += 50;
		is_down = 1;
	}
	else if (!mlx_is_key_down(app->mlx, MLX_KEY_LEFT_CONTROL) && is_down)
	{
		app->cam.view_z -= 50;
		is_down = 0;
	}
	if (is_down)
		*speed /= 2;
}

/* ************************************************************************** */
/*                                                                            */
/*   Handles jumping when pressing the spacebar.                              */
/*                                                                            */
/*   Here the formula y = y0 + v0*t + 1/2*g*t² is being used.                 */
/*   As we want to subtract y to view_z when going up and then                */
/*   suming up again, we get the right doing -v/fabs(v) = -+1.                */
/*                                                                            */
/*   - Negative initial velocity to move upward, positive to fall down.       */
/*   - Stops the jump when the peak height is reached.                        */
/*                                                                            */
/* ************************************************************************** */
void	jump(t_app *app)
{
	static t_vec2	j = {0, 0};
	static int		is_jumping = 0;
	double			dt;
	double			v;

	if (mlx_is_key_down(app->mlx, MLX_KEY_SPACE) || is_jumping)
	{
		if (!is_jumping)
		{
			is_jumping = 1;
			j = (t_vec2){mlx_get_time(), -50};
		}
		dt = mlx_get_time() - j.x;
		v = j.y + 200 * dt;
		if (!(fabs(app->cam.view_z) > HEIGHT && app->cam.view_z * v > 0))
		{
			if (v >= 0)
				app->cam.view_z -= j.y * dt + 100 * pow(dt, 2);
			else
				app->cam.view_z += j.y * dt + 100 * pow(dt, 2);
		}
		if (fabs(j.y) <= fabs(v))
			is_jumping = 0;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Applies fog effect based on distance.                                    */
/*                                                                            */
/*   - Darkens the given color according to the distance and fog density.     */
/*   - Ensures the fog factor does not fall below a minimum threshold.        */
/*                                                                            */
/* ************************************************************************** */
uint32_t	fog(const int *color, double r, double fog_dens, double min_factor)
{
	double	fog_factor;
	int		dr;
	int		dg;
	int		db;

	fog_factor = 1.0 / (1.0 + fog_dens * r);
	if (fog_factor < min_factor)
		fog_factor = min_factor;
	dr = (int)(color[0] * fog_factor);
	dg = (int)(color[1] * fog_factor);
	db = (int)(color[2] * fog_factor);
	return (ft_pixel(dr, dg, db, 255));
}
