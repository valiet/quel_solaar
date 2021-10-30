
extern void		deceve_add_login(char *address, char *name, char *pass);
extern void		deceive_set_arg(int argc, char **argv);
extern ENode	*deceive_get_edit_node(void);

extern void		deceive_save_bookmarks(char *file_name);
extern void		deceive_load_bookmarks(char *file_name);

extern void		deceive_set_intro_draw_func(void (*draw_func)(void *user_pointer), void *user_pointer);
extern void		deceive_intro_handler(BInputState *input, void *application_handler_func);
