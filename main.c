#include "pipex.h"

char * find_path(char **env, char *cmd)
{
    int i = 0;
    char **paths;
    char *path_2;
    char *path;
    while (env[i])
    {
        if (ft_strnstr(env[i], "PATH", 4) == 0)
        {
            paths = ft_split(env[i] + 5, ':');
            break ;
        }
        i++;
    }
    i = 0;
    while (paths[i])
    {
        path_2 = ft_strjoin(paths[i], "/");
        path = ft_strjoin(path_2, cmd);
        if (access(path, F_OK) == 0)
        {
            return (path);
        }
        i++;
    }
    return NULL;
}
void execute (char **av, char **env, char* cmd)
{
    char *path;
    char **cmd_2;
    cmd_2 = ft_split(cmd, ' ');
    path = find_path(env, cmd_2[0]);
    execve(path, cmd_2, env);
}
void children (int *pipe_fd, char **av, char **env)
{
    int output;

    output = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(output, 1);
    close(output);
    dup2(pipe_fd[0], 0);
    close(pipe_fd[1]);
    execute(av, env, av[3]);
}

void parent (int *pipe_fd, char **av, char **env)
{
    int infile;
    infile = open(av[1], O_RDONLY);
    dup2(infile, 0);
    close(infile);
    close(pipe_fd[0]);
    dup2(pipe_fd[1], 1);
    execute(av, env, av[2]);
}

int main(int ac, char **av, char **env)
{
    pid_t pid;
    int infile;
    int output;
    int pipe_fd[2];
    char *path;

    if (ac == 5)
    {
        pipe(pipe_fd);
        pid = fork();

        if (pid == 0)
        {
            children(pipe_fd, av, env);
        }
        else if (pid > 0)
        {
            parent(pipe_fd, av, env);
        }
        else
        {
            perror("fork failed");
            return 1;
        }

        close(pipe_fd[0]);
        close(pipe_fd[1]);

        waitpid(pid, NULL, 0);
    }
    else
    {
        ft_printf("Usage: ./pipex infile cmd1 cmd2 outfile\n");
    }

    return 0;
}
