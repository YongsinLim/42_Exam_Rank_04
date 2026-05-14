/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yolim <yolim@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 17:35:31 by yolim             #+#    #+#             */
/*   Updated: 2026/05/14 22:38:38 by yolim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h> // for close, fork, execvp, dup2, pipe
#include <sys/wait.h> // for wait
#include <stdlib.h> // for exit

int    picoshell(char **cmds[]) {
    int     fd[2];
    pid_t   pid;
    int     last_fd;
    int     i;
    int     status;

    last_fd = -1;
    i = 0;
    while (cmds[i]) {
        if (cmds[i + 1] && pipe(fd) == -1)
            return (1);
        pid = fork();
        if (pid == -1) {
            if (last_fd != -1)
                close(last_fd);
            if (cmds[i + 1]) {
                close(fd[0]);
                close(fd[1]);
            }
            return (1);
        }
        if (pid == 0) {
            if (last_fd != -1) {
                if (dup2(last_fd, STDIN_FILENO) == -1)
                    exit(1);
               close(last_fd);
            }
            if (cmds[i + 1]) {
                close(fd[0]);
                if (dup2(fd[1], STDOUT_FILENO) == -1)
                    exit(1);
                close(fd[1]);
            }
            execvp(cmds[i][0], cmds[i]);
            exit(1);
        }
        if (last_fd != -1)
            close(last_fd);
        if (cmds[i + 1]) {
            close(fd[1]);
            last_fd = fd[0];
        }
        i++;
    }
    while(wait(&status) > 0)
        ;
    return (0);
}

/*#include <stdio.h>

int main(void) {
    char    *cmd1[] = {"ls", NULL};
    char    *cmd2[] = {"grep", "picoshell", NULL};
    char    **cmds[] = {cmd1, cmd2, NULL};

    printf("=== ls | grep picoshell ===\n");
    int ret = picoshell(cmds);
    printf("returned: %d\n", ret); // expect: picoshell (filename) printed, return 0
}*/