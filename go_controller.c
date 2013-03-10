#include "go_controller.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

struct go_controller_context {
  int input_handle;
  int output_handle;
};

int go_controller_init() {
  return 0;
}

int go_controller_launch_engine(const go_controller_setup* const setup, go_controller_context* context) {
  int r_pipes[2];
  int w_pipes[2];
  pid_t pid;

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

    execl("gnugo", "/usr/bin/gnugo", "--mode", "gtp", NULL);
    exit(1);
  }

  close(w_pipes[0]);
  close(r_pipes[1]);

  context->input_handle = w_pipes[1];
  context->output_handle = r_pipes[0];

  return 0;
}

int go_contoller_genmove(go_controller_colour colour, char* move, size_t move_size, go_controller_context* context) {
  char* command;
  const size_t buf_size = 128;
  char response_buf[buf_size];
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

  err = get_response(command, response_buf, buf_size, context);
  return err;
}

int go_contoller_play(go_controller_colour colour, char* move, go_controller_context* context) {
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
  return send_command("quit", context);
}

int go_controller_close() {
  return 0;
}

/*
 * Private functions
 */

int send_command(char* command, go_controller_context* context) {
  ssize_t wc;
  size_t len = strlen(command) + 2;
  char cmd[len];

  strcpy(command, cmd);
  cmd[len - 2] = '\n';
  cmd[len - 1] = '\0';

  wc = write(context->input_handle, command, strlen(command));
  if (wc < 0) {
    printf("write failed with %i: %s\n", errno, strerror(errno));
    return 1;
  }
  return 0;
}


int get_response(char* command, char* response, size_t response_buf_size, go_controller_context* context) {

  char buf[response_buf_size];
  ssize_t rc;
  int err;

  /* Send the command */
  err = send_command(command, context);
  if (err != 0)
    return err;

  /* Retrieve the response */
  memset(buf, 0, response_buf_size);
  rc = read(context->output_handle, buf, response_buf_size);
  if (rc < 0) {
    printf("read failed with %i: %s\n", errno, strerror(errno));
    return 1;
  }
  else {
    buf[rc] = '\0';
    printf("Received response: %s", buf);
    // TODO: Check response for errors
    memcpy(response, buf, rc + 1);
  }

  return 0;
}
