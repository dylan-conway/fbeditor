#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

int main(){
  int num_children = 1;

  pid_t *pid = malloc(sizeof(pid_t) * num_children);

  char *input_args[] = { "./bin/input", (char*)NULL };
  // char *game_args[] = { "./bin/game", (char*)NULL };
  
  char ***args = malloc(sizeof(char**) * num_children);
  args[0] = input_args;
  // args[1] = game_args;

  for(int i = 0; i < num_children; i++){

    pid[i] = fork();
    if(pid[i] == 0){
      execvp(args[i][0], args[i]);
      exit(0);
    }

  }

  for(int i = 0; i < num_children; i++){
    if((int)waitpid(pid[i], 0, WUNTRACED) == -1){
      perror("Failed to wait for child");
    }
  }

  free(pid);
  free(args);
  return 0;
}