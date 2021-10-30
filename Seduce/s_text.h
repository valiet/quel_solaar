/* text line draw */
/*
extern void		seduce_text_line_draw(float size, float spacing, const char *text, uint length);
extern float	seduce_text_line_length(float size, float spacing, const char *text, uint end);*/
extern uint		seduce_text_line_hit_test(float letter_size, float letter_spacing, const char *text, float pos_x);
/*
extern void		sui_text_line_cursur_draw(float pos_x, float pos_y, float size, float curser_x, float curser_y);
extern void		sui_text_line_selection_draw(float pos_x, float pos_y, float size, float curser_start, float curser_end, float curser_y);
extern void		sui_text_line_edit_draw(float pos_x, float pos_y, float size, float spacing, float length, char *text, uint *scroll_start, uint select_start, uint select_end);
extern void		sui_text_line_edit_mouse(float size, float spacing, float length, char *text, uint *scroll_start, uint *select_start, uint *select_end, boolean mouse_button, boolean mouse_button_last, float pointer_x);
*/
/* text box draw */
/*
extern void		seduce_text_block_draw(float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, const char *text);
extern uint		sui_text_block_line_end(const char *text, float spacing, float letter_size, float size);
extern uint		seduce_text_block_hit_test(float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, const char *text, float pos_x, float pos_y);
extern void		sui_text_block_pos(float letter_size, float letter_spacing, float line_size, float line_spacing, const char *text, uint curser, float *pos_x, float *pos_y);

extern void		sui_text_box_edit_draw(float size, float spacing, float line_spacing, float length, char *text, uint lines, uint *line_start, uint select_start, uint select_end, boolean draw_curser);
extern void		sui_text_box_edit_mouse(float size, float spacing, float line_spacing, float length, char *text, uint lines, uint *scroll_start, uint *select_start, uint *select_end, boolean mouse_button, boolean mouse_button_last, float pointer_x, float pointer_y);
*/
/* utilities */
/*
extern boolean  sui_util_text_insert_character(char *text, uint size, uint curser_start, uint curser_end, char *insert);
*/
/* text edit */
/*
extern void		sui_text_edit_forward(char *text, uint *select_start, uint *select_end);
extern void		sui_text_edit_back(char *text, uint *select_start, uint *select_end);
extern void		sui_text_edit_delete(char *text, uint length, uint *select_start, uint *select_end);
extern void		sui_text_edit_end(char *text, uint length, uint *select_start, uint *select_end);
extern void		sui_text_edit_home(char *text, uint length, uint *select_start, uint *select_end);
extern void		sui_text_edit_backspace(char *text, uint length, uint *select_start, uint *select_end);
extern void		sui_text_edit_insert_character(char *text, uint length, uint *select_start, uint *select_end, char character);

*/

