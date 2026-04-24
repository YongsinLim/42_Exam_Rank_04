/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yolim <yolim@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 19:33:46 by yolim             #+#    #+#             */
/*   Updated: 2026/04/24 19:18:59 by yolim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h> // for getc, ungetc, printf, fscanf
#include <stdlib.h> // for malloc, calloc, realloc, free
#include <ctype.h> // for isdigit
#include <unistd.h> // for write


typedef struct	json {
    enum {
        MAP,
        INTEGER,
        STRING
    } type;
    union {
        struct {
            struct pair	*data;
            size_t		size;
        } map;
        int	integer;
        char	*string;
    };
}	json;

typedef struct	pair {
    char	*key;
    json	value;
}	pair;

int     peek(FILE *stream);
void    unexpected(FILE *stream);
int     accept(FILE *stream, char c);
int     expect(FILE *stream, char c);
int     argo(json *dst, FILE *stream);
// ----------------------------------------------------------------------------------------

// Below are own Implementation :
// fscanf(stream, "%d", &dst->integer) reads a number directly from stream. Returns 1 if successful.
int parse_integer(json *dst, FILE *stream) {
    dst->type = INTEGER;
    if (fscanf(stream, "%d", &dst->integer) != 1)
        return (-1);
    return (1);
}

// Read character by character, stop at unescaped "
int parse_string(json *dst, FILE *stream) {
    char    buffer[4096];
    int     i;
    int     c;

    i = 0;
    if (!accept(stream, '"')) // consume opening '"
        return (-1);
    while (1) {
        c = getc(stream);
        if (c == EOF) // string never closed
        {
            unexpected(stream);
            return (-1);
        }
        if (c == '\\') // escape sequence
        {
            c = getc(stream);
            if (c == EOF) {
                unexpected(stream);
                return (-1);
            }
        }
        else if (c == '"') // closing quote
            break ;
        buffer[i] = c;
        i++;
    }
    buffer[i] = '\0';
    dst->type = STRING;
    dst->string = malloc(i + 1);
    if (!dst->string)
        return (-1);
    int j = 0;
    while (j <= i) // <= copies the '\0' too
    {
        dst->string[j] = buffer[j];
        j++;
    }
    return (1);
}

/*
A map is { xxx pairs separated by , xxx }. Each pair is "key" : value.

dst is the MAP you're building. key is just a throwaway variable to borrow parse_string,
then you move key.string into the pair and forget about key.
 */
int parse_map(json *dst, FILE *stream) {
    json    key;
    pair    *temp;

    if (!accept(stream, '{'))
        return (-1);

    // initialize the map
    dst->type = MAP;
    dst->map.data = NULL;
    dst->map.size = 0;

    if (accept(stream, '}')) // empty map {}
        return (1);
    while (1) {
        // STEP 1: parse the key (always a string like "name")
        if (parse_string(&key, stream) == -1)
            return (-1);

        // STEP 2: expect ':' between key and value
        if (!accept(stream, ':'))
        {
            free(key.string);
            return (-1);
        }

        // STEP 3: grow the pairs array by 1
        temp = realloc(dst->map.data, sizeof(pair) * (dst->map.size + 1)); // grow the pairs array
        if (!temp)
            return (-1);
        dst->map.data = temp;

        // STEP 4: point to current slot
        pair *current = &dst->map.data[dst->map.size];

        // STEP 5: store key and value into current slot
        current->key = key.string;
        if (argo(&current->value, stream) == -1)
            return (-1);

        // STEP 6: pair is complete, increase count
        dst->map.size++;

        // STEP 7: '}' ends the map, ',' means more pairs
        if (accept(stream, '}'))
            return (1);
        if (!expect(stream, ','))
            return (-1);
    }
}

int	argo(json *dst, FILE *stream) {
    int c;

    c = peek(stream);
    if (c == EOF) {
        unexpected(stream);
        return (-1);
    }
    if (isdigit(c) || c == '-')
        return (parse_integer(dst, stream));
    if (c == '"')
        return (parse_string(dst, stream));
    if (c == '{')
        return (parse_map(dst, stream));
    unexpected(stream);
    return (-1);
}

// ----------------------------------------------------------------------------------------
// Given Below :

int	peek(FILE *stream)
{
    int	c = getc(stream);
    ungetc(c, stream);
    return c;
}

void	unexpected(FILE *stream)
{
    if (peek(stream) != EOF)
        printf("unexpected token '%c'\n", peek(stream));
    else
        printf("unexpected end of input\n");
}

int	accept(FILE *stream, char c)
{
    if (peek(stream) == c)
    {
        (void)getc(stream);
        return 1;
    }
    return 0;
}

int	expect(FILE *stream, char c)
{
    if (accept(stream, c))
        return 1;
    unexpected(stream);
    return 0;
}

void	free_json(json j)
{
    switch (j.type)
    {
        case MAP:
            for (size_t i = 0; i < j.map.size; i++)
            {
                free(j.map.data[i].key);
                free_json(j.map.data[i].value);
            }
            free(j.map.data);
            break ;
        case STRING:
            free(j.string);
            break ;
        default:
            break ;
    }
}

void	serialize(json j)
{
    switch (j.type)
    {
        case INTEGER:
            printf("%d", j.integer);
            break ;
        case STRING:
            putchar('"');
            for (int i = 0; j.string[i]; i++)
            {
                if (j.string[i] == '\\' || j.string[i] == '"')
                    putchar('\\');
                putchar(j.string[i]);
            }
            putchar('"');
            break ;
        case MAP:
            putchar('{');
            for (size_t i = 0; i < j.map.size; i++)
            {
                if (i != 0)
                    putchar(',');
                serialize((json){.type = STRING, .string = j.map.data[i].key});
                putchar(':');
                serialize(j.map.data[i].value);
            }
            putchar('}');
            break ;
    }
}

int	main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    char *filename = argv[1];
    FILE *stream = fopen(filename, "r");
    json	file;
    if (argo (&file, stream) != 1)
    {
        free_json(file);
        return 1;
    }
    serialize(file);
    printf("\n");
}
