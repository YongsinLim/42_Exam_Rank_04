/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yolim <yolim@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 18:51:11 by yolim             #+#    #+#             */
/*   Updated: 2026/04/22 17:27:47 by yolim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VBC_H
#define VBC_H

#include <stdlib.h> // for malloc, calloc, realloc, free
#include <stdio.h> // for printf
#include <ctype.h> // for isdigit
#include <unistd.h> // for write

void    unexpected(char c);
int     ft_add(char **s);
int     ft_multiply(char **s);
int     ft_factor(char **s);

#endif