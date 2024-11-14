#include "pipex.h"

char *find_path(char **env, char *cmd)
{
    int i = 0;
    char **paths;
    char *path_2;
    char *path;

    while (env[i])
    {
        if (ft_strnstr(env[i], "PATH", 4) == env[i])
        {
            paths = ft_split(env[i] + 5, ':');
            break;
        }
        i++;
    }
    
    if (!env[i])
    {
        perror("PATH not found in environment");
        return NULL;
    }

    i = 0;
    while (paths[i])
    {
        path_2 = ft_strjoin(paths[i], "/");
        path = ft_strjoin(path_2, cmd);
        free(path_2);

        if (access(path, F_OK) == 0)
        {
            while (paths[i])
                free(paths[i++]);
            free(paths);
            return path;
        }
        free(path);
        i++;
    }

    while (paths[i])
        free(paths[i++]);
    free(paths);

    return NULL;
}

void execute(char **av, char **env, char *cmd)
{
    char *path;
    char **cmd_2;

    cmd_2 = ft_split(cmd, ' ');
    path = find_path(env, cmd_2[0]);
    
    if (!path) {
        ft_printf("Command not found: %s\n", cmd_2[0]);
        int i = 0;
        while (cmd_2[i])
            free(cmd_2[i++]);
        free(cmd_2);
        exit(EXIT_FAILURE);
    }
    
    if (execve(path, cmd_2, env) == -1) {
        perror("execve failed");
        int i = 0;
        while (cmd_2[i])
            free(cmd_2[i++]);
        free(cmd_2);
        exit(EXIT_FAILURE);
    }
}

void children(int *pipe_fd, char **av, char **env)
{
    int output;

    output = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output < 0) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    dup2(output, STDOUT_FILENO);
    close(output);

    dup2(pipe_fd[0], STDIN_FILENO);
    close(pipe_fd[1]);

    execute(av, env, av[3]);
}

void parent(int *pipe_fd, char **av, char **env)
{
    int infile;

    infile = open(av[1], O_RDONLY);
    if (infile < 0) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    dup2(infile, STDIN_FILENO);
    close(infile);

    close(pipe_fd[0]);
    dup2(pipe_fd[1], STDOUT_FILENO);

    execute(av, env, av[2]);
}

int main(int ac, char **av, char **env)
{
    pid_t pid;
    int pipe_fd[2];

    if (ac == 5)
    {
        if (pipe(pipe_fd) == -1) {
            perror("pipe failed");
            return 1;
        }

        pid = fork();
        if (pid == -1) {
            perror("fork failed");
            return 1;
        }

        if (pid == 0)
            children(pipe_fd, av, env);
        else {
            parent(pipe_fd, av, env);
            waitpid(pid, NULL, 0);
        }

        close(pipe_fd[0]);
        close(pipe_fd[1]);
    }
    else
    {
        ft_printf("Usage: ./pipex infile cmd1 cmd2 outfile\n");
        return 1;
    }

    return 0;
}
