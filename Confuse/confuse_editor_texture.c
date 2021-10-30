#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "c_internal.h"
#include "betray.h"
#include "seduce.h"

extern uint c_texture_count;
extern CTexture *c_textures;
extern uint (*c_load_image_2d_func)(char *name, uint8 *data, uint x, uint y, void *user);
extern void *c_load_image_2d_user;
extern uint (*c_load_image_3d_func)(char *name, uint8 *data, uint x, uint y, uint z, void *user);
extern void *c_load_image_3d_user;
extern uint (*c_load_image_cube_func)(char *name, uint8 *data, uint x, uint y, void *user);
extern void *c_load_image_cube_user;


boolean c_load_texture_targa(char *file_name)
{
	CTexture *t;
	FILE *image;
	uint8 *draw = NULL;
	unsigned int i, j, identfeald, type, size, size_x, size_y, alpha;



	if((image = fopen(file_name, "rb")) == NULL)
	{
		printf("Gather Warning: cannot open file: \"%s\"\n", file_name);
		return FALSE;
	}

	identfeald = fgetc(image);
	if(0 != fgetc(image))
	{
		printf("Gather Error: File %s a non suported palet image\n", file_name);
		return FALSE;
	}
	type = fgetc(image);
	if(2 != type) /* type must be 2 uncompressed RGB */
	{
		printf("Gather Error: File %s is not a uncompressed RGB image\n", file_name);
		return FALSE;
	}
	for(i = 3; i < 12; i++)
		fgetc(image); /* ignore some stuff */
	size_x = fgetc(image);
	size_x += 256 * fgetc(image);
	size_y = fgetc(image);
	size_y += 256 * fgetc(image);

	size = size_x * size_y;

	alpha = fgetc(image);

	if(alpha != 24 && alpha != 32) /* type must be 24 or 32 bits RGB */
	{
		printf("Gather Error: File %s is not a 24 or 32 bit RGB image\n", file_name);
		return FALSE;
	}


	for(i = 0; i < identfeald; i++)
		fgetc(image); /* ignore some stuff */
	fgetc(image);
	if(alpha == 32)
	{
		draw = malloc((sizeof *draw) * size * 4);
		for(i = 0; i < size * 3; i += 3)
		{
			draw[i + 3] = fgetc(image); /* ignore alpha */
			draw[i + 2] = fgetc(image);
			draw[i + 1] = fgetc(image);
			draw[i + 0] = fgetc(image);
		}


	}
	else
	{
		draw = malloc((sizeof *draw) * size * 4);
		for(i = 0; i < size * 4; i += 4)
		{
			draw[i + 2] = fgetc(image);
			draw[i + 1] = fgetc(image);
			draw[i + 0] = fgetc(image);
			draw[i + 3] = 0;
		}

	}
	fclose(image);



	if(c_texture_count % 16 == 0)
	{
		c_textures = realloc(c_textures, (sizeof *c_textures) * (c_texture_count + 16));
	}
	t  = &c_textures[c_texture_count++];

	if(c_load_image_2d_func != NULL)
		t->texture_id = c_load_image_2d_func(file_name, draw, size_x, size_y, c_load_image_2d_user);
	else
		t->texture_id = -1;

	for(i = j = 0; file_name[i] != 0; i++)
		if(file_name[i] == '/' || file_name[i] == '\\')
			j = i + 1;
	
	for(i = 0; file_name[i + j] != 0 && file_name[i + j] != '.' && i < 31; i++)
			t->name[i] = file_name[i + j];
	t->name[i] = 0;
	t->buffer = draw;
	t->size[0] = size_x;
	t->size[1] = size_y;
	t->size[2] = 1;
	t->type = C_TYPE_IMAGE_2D;
	return TRUE;
}



boolean c_load_texture(char *file_name)
{
	CTexture *t;
	FILE *image;
	uint8 *draw = NULL;
	unsigned int i, j, identfeald, type, size, size_x, size_y, alpha;
	boolean *hdr;

	draw = gather_image_load(file_name, &size_x, &size_y, &hdr);
	if(draw == NULL)
		return FALSE;
	if(c_texture_count % 16 == 0)
	{
		c_textures = realloc(c_textures, (sizeof *c_textures) * (c_texture_count + 16));
	}
	t  = &c_textures[c_texture_count++];

	if(c_load_image_2d_func != NULL)
		t->texture_id = c_load_image_2d_func(file_name, draw, size_x, size_y, c_load_image_2d_user);
	else
		t->texture_id = -1;

	for(i = j = 0; file_name[i] != 0; i++)
		if(file_name[i] == '/' || file_name[i] == '\\')
			j = i + 1;
	
	for(i = 0; file_name[i + j] != 0 && file_name[i + j] != '.' && i < 31; i++)
			t->name[i] = file_name[i + j];
	t->name[i] = 0;
	t->buffer = draw;
	t->size[0] = size_x;
	t->size[1] = size_y;
	t->size[2] = 1;
	t->type = C_TYPE_IMAGE_2D;
	return TRUE;
}

void c_editor_texture_select(BInputState *input)
{
	static boolean init = FALSE;
	uint i;
	if(!init)
	{
	/*	for(i = 0; i < 1; i++)
		{
			c_load_texture("bumpmap3.tga");
			c_load_texture("Con_Dotita_Raiders.tga");
			c_load_texture("icons3.tga");
			c_load_texture("jagged.tga");
			c_load_texture("line_trans.tga");
			c_load_texture("mist.tga");
			c_load_texture("noise1.tga");
			c_load_texture("noise2.tga");
			c_load_texture("pattern4.tga");
		
			c_load_texture("sprites.tga");
			c_load_texture("strokes4.tga");
			c_load_texture("water.tga");
		}
		c_load_texture("halo.tga");
		c_load_texture("text.tga");
		c_load_texture("sprites.tga");
		c_load_texture("perlin.tga");
		c_load_texture("water.tga");
		c_load_texture("eye_all.tga");
		c_load_texture("surface1.tga");
		c_load_texture("surface2.tga");
		c_load_texture("surface3.tga");
		c_load_texture("surface4.tga");
		
		c_load_texture("milkyway.tga");
		c_load_texture("nebula1.tga");
		c_load_texture("nebula2.tga");
		c_load_texture("nebula3.tga");
		c_load_texture("nebula4.tga");
		c_load_texture("planet1.tga");
		c_load_texture("planet2.tga");
		c_load_texture("planet3.tga");
		c_load_texture("planet4.tga");*/

/*		c_load_texture("sprites.tga");
		c_load_texture("perlin.tga");	
		c_load_texture("fire.tga");		
		c_load_texture("body_one.tga");	
		c_load_texture("Pablo.tga");	
		c_load_texture("shift.tga");	
		c_load_texture("morph_line.tga");
		c_load_texture("thin_line_shape.tga");
		c_load_texture("thin_line_color2.tga");
		c_load_texture("watercolor_2.tga");
		c_load_texture("future_a.tga");
		c_load_texture("splats.tga");
		c_load_texture("lines_trans.tga");*/
		init = TRUE;
	}

}