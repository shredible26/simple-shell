#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_INPUT_LENGTH 1024
#define DIR_STACK_SIZE 16

char *directory_stack[DIR_STACK_SIZE];
int directory_stack_top = -1;

void print_prompt() {
  fputs("shell_jr: ", stdout);
  fflush(stdout);
}

void handle_dirs() {
  int dir_index;
  for (dir_index = 0; dir_index <= directory_stack_top; dir_index++) {
    puts(directory_stack[dir_index]);
  }
}

void handle_cd(const char *dir_path) {
  if (dir_path == NULL || chdir(dir_path) != 0) {
    fprintf(stderr, "Cannot change to directory %s", dir_path);
  }
}

void handle_pushd(const char *new_path) {
  if (directory_stack_top >= DIR_STACK_SIZE - 1) {
    fputs("Directory stack is full\n", stderr);
    return;
  }

  char *curr_directory = getcwd(NULL, 0);
  if (curr_directory == NULL) {
    perror("getcwd");
    return;
  }

  if (chdir(new_path) != 0) {
    fprintf(stderr, "Cannot change to directory %s\n", new_path);
    free(curr_directory);
    return;
  }

  directory_stack[++directory_stack_top] = curr_directory;
}

void handle_popd() {
  if (directory_stack_top < 0) {
    fputs("Directory stack is empty\n", stderr);
    return;
  }

  char *previous_directory = directory_stack[directory_stack_top];
  directory_stack[directory_stack_top--] = NULL;
  if (chdir(previous_directory) != 0) {
    fprintf(stderr, "Cannot change to directory %s\n", previous_directory);
  }
  free(previous_directory);
}

void execute_external_command(const char *cmd, const char *cmd_arg) {
  pid_t process_id;
  char *arg_list[3];
  int exec_status;

  process_id = fork();
  if (process_id == 0) {
    arg_list[0] = (char *)cmd;
    arg_list[1] = (char *)cmd_arg;
    arg_list[2] = NULL;
    execvp(cmd, arg_list);
    fprintf(stdout, "Failed to execute %s\n", cmd);
    exit(EXIT_FAILURE);
  } else if (process_id > 0) {
    waitpid(process_id, &exec_status, 0);
  } else {
    perror("fork");
  }
}

int main() {
  char user_input[MAX_INPUT_LENGTH];
  char *user_command;
  char *cmd_argument;
  int stack_index;

  for (stack_index = 0; stack_index < DIR_STACK_SIZE; ++stack_index) {
    directory_stack[stack_index] = NULL;
  }

  while (1) {
    print_prompt();

    if (fgets(user_input, MAX_INPUT_LENGTH, stdin) == NULL) {
      break;
    }

    user_input[strcspn(user_input, "\n")] = 0;
    user_command = strtok(user_input, " ");
    if (user_command == NULL) continue;
    cmd_argument = strtok(NULL, " ");

    if (strcmp(user_command, "exit") == 0 || strcmp(user_command, "goodbye") == 0) {
      fputs("See you\n", stdout);
      exit(0);
    } else if (strcmp(user_command, "cd") == 0) {
      handle_cd(cmd_argument);
    } else if (strcmp(user_command, "pushd") == 0) {
      handle_pushd(cmd_argument);
    } else if (strcmp(user_command, "dirs") == 0) {
      handle_dirs();
    } else if (strcmp(user_command, "popd") == 0) {
      handle_popd();
    } else {
      execute_external_command(user_command, cmd_argument);
    }
  }

  for (stack_index = 0; stack_index <= directory_stack_top; ++stack_index) {
    free(directory_stack[stack_index]);
  }

  return 0;
}
