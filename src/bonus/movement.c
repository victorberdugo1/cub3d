/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aescande <aescande@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 22:25:21 by aescande          #+#    #+#             */
/*   Updated: 2025/04/16 23:54:19 by aescande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

void	get_down(t_app *app, double *speed)
{
	static int	is_down = 0;

	if (mlx_is_key_down(app->mlx, MLX_KEY_LEFT_CONTROL) && !is_down)
	{
		app->camera.view_z += 50;
		is_down = 1;
	}
	else if (!mlx_is_key_down(app->mlx, MLX_KEY_LEFT_CONTROL) && is_down)
	{
		app->camera.view_z -= 50;
		is_down = 0;
	}
	if (is_down)
		*speed /= 2;
}

/*
 *
 * Here the formula y = y0 + v0*t + 1/2*g*t² is being used.
 * As we want to subtract y to view_z when going up and then
 * suming up again, we get the right doing -v/fabs(v) = -+1.
 *
 */

void	jump(t_app *app)
{
	double			v_i;
	double			v;
	static double	t_i;
	double			dt;
	static int		is_jumping = 0;

	if (mlx_is_key_down(app->mlx, MLX_KEY_SPACE) || is_jumping)
	{
		v_i = -50;
		if (!is_jumping)
		{
			t_i = mlx_get_time();
			is_jumping = 1;
		}
		dt = mlx_get_time() - t_i;
		v = v_i + 200 * dt;
		if (!(fabs(app->camera.view_z) > HEIGHT && app->camera.view_z * v/fabs(v) > 0))
		{
			if (v >= 0)
				app->camera.view_z -= (v_i * dt + 0.5 * 200 * pow(dt, 2));
			else
				app->camera.view_z += (v_i * dt + 0.5 * 200 * pow(dt, 2));
		}
		if(fabs(v_i) <= fabs(v))
			is_jumping = 0;
	}
}
