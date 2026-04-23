/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc_simple.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yolim <yolim@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 16:21:06 by yolim             #+#    #+#             */
/*   Updated: 2026/04/23 11:52:14 by yolim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vbc.h"

void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

int ft_add(char **s) {
    int result;
    int j;

    result = ft_multiply(s);
    if (result == -1)
        return (-1);
    while (**s == '+') {
        (*s)++;
        j = ft_multiply(s);
        if (j == -1)
            return (-1);
        result += j;
    }
    return (result);
}

int ft_multiply(char **s) {
    int result;
    int j;

    result = ft_factor(s);
    if (result == -1)
        return (-1);
    while (**s == '*') {
        (*s)++;
        j = ft_factor(s);
        if (j == -1)
            return (-1);
        result *= j;
    }
    return (result);
}

int ft_factor(char **s) {
    int result;

    if (isdigit(**s)) {
        result = **s - '0';
        (*s)++;
        return (result);
    }
    if (**s == '(') {
        (*s)++;
        result = ft_add(s);
        if (result == -1)
            return (-1);
        if (**s != ')') {
            unexpected(**s);
            return (-1);
        }
        (*s)++;
        return (result);
    }
    unexpected(**s);
    return (-1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
    char *s = argv[1];
    int result = ft_add(&s);
    if (result == -1)
        return (1);
    if (*s != '\0') {
        unexpected(*s);
        return (1);
    }
    printf("%d\n", result);
    return (0);
}

/*
Key Concept for VBC :
ft_add      → lowest priority, parsed first
ft_multiply → higher priority, parsed second
ft_factor   → highest priority, parsed last

1. Operator precedence via call chain

  ft_add → ft_multiply → ft_factor
  - * binds tighter than + because ft_multiply is called deeper
  - Each function calls the one below it before doing its own job

  ---
2. The three functions — one job each

  ┌─────────────┬─────────────┬─────────────────────┐
  │  Function   │   Handles   │        Calls        │
  ├─────────────┼─────────────┼─────────────────────┤
  │ ft_add      │ +           │ ft_multiply         │
  ├─────────────┼─────────────┼─────────────────────┤
  │ ft_multiply │ *           │ ft_factor           │
  ├─────────────┼─────────────┼─────────────────────┤
  │ ft_factor   │ digit or () │ ft_add (only for () │
  └─────────────┴─────────────┴─────────────────────┘

  ---
3. ft_factor — the base case

  digit → convert char to int: '5' - '0' = 5
  '('   → call ft_add to parse inside, then expect ')'
  other → unexpected error

  ---
4. Error handling — use -1 as sentinel

  - -1 is safe because all digits are 0-9 and only + * used → result always >= 0
  - When error found: call unexpected(), return -1
  - Always check if (r == -1) return (-1) before using the result

  ---
5. Two error checks in main

  if (result == -1)  // error happened inside parsing
  if (*s != '\0')    // leftover characters e.g. "1+2)"

  ---
6. char **s — pointer to pointer

  s    → points to the pointer
  *s   → the pointer (current position in string)
  **s  → current character
  (*s)++ → advance to next character

  ---
7. Unexpected end vs unexpected token

  void unexpected(char c)
  {
      if (c)      // c != '\0' → it's a real character
          printf("Unexpected token '%c'\n", c);
      else        // c == '\0' → end of string
          printf("Unexpected end of input\n");
  }
 */