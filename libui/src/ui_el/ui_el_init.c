/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_el_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbednar <sbednar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/28 13:41:42 by edraugr-          #+#    #+#             */
/*   Updated: 2019/04/24 14:10:26 by sbednar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libui.h"

void	ui_el_init(t_ui_el *el)
{
	static int	elementID = 0;

	ft_bzero(el, sizeof(t_ui_el));
	el->id = ++elementID;
}
