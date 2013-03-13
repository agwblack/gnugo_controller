#include "go_controller.h"
#include <stdio.h>

int main() {
  int err, size;
  go_controller_setup setup;
  go_controller_context *context;

  err = go_controller_init();
  if (err != 0) {
    printf("init failed\n");
    return 1;
  }

  setup.level = 10;
  context = NULL;
  context = go_controller_launch_engine(&setup);
  if (context == NULL) {
    printf("Failed to launch engine\n");
    return 1;
  }

  size = 128;
  char move[size];
  err = go_controller_genmove(BLACK, move, size, context);
  if (err != 0) {
    printf("genmove failed\n");
    return 1;
  }
  printf("Generated move: %s\n", move);

  err = go_controller_quit_engine(context);
  if (err != 0) {
    printf("engine quit failed\n");
    return 1;
  }
  err = go_controller_close();
  if (err != 0) {
    printf("close failed\n");
    return 1;
  }
  return 0;
}
