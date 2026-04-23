/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yolim <yolim@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 18:39:10 by yolim             #+#    #+#             */
/*   Updated: 2026/04/22 19:27:44 by yolim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h> // for malloc, calloc, realloc, free
#include <stdio.h> // for printf
#include <ctype.h> // for isdigit
#include <unistd.h> // for write

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}

void    destroy_tree(node *n)
{
    if (!n)
        return ;
    if (n->type != VAL)
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

int accept(char **s, char c)
{
    if (**s == c) // +++++++++++++++
    {
        (*s)++;
        return (1);
    }
    return (0);
}

int expect(char **s, char c)
{
    if (accept(s, c))
        return (1);
    unexpected(**s);
    return (0);
}

//-------------------------------------------------------------------------------

node    *ft_add(char **s);

node    *ft_factor(char **s) {
    node    temp;
    node    *result;

    if (isdigit(**s)) {
        temp.type = VAL;
        temp.val = **s - '0';
        temp.l = NULL;
        temp.r = NULL;
        result = new_node(temp);
        (*s)++;
        return (result);
    }
    if (accept(s, '(')) {
        result = ft_add(s);
        if (!result)
            return (NULL);
        if (!expect(s, ')')) {
            destroy_tree(result);
            return (NULL);
        }
        return (result);
    }
    unexpected(**s);
    return (NULL);
}

node    *ft_multiply(char **s) {
    node    temp;
    node    *left;
    node    *right;

    left = ft_factor(s);
    if (!left)
        return (NULL);
    while (accept(s, '*')) {
        right = ft_factor(s);
        if (!right) {
            destroy_tree(left);
            return (NULL);
        }
        temp.type = MULTI;
        temp.val = 0;
        temp.l = left;
        temp.r = right;
        left = new_node(temp);
        if (!left)
            return (NULL);
    }
    return (left);
}

node    *ft_add(char **s) {
    node    temp;
    node    *left;
    node    *right;

    left = ft_multiply(s);
    if (!left)
        return (NULL);
    while (accept(s, '+')) {
        right = ft_multiply(s);
        if (!right) {
            destroy_tree(left);
            return (NULL);
        }
        temp.type = ADD;
        temp.val = 0;
        temp.l = left;
        temp.r = right;
        left = new_node(temp);
        if (!left)
            return (NULL);
    }
    return (left);
}

int eval_tree(node *tree)
{
    switch (tree->type)
    {
        case ADD:
            return (eval_tree(tree->l) + eval_tree(tree->r));
        case MULTI:
            return (eval_tree(tree->l) * eval_tree(tree->r));
        case VAL:
            return (tree->val);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
    char *s = argv[1];
    node *tree = ft_add(&s);
    if (!tree)
        return (1);
    if (*s != '\0') {
        unexpected(*s);
        destroy_tree(tree);
        return (1);
    }
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
}
