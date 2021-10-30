#if !defined(SEDUCE_H)
#define	SEDUCE_H

/*
This is the new and improved .h file for the "ui tool kit" used in varius verse related applications.
*/

#include "forge.h"
#include "betray.h"
#include "relinquish.h"

/*     Initialize Seduce      */
/* -------------------------- */

extern void	seduce_init(void);
extern void seduce_element_endframe(BInputState *input, boolean debug);

extern void seduce_animate(BInputState *input, float *timer, boolean active, float speed);

#define SEDUCE_T_SIZE 0.007
#define SEDUCE_T_SPACE 0.2
#define SEDUCE_T_SPACEING 2.5

/* text lines */

extern float	seduce_text_line_draw(float pos_x, float pos_y, float letter_size, float letter_spacing, const char *text, float red, float green, float blue, float alpha, uint length);
extern uint		seduce_text_line_hit_test(float letter_size, float letter_spacing, const char *text, float pos_x);
extern float	seduce_text_line_length(float size, float spacing, const char *text, uint end);

typedef enum{
	S_TIS_IDLE,
	S_TIS_ACTIVE,
	S_TIS_DONE
}STypeInState;

extern STypeInState	seduce_text_line_edit(BInputState *input, void *id, char *text, uint buffer_size, float pos_x, float pos_y, float length, float size, char *label, boolean left, void (*done_func)(void *user, char *text), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha);
extern STypeInState	seduce_text_password_edit(BInputState *input, void *id, char *text, uint buffer_size, float pos_x, float pos_y, float length, float size, char *label, boolean left, void (*done_func)(void *user, char *text), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha);
extern STypeInState	seduce_text_double_edit(BInputState *input, void *id, double *value, float pos_x, float pos_y, float length, float size, boolean left, void (*done_func)(void *user, double value), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha);
extern STypeInState	seduce_text_float_edit(BInputState *input, void *id, float *value, float pos_x, float pos_y, float length, float size, boolean left, void (*done_func)(void *user, double value), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha);

extern STypeInState	seduce_text_int_edit(BInputState *input, void *id, int *value, float pos_x, float pos_y, float length, float size, boolean left, void (*done_func)(void *user, int value), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha);
extern STypeInState	seduce_text_uint_edit(BInputState *input, void *id, uint *value, float pos_x, float pos_y, float length, float size, boolean left, void (*done_func)(void *user, uint value), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha);

/* text blocks */

typedef enum{
	SEDUCE_BS_LEFT,
	SEDUCE_BS_RIGHT,
	SEDUCE_BS_CENTER,
	SEDUCE_BS_STRETCH,
	SEDUCE_BS_CODE
}SUIBlockStyle;

extern uint		seduce_text_block_draw(float pos_x, float pos_y, float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, SUIBlockStyle style, const char *text, uint text_start, float *color, uint *color_switch, uint *scroll, void (*func)(float pos_x, float pos_y, float *start, float *end, void *user), void *user);
extern uint		seduce_text_block_hit_test(float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, const char *text, float pos_x, float pos_y);

typedef struct{
	float pos_x;
	float pos_y;
	float letter_size;
	float letter_spacing;
	float line_size;
	float line_spacing;
	uint line_count;
	SUIBlockStyle style;
}SUIBlock;

extern uint		sui_text_block_edit(SUIBlock *blocks, uint block_count, const char *text, void (*done_func)(void *user, char *text), void* user, float *color, uint *color_switch);

/* text buttons */

extern boolean	seduce_text_button(BInputState *input, void *id, float pos_x, float pos_y, float center, float size, float spacing, const char *text, float red, float green, float blue, float alpha, float red_select, float green_select, float blue_select, float alpha_select);
extern uint		seduce_text_button_list(BInputState *input, void *id, float pos_x, float pos_y, float center, float size, float spacing, const char **texts, uint text_count, float red, float green, float blue, float alpha, float red_select, float green_select, float blue_select, float alpha_select);

/* tool tips */

extern void		seduce_tool_tip(BInputState *input, void *id, char *tooltip, char *extended);

/* Language translation */

extern char		*seduce_translate(char *text);
extern void		seduce_translate_load(char *file);
extern void		seduce_translate_save(char *file);

/* Popup Menus */

boolean seduce_popup_detect_mouse(BInputState *input, uint button, void (*func)(BInputState *input, float time, void *user), void *user);
boolean seduce_popup_detect_multitouch(BInputState *input, uint finger_count, void (*func)(BInputState *input, float time, void *user), void *user);
boolean seduce_popup_detect_axis(BInputState *input, uint button, void (*func)(BInputState *input, float time, void *user), void *user);
boolean seduce_popup_detect_icon(BInputState *input, void *id, uint icon, float pos_x, float pos_y,  float scale, float time, void (*func)(BInputState *input, float time, void *user), void *user, boolean displace, float *color);

#define SEDUCE_POP_UP_NO_ACTION -1
#define SEDUCE_POP_UP_DEACTIVATE -2

typedef enum{
	S_PUT_TOP,
	S_PUT_BOTTOM,
	S_PUT_ANGLE,
	S_PUT_BUTTON,
	S_PUT_IMAGE,
}SPopUpType;

typedef struct{
	SPopUpType	type;
	char		*text;
	union{
		float button_pos[2];
		float angle[2];
		struct{
			float pos[2];
			float size[2];
			uint texture_id;
		}image;
		struct{
			float angle[2];
			float *value;
		}slider_angle;
	}data;
}SUIPUElement;

extern uint seduce_popup_simple(BInputState *input, uint user_id, float pos_x, float pos_y, char **lables, uint element_count, float *time, boolean active, float red, float green, float blue, float red_active, float green_active, float blue_active);
extern uint seduce_popup(BInputState *input, SUIPUElement *elements, uint element_count, float time);
/* view and projection code */

extern void seduce_object_3d_color(float col_a_r, float col_a_g, float col_a_b, float col_b_r, float col_b_g, float col_b_b, float reflect_r, float reflect_g, float reflect_b);

extern boolean seduce_widget_icon_button(BInputState *input, void *id, uint icon, float pos_x, float pos_y,  float scale, float time, float *color);
extern boolean seduce_widget_icon_button_invisible(BInputState *input, void *id, float pos_x, float pos_y, float scale);
extern boolean seduce_widget_icon_toggle(BInputState *input, void *id, boolean *value, uint icon, float pos_x, float pos_y, float scale, float time);
extern boolean seduce_widget_slider_line(BInputState *input, void *id, float *value, float pos_a_x, float pos_a_y, float pos_b_x, float pos_b_y, float size, float scale, float min, float max, float time, float *color, boolean snaps);
extern boolean seduce_widget_slider_radial(BInputState *input, void *id, float *value, float pos_x, float pos_y, float size, float scale, float min, float max, float time, float *color);
extern boolean seduce_widget_wheel_radial(BInputState *input, void *id, float *color, float pos_x, float pos_y, float size, float scale, float time);
extern boolean seduce_widget_select_radial(BInputState *input, void *id, uint *selected, char **lables, uint element_count, SPopUpType type, float pos_x, float pos_y, float size, float scale, float time, boolean release_only);

/*

Not implemented yet.

extern void s_widget_pie(BInputState *input, float pos_x, float pos_y, float size, float scale, float time, uint slize_count, float *values, float *colors);
extern void s_widget_chart(BInputState *input, float pos_x, float pos_y, float size_x, float size_y, float scale, float time, uint chart_count, uint chart_length, float **values, float min, float max, boolean auto_scale, float *colors, char **lables, boolean indicators, float select_red, float select_green, float select_blue);
*/
/*
typedef enum{
	SEDUCE_PET_LABEL,
	SEDUCE_PET_BUTTON,
	SEDUCE_PET_BOOLEAN,
	SEDUCE_PET_INTEGER,
	SEDUCE_PET_UNSIGNED_INTEGER,
	SEDUCE_PET_REAL,
	SEDUCE_PET_SLIDER,
	SEDUCE_PET_TEXT,
	SEDUCE_PET_PASSWORD,
	SEDUCE_PET_COLOR_RGB,
	SEDUCE_PET_COLOR_WHEEL, 
	SEDUCE_PET_COLOR_HUE, 
	SEDUCE_PET_COLOR_SATURATION,
	SEDUCE_PET_COLOR_VALUE,
	SEDUCE_PET_SELECT,
	SEDUCE_PET_POPUP,
//	SEDUCE_PET_CUSTOM,
	SEDUCE_PET_SECTION_START,
	SEDUCE_PET_SECTION_END
}SeducePanelElementType;
*/
typedef enum{
	SEDUCE_PET_BOOLEAN,
	SEDUCE_PET_TRIGGER,
	SEDUCE_PET_INTEGER,
	SEDUCE_PET_UNSIGNED_INTEGER,
	SEDUCE_PET_INTEGER_BOUND,
	SEDUCE_PET_REAL,
	SEDUCE_PET_REAL_BOUND,
	SEDUCE_PET_2D_POS,
	SEDUCE_PET_3D_POS,
	SEDUCE_PET_4D_POS,
	SEDUCE_PET_QUATERNION,
	SEDUCE_PET_2D_NORMAL,
	SEDUCE_PET_3D_NORMAL,
	SEDUCE_PET_2X2MATRIX,
	SEDUCE_PET_3X3MATRIX,
	SEDUCE_PET_4X4MATRIX,
	SEDUCE_PET_TEXT,
	SEDUCE_PET_PASSWORD,
	SEDUCE_PET_TEXT_BUFFER,
	SEDUCE_PET_COLOR_RGB,
	SEDUCE_PET_COLOR_RGBA,
	SEDUCE_PET_TIME,
	SEDUCE_PET_DATE,
	SEDUCE_PET_SELECT,
	SEDUCE_PET_SECTION_START,
	SEDUCE_PET_SECTION_END,
	SEDUCE_PET_CUSTOM,
	SEDUCE_PET_COUNT
}SeducePanelElementType;
/*
Tags

*/
typedef struct{
	SeducePanelElementType type;
	char *text;
	char *description;
	union{
		boolean active; 
		boolean trigger; 
		int		integer;
		uint	uinteger;
		struct{
			double value;
			double max;
			double min;
		}real;
		float color[4];
		double vector[4];
		double matrix[16];
		char text[64];
		struct{
			char	*text_buffer;
			uint	buffer_size;
		}buffer;
		double time; /* Seconds in a day */
		struct{
			uint16	year;
			uint8	month;
			uint8	day;
		}date;
		struct{
			char	**text;
			uint	count;
			uint	active;
		}select;
		struct{
			void *data;
			void *function;
		}custom;
	}param;
}SeducePanelElement;

extern void seduce_widget_list_element_background(BInputState *input, float pos_x, float pos_y, float width, float scale, SeducePanelElement *element, uint element_count, uint *selected_element, void *id, float time);
extern void seduce_widget_list_element_list(BInputState *input, float pos_x, float pos_y, float width, float scale, SeducePanelElement *element, uint element_count, uint *selected_element, void *id, float time);

extern void seduce_settings_betray_set(SeducePanelElement *element);
extern void seduce_settings_betray_get(SeducePanelElement *element);

/*
extern void sui_widget_panel(BInputState *input, float pos_x, float pos_y, float length, float scale, SeducePanelElement *element, uint element_count, void *id, float time, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha);

extern float sui_setting_view_size(SeducePanelElement *element);
*/
/* Background panel code */
/* --------------------- */


extern boolean seduce_background_tri_draw(BInputState *input, void *id,
														float a_x, float a_y, float a_z, 
														float b_x, float b_y, float b_z, 
														float c_x, float c_y, float c_z, 
														float normal_x, float normal_y, float particle_influence,
														float surface_r, float surface_g, float surface_b, float surface_a);

extern boolean seduce_background_quad_draw(BInputState *input, void *id,
														float a_x, float a_y, float a_z, 
														float b_x, float b_y, float b_z, 
														float c_x, float c_y, float c_z, 
														float d_x, float d_y, float d_z, 
														float normal_x, float normal_y, float particle_influence,
														float surface_r, float surface_g, float surface_b, float surface_a);

extern void seduce_background_polygon_flush(BInputState *input, float *center, float time);

extern boolean seduce_background_image_draw(BInputState *input, void *id, float pos_x, float pos_y, float pos_z, float size_x, float size_y, float u_start, float v_start, float u_size, float v_size, float timer, float *center, uint texture_id);

extern boolean seduce_background_gallery_vertical_draw(BInputState *input, float pos_x, float pos_y, float pos_z, float size_x, float *size_y, float image_height, uint *texture_id, float *aspects, uint image_count, float timer, float *center);
extern boolean seduce_background_gallery_horizontal_draw(BInputState *input, float pos_x, float pos_y, float pos_z, float *size_x, float size_y, float image_height, uint *texture_id, float *aspects, uint image_count, float timer, float *center);

typedef enum{
	S_PT_PRIMARY,
	S_PT_SECONDARY,
	S_PT_SURFACE
}SPraticleType;

extern SPraticleType seduce_background_particle_color_allocate(void *id, float red, float green, float blue);

extern void seduce_background_particle_spawn(BInputState *input, float pos_x, float pos_y, float vec_x, float vec_y, float start_age, SPraticleType type);
extern void seduce_background_particle_burst(BInputState *input, float pos_x, float pos_y, uint count, float speed, SPraticleType type);
extern void seduce_background_particle_square(BInputState *input, float pos_x, float pos_y, float size_x, float size_y, uint count, SPraticleType type);

typedef enum{
	SEDUCE_CS_SURFACE,
	SEDUCE_CS_SURFACE_LIGHT,
	SEDUCE_CS_SURFACE_SHADE,
	SEDUCE_CS_OBJECT,
	SEDUCE_CS_OBJECT_SHADED,
	SEDUCE_CS_OBJECT_SIGNAL,
	SEDUCE_CS_COUNT
}SColorSettings;

extern void seduce_color_set(SColorSettings setting, float red, float green, float blue, float alpha);


extern void seduce_background_color(float surface_r, float surface_g, float surface_b, float surface_a, 
									 float light_r, float light_g, float light_b, float light_a, 
									 float shade_r, float shade_g, float shade_b, float shade_a);

extern void		seduce_background_negative_draw(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, float d_x, float d_y, float timer);
extern boolean	seduce_background_square_draw(BInputState *input, void *id, float pos_x, float pos_y, float size_x, float size_y, float split, float tilt, float timer);

extern boolean	seduce_background_shape_draw(BInputState *input, void *id, float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, float d_x, float d_y, float timer, float normal_x, float normal_y, float *center, float transparancy);


//extern boolean	seduce_background_image_draw(BInputState *input, void *id, float pos_x, float pos_y, float pos_z, float size_x, float size_y, float u_start, float v_start, float u_size, float v_size, float timer, float *center, uint texture_id);
extern boolean	seduce_background_shape_matrix_interact(BInputState *input, void *id, float *matrix, boolean scale, boolean rotate);

/* Seduce view and projection code */
/* ------------------------------- */

typedef enum{
	S_VIS_LINEAR,
	S_VIS_EASE_IN,
	S_VIS_EASE_OUT,
	S_VIS_SMOOTH,
	S_VIS_CUT,
	S_VIS_COUNT
}SViewInterpolationStyle;


typedef struct{
	float matrix[16];
	float model[16];
	float target[3];
	float camera[3];
	float distance;
	uint type;
	float speed;
	SViewInterpolationStyle interpolation_style;
	union{
		struct{
			float			target_target[3];
			float			target_camera[3];
			float			target_up[3];
			float			progress;
		}camera;
		struct{
			float			delta_rot[2];
		}orbit;
		struct{
			float			current_rot[2];
			float			delta_rot[2];
			boolean			slide;
		}rotate;
		struct{
			float			delta_pan[2];
		}pan;
		struct{
			float			delta_distance;
		}distance;
		struct{
			float			target_target[3];
			float			target_camera[3];
			float			target_up[3];
			float			progress;
		}scroll;
		struct{
			float			pitch;
			float			pitch_target;
			float			yaw;
			float			yaw_target;
			float			target[3];
			float			position[3];
			float			speed;
			float			distance_target;
			float			grid_size;
			float			direction;
			float			grab[2];
			uint			view_axis;
			uint			axis;
		}right_button;
	}data;
}SViewData;

extern void		seduce_view_init(SViewData *v);
extern void		seduce_view_set(SViewData *v, RMatrix *m);
extern void		seduce_view_update(SViewData *v, float delta);
extern void		seduce_view_interpolation_style_set(SViewData *v, SViewInterpolationStyle style, float speed);

extern void		seduce_view_change_look_at(SViewData *v, float *target, float *camera, float *up);
extern void		seduce_view_change_right_button(SViewData *v, BInputState *input);
extern void		seduce_view_change_mouse_look(SViewData *v, BInputState *input);
extern void		seduce_view_change_multi_touch(SViewData *v, BInputState *input);

extern void		seduce_view_change_axised_orbit(SViewData *v, BInputState *input);
extern void		seduce_view_change_spin_orbit(SViewData *v, BInputState *input);
extern void		seduce_view_change_spin_axised_orbit(SViewData *v, BInputState *input);

extern void		seduce_view_change_orbit(SViewData *v, BInputState *input, uint button, boolean slide);
extern void		seduce_view_change_orbit_delta(SViewData *v, BInputState *input, uint button, boolean slide);
extern void		seduce_view_orbit(SViewData *v, float x, float y);

extern void		seduce_view_change_rotate(SViewData *v, BInputState *input, uint button, boolean slide);
extern void		seduce_view_change_rotate_delta(SViewData *v, BInputState *input, uint button, boolean slide);
extern void		seduce_view_rotate(SViewData *v, float x, float y);

extern void		seduce_view_change_pan(SViewData *v, BInputState *input, uint button, boolean slide);
extern void		seduce_view_change_pan_delta(SViewData *v, BInputState *input, uint button, boolean slide);
extern void		seduce_view_pan(SViewData *v, float x, float y);
extern void		seduce_view_pan_to_set(SViewData *v, float position_x, float position_y, float position_z);

extern void		seduce_view_change_distance(SViewData *v, BInputState *input, uint button, boolean slide);
extern void		seduce_view_change_distance_delta(SViewData *v, BInputState *input, uint button, boolean slide);
extern void		seduce_view_change_distance_scroll(SViewData *v, BInputState *input);
extern void		seduce_view_distance(SViewData *v, float x, float y);
extern void		seduce_view_distance_to_set(SViewData *v, float distance);


extern void		seduce_view_change_distance_scroll_zoom(SViewData *v, BInputState *input);


extern void		seduce_view_center_set(SViewData *v, float position_x, float position_y, float position_z);
extern void		seduce_view_camera_set(SViewData *v, float position_x, float position_y, float position_z);
extern void		seduce_view_up_set(SViewData *v, float up_x, float up_y, float up_z);
extern void		seduce_view_pan_to_set(SViewData *v, float position_x, float position_y, float position_z);
extern void		seduce_view_direction_set(SViewData *v, float normal_x, float normal_y, float normal_z);


extern void		seduce_view_change_zoom(SViewData *v, float delta_x, float delta_y);
extern void		seduce_view_center_getf(SViewData *v, float *center);
extern void		seduce_view_center_getd(SViewData *v, double *center);
extern void		seduce_view_distance_camera_set(SViewData *v, double distance);
extern double	seduce_view_distance_camera_get(SViewData *v);

extern void		seduce_view_camera_getf(SViewData *v, float *camera);
extern void		seduce_view_camera_getd(SViewData *v, double *camera);
extern void		seduce_view_camera_vector_getf(SViewData *v, float *camera, float x, float y);
extern void		seduce_view_camera_vector_getd(SViewData *v, double *camera, double x, double y);
extern void		seduce_view_grid_size_set(SViewData *v, double grid_size);

extern void		seduce_view_model_matrixd(SViewData *v, double *matrix);
extern void		seduce_view_model_matrixf(SViewData *v, float *matrix);
extern void		seduce_view_sprite_matrixf(SViewData *v, float *matrix);

extern void		seduce_view_projectiond(SViewData *v, double *output, double x, double y);
extern void		seduce_view_projectionf(SViewData *v, float *output, float x, float y);
extern void		seduce_view_projection_vertexf(SViewData *v, float *output, float *vertex, float x, float y);
extern void		seduce_view_projection_vertexd(SViewData *v, double *output, double *vertex, double x, double y);
extern void		seduce_view_projection_screend(SViewData *v, double *output, double x, double y, double z);
extern void		seduce_view_projection_screenf(SViewData *v, float *output, float x, float y, float z);
extern double	seduce_view_projection_screen_distanced(SViewData *v, double space_x, double space_y, double space_z, double screen_x, double screen_y);
extern void		seduce_view_projection_planed(SViewData *v, double *dist, uint axis, double pointer_x, double pointer_y , double depth);
extern double	seduce_view_projection_lined(SViewData *v, double *dist, uint axis, double pointer_x, double pointer_y, double *pos);


/* Manipulator */
/* ----------- */

extern boolean	seduce_manipulator_point(BInputState *input, RMatrix *m, float *pos, void *id, float scale);
extern boolean	seduce_manipulator_pos_xyz(BInputState *input, RMatrix *m, float *pos, void *id, float *snap, boolean snap_active, boolean x, boolean y, boolean z, float scale, float time);
extern boolean	seduce_manipulator_point_plane(BInputState *input, RMatrix *m, float *pos, void *id, float *snap, boolean snap_active, uint axis, float scale);
extern boolean	seduce_manipulator_point_axis(BInputState *input, RMatrix *m, float *pos, void *id, float *snap, boolean snap_active, uint axis, float scale);
extern boolean	seduce_manipulator_point_vector(BInputState *input, RMatrix *m, float *pos, void *id, float *snap, boolean snap_active, float *vector, float scale);

extern boolean	seduce_manipulator_normal(BInputState *input, RMatrix *m, float *pos, float *normal, void *id, float scale, float time);
extern boolean	seduce_manipulator_radius(BInputState *input, RMatrix *m, float *pos, float *radius, void *id, float time);
extern boolean	seduce_manipulator_scale(BInputState *input, RMatrix *m, float *pos, float *size, void *id, float *snap, boolean snap_active, boolean x, boolean y, boolean z, float scale, float time);
extern boolean	seduce_manipulator_rotate(BInputState *input, RMatrix *matrix, float *pos, float *rotation_matrix, void *id, float *snap, boolean snap_active, float scale, float time);

extern boolean	seduce_manipulator_square_centered(BInputState *input, RMatrix *m, float *pos, float *size, void *id, float *snap, boolean snap_active, boolean aspect_lock, float scale, float time);
extern boolean	seduce_manipulator_square_cornered(BInputState *input, RMatrix *m, float *down_left, float *up_right, void *id, float *snap, boolean snap_active, boolean aspect_lock, float scale, float time);

extern boolean	seduce_manipulator_cube_centered(BInputState *input, RMatrix *m, float *pos, float *size, void *id, float *snap, boolean snap_active, float scale, float time);
extern boolean	seduce_manipulator_cube_cornered(BInputState *input, RMatrix *m, float *min, float *max, void *id, float *snap, boolean snap_active, float scale, float time);

/* Seduce colission elements */
/* ------------------------- */

extern void seduce_element_add_point(BInputState *input, void *id, uint part, float *pos, float size);
extern void seduce_element_add_line(BInputState *input, void *id, uint part, float *a, float *b, float size);
extern void seduce_element_add_triangle(BInputState *input, void *id, uint part, float *a, float *b, float *c);
extern void seduce_element_add_quad(BInputState *input, void *id, uint part, float *a, float *b, float *c, float *d);
extern void seduce_element_add_surface(BInputState *input, void *id);

extern void *seduce_element_colission_test(float *pos, uint *part, uint user_id);
extern void *seduce_element_pointer_id(BInputState *input, uint pointer, uint *part);
extern uint seduce_element_pointer(BInputState *input, void *id, uint *part);
extern void *seduce_element_selected_id(uint user_id, float *pos, uint *part);
extern uint seduce_element_primary_axis(BInputState *input, uint user_id);
extern boolean seduce_element_surface_project(BInputState *input, void *id, float *output, float pointer_x, float pointer_y);
extern boolean seduce_element_active(BInputState *input, void *id, void *part);

extern void seduce_element_popup_action_begin(BInputState *input, uint user_id);
extern void seduce_element_popup_action_end(BInputState *input);
extern void *seduce_element_popup_action(boolean active);
#endif
