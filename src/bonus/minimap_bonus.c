/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 11:36:39 by victor            #+#    #+#             */
/*   Updated: 2025/04/18 13:04:51 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"


void render_minimap(t_app *app)
{
	for (int i = -MINI_RADIUS; i <= MINI_RADIUS; i++) {
		for (int j = -MINI_RADIUS; j <= MINI_RADIUS; j++) {
			if (i * i + j * j <= MINI_RADIUS * MINI_RADIUS)
				mlx_put_pixel(app->image, MINI_X + j, MINI_Y + i, 0x202020FF);
		}
	}

	double angle = -atan2(app->camera.dir.y, app->camera.dir.x) - M_PI / 2.0;
	double sin_a = sin(angle);
	double cos_a = cos(angle);

	for (int map_y = -MINI_VIEW_DIST; map_y <= MINI_VIEW_DIST; map_y++) {
		for (int map_x = -MINI_VIEW_DIST; map_x <= MINI_VIEW_DIST; map_x++) {
			int actual_x = (int)(app->camera.pos.x) + map_x;
			int actual_y = (int)(app->camera.pos.y) + map_y;

			char tile = safe_get_tile(&app->game, actual_x, actual_y);
			uint32_t color = 0;
			if (tile == '1') {
				color = 0x808080FF;
			}
			else if (tile == '2' || tile == '3') {
				int door_found = 0;
				for (int d = 0; d < app->game.door_count; d++) {
					t_door *door = &app->game.doors[d];
					if (door->x == actual_x && door->y == actual_y) {
						color = (door->is_open) ? 0x00FF00FF : 0xFF8000FF;
						door_found = 1;
						break;
					}
				}
				if (!door_found)
					color = 0xFF8000FF; // fallback si por alguna razón no está en la lista
			}

			if (color != 0) {
				double rel_x = map_x;
				double rel_y = map_y;

				double rot_x = rel_x * cos_a - rel_y * sin_a;
				double rot_y = rel_x * sin_a + rel_y * cos_a;

				int center_x = MINI_X + (int)(rot_x * MINI_SCALE);
				int center_y = MINI_Y + (int)(rot_y * MINI_SCALE);

				for (int dy = -2; dy <= 5; dy++) {
					for (int dx = -2; dx <= 5; dx++) {
						double px = dx;
						double py = dy;

						double r_px = px * cos_a - py * sin_a;
						double r_py = px * sin_a + py * cos_a;

						int fx = center_x + (int)r_px;
						int fy = center_y + (int)r_py;

						if (hypot(fx - MINI_X, fy - MINI_Y) <= MINI_RADIUS)
							mlx_put_pixel(app->image, fx, fy, color);
					}
				}
			}
		}
	}

	for (int i = 0; i < app->game.enemy_count; i++) {
		t_enemy *e = &app->game.enemies[i];
		if (!e->is_active) continue;

		double dx = e->pos_x - app->camera.pos.x;
		double dy = e->pos_y - app->camera.pos.y;

		double rot_x = dx * cos_a - dy * sin_a;
		double rot_y = dx * sin_a + dy * cos_a;

		int center_x = MINI_X + (int)(rot_x * MINI_SCALE);
		int center_y = MINI_Y + (int)(rot_y * MINI_SCALE);

		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				double px = dx;
				double py = dy;

				double r_px = px * cos_a - py * sin_a;
				double r_py = px * sin_a + py * cos_a;

				int fx = center_x + (int)r_px;
				int fy = center_y + (int)r_py;

				if (hypot(fx - MINI_X, fy - MINI_Y) <= MINI_RADIUS)
					mlx_put_pixel(app->image, fx, fy, 0xFF0000FF);
			}
		}
	}

	// Jugador en el centro
	mlx_put_pixel(app->image, MINI_X, MINI_Y, 0x00FF00FF);

	int arrow_len = 10; 
	int head_size = 5;

	// Cuerpo de la flecha (línea principal)
	for (int i = 0; i < arrow_len; i++) {
		int x = MINI_X;
		int y = MINI_Y + i;
		if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
			mlx_put_pixel(app->image, x, y, 0xFFFFFFFF);
		}
	}

	for (int i = 0; i < head_size; i++) {
		int x = MINI_X - i;
		int y = MINI_Y - (arrow_len/3) + i;
		if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
			mlx_put_pixel(app->image, x, y, 0xFFFFFFFF);
		}

		x = MINI_X + i;
		y = MINI_Y - (arrow_len/3) + i ;
		if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
			mlx_put_pixel(app->image, x, y, 0xFFFFFFFF);
		}
	}
}

