
typedef enum{
	S_EWT_TEXT_LINE_DRAW,
	S_EWT_TEXT_LINE_EDIT,
	S_EWT_TEXT_PASSWORD_EDIT,
	S_EWT_TEXT_DOUBLE_EDIT,
	S_EWT_TEXT_FLOAT_EDIT,
	S_EWT_TEXT_INT_EDIT,
	S_EWT_TEXT_UINT_EDIT,
	S_EWT_TEXT_BLOCK_DRAW,
	S_EWT_TEXT_BUTTON,
	S_EWT_TEXT_BUTTON_LIST,
	S_EWT_WIDGET_ICON_BUTTON,
	S_EWT_WIDGET_ICON_TOGGLE,
	S_EWT_WIDGET_ICON_POPUP,
	S_EWT_WIDGET_SLIDER_RADIAL,
	S_EWT_WIDGET_WHEEL_RADIAL,
	S_EWT_WIDGET_SELECT_RADIAL,
	S_EWT_BACKGROUND_SQUARE,
	S_EWT_BACKGROUND_SHAPE,
	S_EWT_BACKGROUND_IMAGE,
	S_EWT_COUNT
}SEditorWidgetType;

#define SE_POS_COUNT 4

typedef struct{
	uint type;
	char name[32];
	float pos[2 * SE_POS_COUNT];
	char comment[1024];
	union{
		struct{
			float size;
			float spacing;
			float center;
			float color[8];
		}text;
		struct{
			uint icon;
			float scale;
			boolean color_active;
			float color[4];
		}icon;
		struct{
			float split;
		}window;
		struct{
			float normal[2];
			float transparancy;
		}surface;
		struct{
			float u_start;
			float v_start;
			float u_end;
			float v_end;
		}image;
		struct{
			char *array[32];
			uint array_length;
			float scale;
		}options;
	}data;
}SEditorWidget;


extern SEditorWidget *s_editor_widgets;
extern uint s_editor_widget_count;

extern uint s_editor_widget_add(SEditorWidgetType type, float x, float y);
extern void seduce_editor_list(BInputState *input, float time, float scale);
extern void seduce_editor_panel_draw(BInputState *input, SEditorWidget *w);
extern void seduce_editor_save_function(FILE *f, char *name);
extern void seduce_editor_save_data(char *file_name);
extern void seduce_editor_load_data(char *file_name);
