/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yolim <yolim@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 14:25:59 by yolim             #+#    #+#             */
/*   Updated: 2026/04/20 21:07:35 by yolim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h> // for pipe. fork, dup2, execvp, close (type man 2 pipe in terminal to get library)
#include <stdlib.h> // for exit (type man 3 exit in terminal to get library)

/*
ft_popen forks a child process, runs a command in it, and returns a file
descriptor (fd) so the parent can communicate with that command via a pipe.

A pipe has two ends:
  pipe[0] = read end (data comes OUT here)
  pipe[1] = write end (data goes IN here)
Data flows in one direction: written into pipe[1], read from pipe[0].

Type 'r' — Read from the command's output
Child process (ls)           Parent process
      stdout (fd 1)   ──pipe──►  pipe[0]  ← you read from here

Type 'w' — Write to the command's input
Parent process               Child process (grep)
      pipe[1]  ← you write here  ──pipe──►  stdin (fd 0)
 */

int ft_popen(const char *file, char *const argv[], char type) {
    int     fd[2];
    pid_t   pid;

    if (!file || !argv || (type != 'r' && type != 'w'))
        return (-1);
    if (pipe(fd) == -1)
        return (-1);
    pid = fork();
    if (pid == -1) {
        close(fd[0]);
        close(fd[1]);
        return (-1);
    }
    if (pid == 0) {
        if (type == 'r') {
            if (dup2(fd[1], 1) == -1)
                exit(1);
        }
        else {
            if (dup2(fd[0], 0) == -1)
                exit(1);
        }
        close(fd[0]);
        close(fd[1]);
        execvp(file, argv);
        exit(1);
    }
    if (type == 'r') {
        close(fd[1]);
        return (fd[0]);
    }
    close(fd[0]);
    return (fd[1]);
}

/*#include <stdio.h>

int main(void) {
    int fd;
    char buf[1024];
    ssize_t read_byte;

    printf("=== TEST 1: 'r' with echo ===\n");
    fd = ft_popen("echo", (char *const[]){"echo", "Hello World", NULL}, 'r');
    if (fd == -1)
        return (printf("Fail : 'r'"), 1);
    read_byte = read(fd, buf, sizeof(buf) - 1);
    buf[read_byte] = '\0';
    printf("Got : %s", buf);
    close(fd);

    printf("=== TEST 2: 'w' with cat ===\n");
    fd = ft_popen("cat", (char *const[]){"cat", NULL}, 'w');
    if (fd == -1)
        return (printf("Fail : 'w'"), 1);
    write(fd, "Got : writing to cat\n", 21);
    close(fd);
    sleep(5);
    return (0);
}*/