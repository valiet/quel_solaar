#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"


#define SEDUCE_3D_GEN

#include "verse.h"
#include "enough.h"
#include "vetk.h"

#ifdef SEDUCE_3D_GEN

#include "seduce.h"

float *sui_load_targa(char *file_name, uint *x_size, uint *y_size)
{
	FILE *image;
	float *draw = NULL;
	unsigned int i, j, identfeald, type, alpha;

	if((image = fopen(file_name, "rb")) == NULL)
	{
		printf("Warning: cannot open file: \"%s\"\n", file_name);
		return FALSE;
	}

	identfeald = fgetc(image);
	if(0 != fgetc(image))
	{
		printf("Error: File %s a non suported palet image\n", file_name);
		return FALSE;
	}
	type = fgetc(image);
	if(2 != type) /* type must be 2 uncompressed RGB */
	{
		printf("Error: File %s is not a uncompressed RGB image\n", file_name);
		return FALSE;
	}
	for(i = 3; i < 12; i++)
		fgetc(image); /* ignore some stuff */
	*x_size = fgetc(image);
	*x_size += 256 * fgetc(image);
	*y_size = fgetc(image);
	*y_size += 256 * fgetc(image);

	alpha = fgetc(image);

	if(alpha != 24 && alpha != 32) /* type must be 24 or 32 bits RGB */
	{
		printf("Error: File %s is not a 24 or 32 bit RGB image\n", file_name);
		return FALSE;
	}

	for(i = 0; i < identfeald; i++)
		fgetc(image); /* ignore some stuff */

	draw = malloc((sizeof *draw) * *x_size * *y_size * 3);
//	fgetc(image);
	if(alpha == 32)
	{
		for(i = 0; i < *x_size * *y_size * 3; i += 3)
		{
			fgetc(image); /* ignore alpha */
			draw[i + 2] = (float)fgetc(image) / (float)255.0;
			draw[i + 1] = (float)fgetc(image) / (float)255.0;
			draw[i + 0] = (float)fgetc(image) / (float)255.0;
		}
	}
	else for(i = 0; i < *x_size * *y_size * 3; i += 3)
	{			
		draw[i] = (float)fgetc(image) / (float)255.0;
		draw[i + 2] = (float)fgetc(image) / (float)255.0;
		draw[i + 1] = (float)fgetc(image) / (float)255.0;
	}
	fclose(image);
	return draw;
}

void sui_3d_make(/*char *name*/)
{
	ENode *node;
	EGeoLayer *layer;
	uint i, tri_count = 0, mesh_count = 0, x_size, y_size, image_size[4];
	float *texture;
	VETKQuadTri *mesh;
	FILE *file;
	return;
	enough_init();
	e_vc_set_auto_subscribe(V_NT_GEOMETRY, TRUE);
	e_vc_connect("localhost", "name", "pass", NULL);


	for(i = 0; i < 5000; i++)
	{
		e_vc_connection_update(0, 1000);
		for(node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
			for(layer = e_nsg_get_layer_next(node, 0); layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
				e_nsg_get_layer_data(node, layer);
	}

	file = fopen("s_draw_3d_data.h", "w"); 
	fprintf(file, "void *r_3d_array = NULL;\n\n");

	texture = sui_load_targa("sphere2.tga", &x_size, &y_size);
	image_size[0] = x_size;
	image_size[1] = y_size;
	fprintf(file, "uint8 seduce_object_3d_texture_shade_data[%u] = {0, 0, ", x_size * y_size * 3 + 2);
	for(i = 0; i < x_size * y_size * 3 - 1; i++)
	{
		if(i % 128 == 128 - 1)
			fprintf(file, "\n\t\t\t\t\t\t");
		fprintf(file, "%u, ", (uint8)(texture[i] * 255.0));
	}
	fprintf(file, "%u};\n\n", (uint8)(texture[i] * 255.0));
	free(texture);

/*	texture = sui_load_targa("reflection2.tga", &x_size, &y_size);
	image_size[2] = x_size;
	image_size[3] = y_size;
	fprintf(file, "uint8 seduce_object_3d_texture_reflection_data[%u] = {", x_size * y_size * 3);
	for(i = 0; i < x_size * y_size * 3 - 1; i++)
	{
		if(i % 128 == 128 - 1)
			fprintf(file, "\n\t\t\t\t\t\t");
		fprintf(file, "%u, ", (uint8)(texture[i] * 255.0));
	}
	fprintf(file, "%u};\n\n", (uint8)(texture[i] * 255.0));
	free(texture);*/

	for(node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
	{
		char *vertex_params[7] = {VETK_VP_VERTEX_X, VETK_VP_VERTEX_Y, VETK_VP_VERTEX_Z, VETK_VP_NORMAL_X, VETK_VP_NORMAL_Y, VETK_VP_NORMAL_Z, "material"};
		
		printf("Name: %s\n", e_ns_get_node_name(node));
		mesh = vetk_create_mesh(node, vertex_params, 7, FALSE, FALSE);
		if(mesh != 0)
		{
			uint16 group_id, tag_id;
			VNTag *tag;
			mesh_count++;
			tri_count += mesh->tri_length;
			printf("Length: %u\n", mesh->tri_length);
			fprintf(file, "float sui_3d_data_%s[%u] = {", e_ns_get_node_name(node), mesh->tri_length * 7);


			tag = e_ns_get_tag_by_type(node, "flat", VN_TAG_BOOLEAN, &group_id, &tag_id);

			if(tag == NULL || !tag->vboolean)
			{
				for(i = 0; i < mesh->tri_length; i++)
				{
					mesh->vertex[i * 7 + 3] += mesh->vertex[i * 7 + 0];
					mesh->vertex[i * 7 + 4] += mesh->vertex[i * 7 + 1];
					mesh->vertex[i * 7 + 5] += mesh->vertex[i * 7 + 2];
				}
			}


			for(i = 0; i < mesh->tri_length * 7 - 1; i++)
			{
				if(i % (8 * 7) == (8 * 7) -1 )
					fprintf(file, "\n\t\t\t\t\t\t");
				if(mesh->vertex[i] != mesh->vertex[i] || mesh->vertex[i] > 1000 || mesh->vertex[i] < -1000)
					fprintf(file, "1.0f, ");
				else
					fprintf(file, "%ff, ", mesh->vertex[i]);
			}
			if(mesh->vertex[i] != mesh->vertex[i] || mesh->vertex[i] > 1000 || mesh->vertex[i] < -1000)
				fprintf(file, "1.0f};\n\n");
			else
				fprintf(file, "%ff};\n\n", mesh->vertex[i]);
		}
		e_ns_set_custom_data(node, 0, mesh);
	}
	fprintf(file, "\nvoid *seduce_object_3d_sections[%u];\n\n", mesh_count);

	fprintf(file, "\n#define SEDUCE_DRAW_3D_COUNT %u\n\n", mesh_count);
	fprintf(file, "\nfloat *seduce_3d_object_array[%u] = {", mesh_count + 1);
	for(node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
	{
		if(e_ns_get_custom_data(node, 0) != NULL)
		{
			fprintf(file, "\n\t\t\t\tsui_3d_data_%s,", e_ns_get_node_name(node));
		}
	}
	fprintf(file, " 0};\n\n", e_ns_get_node_name(node));

	fprintf(file, "\nuint seduce_3d_object_size[%u] = {", mesh_count + 1);
	for(node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
	{
		mesh = e_ns_get_custom_data(node, 0);
		if(mesh != NULL)
		{
			fprintf(file, "%u, ", mesh->tri_length);
		}
	}
	fprintf(file, "0};\n\n", e_ns_get_node_name(node));
	fclose(file);

	file = fopen("s_draw_3d.h", "w"); 

	fprintf(file, "\ntypedef enum{\n");
	for(node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
	{
		if(e_ns_get_custom_data(node, 0) != NULL)
		{
			char text[1024];
			sprintf(text, "SUI_3D_OBJECT_%s", e_ns_get_node_name(node));
			for(i = 0; i < 1023 && text[i] != 0; i++)
				if(text[i] >= 65 + 32 && text[i] <= 90 + 32)
					text[i] -= 32;
			fprintf(file, "\t%s,\n", text);
		}
	}
	fprintf(file, "\tSUI_3D_OBJECT_COUNT\n");
	fprintf(file, "}SUI3DObjects;\n\n");
	fclose(file);
	exit(0);
}

#endif
