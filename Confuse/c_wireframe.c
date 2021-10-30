#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "c_internal.h"
#include "relinquish.h"


void (*c_preview_draw_line)(float x_a, float y_a, float z_a, float x_b, float y_b, float z_b) = NULL;
void (*c_preview_draw_sphere)(float x, float y, float z, float radius) = NULL;
void (*c_preview_draw_axis)(float x_a, float y_a, float z_a, float x_b, float y_b, float z_b) = NULL;


void c_preview_draw_line_func_set(void (*draw_line)(float x_a, float y_a, float z_a, float x_b, float y_b, float z_b))
{
	c_preview_draw_line = draw_line;
}

void c_preview_draw_sphere_func_set(void (*draw_sphere)(float x, float y, float z, float radius))
{
	c_preview_draw_sphere = draw_sphere;
}

void c_preview_draw_axis_func_set(void (*draw_axis)(float x_a, float y_a, float z_a, float x_b, float y_b, float z_b))
{
	c_preview_draw_axis = draw_axis;
}