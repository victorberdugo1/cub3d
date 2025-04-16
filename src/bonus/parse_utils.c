/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 22:51:37 by victor            #+#    #+#             */
/*   Updated: 2025/04/16 11:03:03 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Skips leading spaces in a string and returns the first non-space char.   */
/*                                                                            */
/* ************************************************************************** */
char	*skip_spaces(char *s)
{
	while (*s == ' ')
		s++;
	return (s);
}

/* ************************************************************************** */
/*                                                                            */
/*   Processes a line read from the file and stores it in a dynamic array.    */
/*                                                                            */
/*   - If `is_newline` is true, it removes the trailing newline character.    */
/*   - Reallocates memory to store the new line in the `temp` array.          */
/*   - Duplicates and stores the line, updating the count of stored lines.    */
/*   - If `is_newline`, resets `*line` to an empty string for the next input. */
/*   - Returns 0 on success, -1 on memory allocation failure.                 */
/*                                                                            */
/* ************************************************************************** */
static int	process_line(char **line, char ***temp, int *count, int is_newline)
{
	size_t	len;

	if (is_newline)
	{
		len = ft_strlen(*line);
		if (len > 0)
			(*line)[len - 1] = '\0';
	}
	*temp = ft_realloc(*temp, ((*count) + 1) * sizeof(char *));
	if (!*temp)
		return (-1);
	(*temp)[*count] = ft_strdup(*line);
	if (!(*temp)[*count])
		return (free(*line), -1);
	(*count)++;
	if (is_newline)
	{
		free(*line);
		*line = ft_strdup("");
		if (!*line)
			return (-1);
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Reads a file character by character, dynamically storing each line.      */
/*                                                                            */
/*   - Reads one character at a time and appends it to the current `line`.    */
/*   - If a newline is encountered, calls `process_line()` to store it.       */
/*   - Stops reading when EOF is reached or on error.                         */
/*   - Returns 0 on success, -1 on memory allocation failure.                 */
/*                                                                            */
/* ************************************************************************** */
static int	read_loop(int fd, char ***temp, int *count, char **line)
{
	int		r;
	char	buf[2];
	char	*new_line;

	r = read(fd, buf, 1);
	while (r > 0)
	{
		buf[1] = '\0';
		new_line = ft_strjoin(*line, buf);
		if (!new_line)
			return (-1);
		free(*line);
		*line = new_line;
		if (buf[0] == '\n')
		{
			if (process_line(line, temp, count, 1) == -1)
				return (-1);
		}
		r = read(fd, buf, 1);
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Reads all lines from a file and stores them in a dynamically allocated   */
/*   array.                                                                   */
/*                                                                            */
/*   - Opens the file and initializes variables.                              */
/*   - Uses `read_loop()` to read and store lines dynamically.                */
/*   - Ensures the last line is processed if it doesn't end in '\n'.          */
/*   - Closes the file and updates `lines` and `line_count`.                  */
/*   - Returns 0 on success, -1 on failure.                                   */
/*                                                                            */
/* ************************************************************************** */
int	read_lines(const char *filename, char ***lines, int *line_count)
{
	int		fd;
	int		count;
	char	**temp;
	char	*line;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (-1);
	count = 0;
	temp = NULL;
	line = ft_strdup("");
	if (!line)
		return (close(fd), -1);
	if (read_loop(fd, &temp, &count, &line) == -1)
		return (close(fd), -1);
	if (ft_strlen(line) > 0)
		if (process_line(&line, &temp, &count, 0) == -1)
			return (close(fd), -1);
	free(line);
	close(fd);
	*lines = temp;
	*line_count = count;
	return (0);
}
