#include "go_controller.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

/* TODO:
 * Logging functions.
 * Error checking.
 */

struct go_controller_context {
  int input_handle;
  int output_handle;
};

int go_controller_init() {
  /* TODO: Set log level */
  return 0;
}

go_controller_context* go_controller_launch_engine(const go_controller_setup* const setup) {
  int r_pipes[2];
  int w_pipes[2];
  pid_t pid;
  go_controller_context* context = malloc(sizeof(go_controller_context));

  pipe(r_pipes);
  pipe(w_pipes);
  pid = fork();

  if (pid == 0) {
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    dup2(w_pipes[0], STDIN_FILENO);
    dup2(r_pipes[1], STDOUT_FILENO);
    dup2(r_pipes[1], STDERR_FILENO);

    close(w_pipes[1]);
    close(w_pipes[0]);
    close(r_pipes[1]);
    close(r_pipes[0]);

    // TODO: This will fail if gnugo is not in the working directory
    int err = execl("gnugo", "/usr/bin/gnugo", "--mode", "gtp", NULL);
    if (err == -1)
      printf("gnugo has finished with err: %i: %i: %s\n", err, errno, strerror(errno));
    exit(1);
  }

  close(w_pipes[0]);
  close(r_pipes[1]);

  context->input_handle = w_pipes[1];
  context->output_handle = r_pipes[0];

  return context;
}

int go_controller_genmove(go_controller_colour colour, char* move, size_t move_size, go_controller_context* context) {
  char* command;
  int err;

  switch (colour) {
    case BLACK:
      command = "genmove black";
      break;
    case WHITE:
      command = "genmove white";
      break;
    default:
      return 1;
  }

  err = get_response(command, move, move_size, context);
  return err;
}

int go_controller_play(go_controller_colour colour, char* move, go_controller_context* context) {
  const size_t buf_size = 128;
  char response_buf[buf_size];
  char command[buf_size];
  int err;

  switch (colour) {
    case BLACK:
      strcpy(command, "play black ");
      break;
    case WHITE:
      strcpy(command, "play white ");
      break;
  }

  strcat(command, move);

  err = get_response(command, response_buf, buf_size, context);
  return err;
}

int go_controller_undo(go_controller_context* context) {
  const size_t buf_size = 128;
  char response_buf[buf_size];
  int err;

  err = get_response("undo", response_buf, buf_size, context);
  return err;
}

int go_controller_quit_engine(go_controller_context* context) {
  int err = send_command("quit", context);
  close(context->input_handle);
  close(context->output_handle);
  free(context);
  return err;
}

int go_controller_close() {
  return 0;
}

/*
 * Private functions
 */

int send_command(char* command, go_controller_context* context) {
  ssize_t wc;
  int len;

  len = strlen(command);
  char cmd[len + 2];
  strncpy(cmd, command, len);
  cmd[len] = '\n';
  cmd[len + 1] = '\0';

  wc = write(context->input_handle, cmd, strlen(cmd));
  if (wc < 0) {
    printf("write failed with %i: %s\n", errno, strerror(errno));
    return 1;
  }
#ifdef DEBUG
  printf("wrote %s", cmd);
  printf("wrote %zd bytes\n", wc);
#endif
  return 0;
}


int wait_for_activity(int read_handle) {
  fd_set read_fds;
  int err;

  FD_ZERO(&read_fds);
  FD_SET(read_handle, &read_fds);
#ifdef DEBUG
  printf("Waiting...\n");
#endif
  err = select(read_handle + 1, &read_fds, NULL, NULL, NULL);
  if (err < 0) {
    printf("Error occurred: %i: %s\n", errno, strerror(errno));
  }
#ifdef DEBUG
  printf("Finished waiting...\n");
#endif
  return err;
}

int get_response(char* command, char* response, size_t response_buf_size, go_controller_context* context) {
  ssize_t rc;
  int err;

  /* Send the command */
  err = send_command(command, context);
  if (err != 0)
    return err;

  /* Retrieve the response */
  // TODO: Do we need to loop here? if so how to exit?
  rc = wait_for_activity(context->output_handle);
  if (rc < 0)
    return rc;
  else if (rc == 0) {
    printf("wait_for_activity returned zero\n");
  }
  else {
#ifdef DEBUG
    printf("wait_for_activity returned %zd\n", rc);
#endif
    memset(response, 0, response_buf_size);
    rc = read(context->output_handle, response, response_buf_size);
    if (rc < 0) {
      printf("read failed with %i: %s\n", errno, strerror(errno));
      return 1;
    }
    else {
      // TODO: Check response for errors and clean it up
      response[rc] = '\0';
#ifdef DEBUG
      printf("read %zd bytes\n", rc);
      printf("Received response: %s\n", response);
#endif
    }
  }

  return 0;
}
