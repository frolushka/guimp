/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnequ.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbednar <sbednar@student.fr.42>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/25 23:39:22 by sbednar           #+#    #+#             */
/*   Updated: 2018/11/27 23:51:04 by sbednar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int		ft_strnequ(char const *s1, char const *s2, size_t n)
{
	size_t	i;

	i = 0;
	if (!s1 && !s2)
		return (1);
	else if (!s1 || !s2)
		return (0);
	while (s1[i] && s2[i] && i < n && s1[i] == s2[i])
		++i;
	return (((s1[i] == '\0' && s2[i] == '\0') || (i == n)) ? 1 : 0);
}