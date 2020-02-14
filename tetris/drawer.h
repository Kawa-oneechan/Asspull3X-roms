#ifndef DRAWER_HEADER
#define DRAWER_HEADER

#include "grid.h"
#include "helpers.h"

/*
 * scene:  represents the current scene on the screen.
 *
 * grid       : grid to be drawn.
 */
typedef struct
{
        grid *grid;
} scene;

/*
 * init_scene:  creates a new scene with provided arguments,
 *              returns the pointer to the scene.
 */
scene *init_scene(grid *grid);

/*
 * refresh_scene:  draw scene in its current state.
 */
void refresh_scene(const scene *scene);

/*
 * free_scene:  frees memory occupied by scene.
 */
void free_scene(scene *scene);

#endif
