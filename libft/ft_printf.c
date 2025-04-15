/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/28 14:27:23 by vberdugo          #+#    #+#             */
/*   Updated: 2024/08/24 13:55:59 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <unistd.h>

int	ft_putunbr_fd(unsigned int n, int fd)
{
	char	buffer[10];
	int		i;
	int		count;

	i = 0;
	count = 0;
	if (n == 0)
		return (write(fd, "0", 1));
	while (n > 0)
	{
		buffer[i++] = '0' + (n % 10);
		n /= 10;
	}
	while (i--)
		count += write(fd, &buffer[i], 1);
	return (count);
}

int	ft_putnum_hex(unsigned long num, int uppercase)
{
	char		*hex_digits;
	char		buffer[16];
	int			i;
	int			count;

	if (uppercase)
		hex_digits = "0123456789ABCDEF";
	else
		hex_digits = "0123456789abcdef";
	i = 0;
	count = 0;
	if (num == 0)
		return (write(1, "0", 1), 1);
	while (num != 0)
	{
		buffer[i++] = hex_digits[num % 16];
		num /= 16;
	}
	while (i--)
	{
		write(1, &buffer[i], 1);
		count++;
	}
	return (count);
}

int	ft_print_ptr(void *p)
{
	unsigned long	addr;
	int				count;

	count = 0;
	addr = (unsigned long)p;
	if (addr == 0)
	{
		ft_putstr_fd("(nil)", 1);
		count += 5;
		return (count);
	}
	else
	{
		write(1, "0x", 2);
		count += 2;
		count += ft_putnum_hex(addr, 0);
	}
	return (count);
}

int	handle_format(const char **format, va_list args)
{
	int		count;
	char	*str ;

	count = 0;
	(*format)++;
	if (**format == 'c')
		count += ft_putchar_fd((char)va_arg(args, int), 1);
	else if (**format == 's')
	{
		str = va_arg(args, char *);
		count += ft_putstr_fd(str, 1);
	}
	else if (**format == 'p')
		count += ft_print_ptr(va_arg(args, void *));
	else if (**format == 'd' || **format == 'i' )
		count += ft_putnbr_fd(va_arg(args, int), 1);
	else if (**format == 'u')
		count += ft_putunbr_fd(va_arg(args, unsigned int), 1);
	else if (**format == 'x')
		count += ft_putnum_hex(va_arg(args, unsigned int), 0);
	else if (**format == 'X')
		count += ft_putnum_hex(va_arg(args, unsigned int), 1);
	return (count);
}

int	ft_printf(char const *format, ...)
{
	va_list	args;
	int		count;

	count = 0;
	va_start(args, format);
	while (*format)
	{
		if (*format == '%')
		{
			if (*(format + 1) == '%')
			{
				count += ft_putchar_fd('%', 1);
				format++;
			}
			else
				count += handle_format(&format, args);
		}
		else
			count += ft_putchar_fd(*format, 1);
		format++;
	}
	va_end(args);
	return (count);
}
