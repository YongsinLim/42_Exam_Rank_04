/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandbox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yolim <yolim@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 19:46:36 by yolim             #+#    #+#             */
/*   Updated: 2026/04/23 18:05:41 by yolim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h> // for fork, alarm,
#include <sys/wait.h> // for waitpid,
#include <stdlib.h> // for exit
#include <signal.h> // for sigaction, kill, sigaddset, sigemptyset, sigfillset, sigdelset, sigismember
#include <stdio.h> // for printf
#include <string.h> // for strsignal
#include <sys/errno.h> // for errno
#include <stdbool.h>

/*
empty handler's job is :
"exist but do nothing" — OS doesn't kill the parent and just interrupts waitpid.
waitpid then will return -1, errno = EINTR  ✓

if no this empty handler, OS will use default action for SIGALRM (terminate the process  ← parent dies)
waitpid never returns -1 with EINTR
child becomes zombie
*/
void    alarm_handler(int sig) {
    (void)sig;
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose) {

    struct sigaction    sa;
    pid_t               pid;
    int                 status;

    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // not set to SA_RESTART — lets waitpid return EINTR (so can detect timeout)
    sigaction(SIGALRM, &sa, NULL);

    pid = fork();
    if (pid == -1)
        return (-1);
    if (pid == 0) {
        f(); // child: run f
        exit(0); // if f() returns normally, exit with 0
    }
    alarm(timeout); // sends SIGALRM to parent after timeout seconds

    if (waitpid(pid, &status, WUNTRACED) == -1 && errno == EINTR) {
        // waitpid return -1 when if an error occurred
        // errno = EINTR   ← "I was interrupted by a signal"
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0); // reap child, no zombie; NULL : don't need this info.
        if (verbose)
            printf("Bad function: timed out after %u seconds\n", timeout);
        return (0);
    }

    alarm(0); // cancel alarm if child finished before timeout

    if (WIFEXITED(status)) {
        // child called exit() normally
        int exit_code = WEXITSTATUS(status);
        if (exit_code == 0) {
            if (verbose)
                printf("Nice function!\n");
            return (1); // nice function
        }
        else {
            if (verbose)
                printf("Bad function: exited with code %d\n", exit_code);
            return (0); // bad function: wrong exit code
        }
    }
    if (WIFSIGNALED(status))  // child was killed by a signal
    {
        int sig = WTERMSIG(status);
        if (verbose)
            printf("Bad function: %s\n", strsignal(sig));
        return (0); // bad function
    }
    if (WIFSTOPPED(status)) {
        // child was stopped by a signal
        int sig1 = WSTOPSIG(status);
        if (verbose)
            printf("Bad function: %s\n", strsignal(sig1));
        kill(pid, SIGKILL); // kill it so no zombie
        waitpid(pid, NULL, 0);
        return (0);
    }
    return (-1);
}
