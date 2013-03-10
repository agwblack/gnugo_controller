/*
 * Library to interface with gnugo
 */


#include <stdlib.h>

/* * * * * * * * * *
 * Data structures *
 * * * * * * * * * */

typedef enum go_controller_colour {
  BLACK,
  WHITE
} go_controller_colour;

/* Struct that holds all the initialisation information for gnugo. */
typedef struct {
  int level;
} go_controller_setup;

/* Opaque type which contains information pertaining to a particular game */
typedef struct go_controller_context go_controller_context;



/* * * * * * *
 * Functions *
 * * * * * * */

/* Call this before calling any other functions */
int go_controller_init();

/* Should be called once for each game gnugo is to manage. setup contains the
 * data that specifies what kind of game will be initialised. context will be
 * initialised appropriately, and should be passed to all other function calls
 * concerning this game */
int go_controller_launch_engine(const go_controller_setup* const setup, go_controller_context* context);

/* Generates a move for the given colour, given the specified context */
int go_controller_genmove(go_controller_colour colour, char* move, size_t move_size, go_controller_context* context);

/* Adds a move for the given colour to the specified context */
int go_contoller_play(go_controller_colour colour, char* move, go_controller_context* context);

/* Removes the previous move from the specified context */
int go_controller_undo(go_controller_context* context);

/* Kills the gnugo engine for this context. context is set to NULL and should
 * not be used again, other than for a further call to
 * go_controller_launch_engine */
int go_controller_quit_engine(go_controller_context* context);

/* Call to tidy up once finished using the library */
int go_controller_close();



/*
 * Internal functions and data.
 * Do not use directly
 */

/* sends command to the engine but doesn't wait for response */
int send_command(char* command, go_controller_context* context);

/* Sends command and gets response */
int get_response(char* command, char* response, size_t response_buf_size, go_controller_context* context);
