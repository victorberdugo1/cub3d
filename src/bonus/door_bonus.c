/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 20:51:23 by victor            #+#    #+#             */
/*   Updated: 2025/04/17 02:12:56 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

void update_door_animation(t_app *app) {
    for (int i = 0; i < app->game.door_count; i++) {
        t_door *door = &app->game.doors[i];
        if (door->move_progress < 50.0) {
            door->move_progress = fmin(door->move_progress + 0.08, 10.0);
            double t = door->move_progress;
            door->open_offset = door->open_offset * (t * t * (3 - 2 * t));
        }
    }
}

void toggle_doors(t_app *app) {
    static int last_space = 0;
    int current_space = mlx_is_key_down(app->mlx, MLX_KEY_SPACE);

    if (current_space && !last_space) {
        t_ray ray;
        init_ray(app, WIDTH/2, &ray);
        do_dda(app, &ray);

        if (ray.perpwalldist < 1.5 && (ray.hit_tile == '2' || ray.hit_tile == '3')) {
            for (int i = 0; i < app->game.door_count; i++) {
                t_door *door = &app->game.doors[i];
                if (door->x == ray.map_x && door->y == ray.map_y) {
					door->is_open = !door->is_open;
					door->open_offset = door->is_open ? 50.0 : -50.0; // Offset completo del tile
					door->move_progress = 0.0; // Reiniciar animación
					break;
                }
            }
        }
    }
    last_space = current_space;
}
