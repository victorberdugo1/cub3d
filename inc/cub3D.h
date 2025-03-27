/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 11:48:08 by vberdugo          #+#    #+#             */
/*   Updated: 2025/03/26 20:09:04 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include <math.h>
# include "../libft/libft.h"
# include "../MLX42/include/MLX42/MLX42.h"

# define WIDTH 1024
# define HEIGHT 1024
# define COLLISION_RADIUS 0.2

typedef struct s_vec2
{
	double	x;
	double	y;
}	t_vec2;

typedef struct s_camera
{
	t_vec2	pos;
	t_vec2	dir;
	t_vec2	plane;
	double	move_speed;
	double	rot_speed;
}	t_camera;

typedef struct s_game
{
	char			*texture_no;
	char			*texture_so;
	char			*texture_we;
	char			*texture_ea;
	int				floor_color[3];
	int				ceiling_color[3];
	char			**map;
	int				map_height;
	mlx_texture_t	*tex_no;
	mlx_texture_t	*tex_so;
	mlx_texture_t	*tex_we;
	mlx_texture_t	*tex_ea;
}	t_game;

typedef struct s_app
{
	mlx_t		*mlx;
	mlx_image_t	*image;
	t_camera	camera;
	t_game		game;
}	t_app;

typedef struct s_collision
{
	double	r;
	int		min_i;
	int		max_i;
	int		min_j;
	int		max_j;
	int		i;
	int		j;
	double	closest_x;
	double	closest_y;
	double	dx;
	double	dy;
}	t_collision;

void	process_lines(char **lines, int count, t_game *game, t_camera *camera);
int		read_lines(const char *filename, char ***lines, int *line_count);
char	safe_get_tile(t_game *game, int x, int y);
int		collides(t_game *game, double new_x, double new_y);
void	render_scene(void *param);
void	move_camera(void *param);
void	close_window(void *param);
int		load_map(char *filename, char ***lines, int *line_count);
void	free_map_lines(char **lines, int line_count);
int		init_app_struct(t_app *app, char **lines, int line_count);
void	cleanup(t_app *app);

#endif
