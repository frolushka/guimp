/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_el_begin_drag.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbednar <sbednar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/22 05:47:59 by sbednar           #+#    #+#             */
/*   Updated: 2019/04/22 07:32:14 by sbednar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libui.h"

void	ui_el_begin_drag(void *a1, void *a2)
{
	t_ui_main	*m;
	t_ui_el		*el;

	m = (t_ui_main *)a1;
	el = (t_ui_el *)a2;
	el->params |= EL_ON_DRAG;
}