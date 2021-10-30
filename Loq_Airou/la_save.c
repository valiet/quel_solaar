#include "la_includes.h"

#include "la_geometry_undo.h"
#include "la_projection.h"
#include "la_draw_overlay.h"
#include "la_tool.h"
#include "la_pop_up.h"


uint geometry_read_uint(char *string, uint *output)
{
	uint i;
	*output = 0;
	for(i = 0; string[i] >= 48 && string[i] <= 58; i++)
		*output = (string[i] - 48) + (*output) * 10;
	return i;
}

void la_geometry_load_obj(char *file)
{
	FILE *obj;
	char line[512];
	ENode *node;
	EGeoLayer *layer;
	uint i, poly_id, ref_length = 0, vertex_length = 1, uv_length = 1, count, ref[5], uv_ref[5], *vertex_ids, poly_i, uv_layer_ids[2] = {2, 3};
	float x, y, z, *uv_array;

	obj = fopen(file, "r");
	if(obj != NULL)
	{
		for(i = 0; i < 512; i++)
			line[i] = 0;
		while((fgets(line, sizeof line, obj)) != NULL)
		{
			if(line[0] == 'v')
			{
				if(line[1] == 't')
				{
					if(sscanf(line, "vt %f %f", &x, &y) == 2)
						uv_length++;
				}else
					if(sscanf(line, "v %f %f %f", &x, &y, &z) == 3)
						vertex_length++;
			}
		}
		vertex_ids = malloc((sizeof *vertex_ids) * vertex_length);
		uv_array = malloc((sizeof *uv_array) * uv_length * 2);
		for(i = 0; i < vertex_length; i++)
			vertex_ids[i] = udg_find_empty_slot_vertex();
		rewind(obj); 

		if(uv_length > 0)
		{
			node = e_ns_get_node(0, udg_get_modeling_node());

			if(node != NULL)
			{

				if(e_nsg_get_layer_by_name(node, "map_u") == NULL || e_nsg_get_layer_by_name(node, "map_v") == NULL)
				{
					if(e_nsg_get_layer_by_name(node, "map_u") == NULL)
						verse_send_g_layer_create(udg_get_modeling_node(), -1, "map_u", VN_G_LAYER_POLYGON_CORNER_REAL, 0, 0);
					if(e_nsg_get_layer_by_name(node, "map_v") == NULL)
						verse_send_g_layer_create(udg_get_modeling_node(), -1, "map_v", VN_G_LAYER_POLYGON_CORNER_REAL, 0, 0);
					while(e_nsg_get_layer_by_name(node, "map_u") == NULL || 
							e_nsg_get_layer_by_name(node, "map_v") == NULL)
							verse_callback_update(1000);
					
				}
				uv_layer_ids[0] = e_nsg_get_layer_id(e_nsg_get_layer_by_name(node, "map_u"));
				uv_layer_ids[1] = e_nsg_get_layer_id(e_nsg_get_layer_by_name(node, "map_v"));
			}
		}
		uv_length = 0;
		i = 0;
		while((fgets(line, sizeof line, obj)) != NULL)
		{
			if(line[0] == 'v')
			{
				if(line[1] == 't')
				{
					count = sscanf(line, "vt %f %f ", &uv_array[uv_length * 2], &uv_array[uv_length * 2 + 1]);
					if(count == 2)
						uv_length++;
				}else
				{
					count = sscanf(line, "v %f %f %f", &x, &y, &z);
					if(count == 3)
						udg_vertex_set(vertex_ids[i++], NULL, x * 0.1, y * 0.1, z * 0.1);
				}
			}else if(line[0] == 'f') /* f */
			{
				uint pos = 2, add, value, corner = 0, on = 0;
				uv_ref[0] = uv_ref[1] = uv_ref[2] = uv_ref[3] = 0;
				while(line[pos] == ' ')
					pos++;
				while(line[pos] != 0 && corner < 5)
				{
					add = geometry_read_uint(&line[pos], &value);
					if(add != 0)
					{
						if(on == 0)
							ref[corner] = value - 1;
						else if(on == 1)
							uv_ref[corner] = value - 1;
					}
					pos += add;
					if(line[pos] != 0)
					{
						if(line[pos] == '/')
							on++;
						if(line[pos] == ' ')
						{
							on = 0;
							corner++;
						}
					}
					pos++;
				}
				if(corner == 4 || corner == 3)
				{
					poly_id = udg_find_empty_slot_polygon();
					if(corner == 4)
					{
						printf("Corner 4 %u %u %u %u\n", ref[0], ref[1], ref[2], ref[3]);
						udg_polygon_set(poly_id, vertex_ids[ref[0]], vertex_ids[ref[1]], vertex_ids[ref[2]], vertex_ids[ref[3]]);
					}else
					{
						printf("Corner 3 %u %u %u %u\n", ref[0], ref[1], ref[2], ref[3]);
						udg_polygon_set(poly_id, vertex_ids[ref[0]], vertex_ids[ref[1]], vertex_ids[ref[2]], -1);
					}
					verse_send_g_polygon_set_corner_real64(udg_get_modeling_node(), uv_layer_ids[0], poly_id, uv_array[uv_ref[0] * 2 + 0], uv_array[uv_ref[1] * 2 + 0], uv_array[uv_ref[2] * 2 + 0], uv_array[uv_ref[3] * 2 + 0]);
					verse_send_g_polygon_set_corner_real64(udg_get_modeling_node(), uv_layer_ids[1], poly_id, uv_array[uv_ref[0] * 2 + 1], uv_array[uv_ref[1] * 2 + 1], uv_array[uv_ref[2] * 2 + 1], uv_array[uv_ref[3] * 2 + 1]);
				}
			}
		}
		fclose(obj);
	}
}

void la_geometry_save_obj(char *file)
{
	FILE *obj;
	double *vertex;
	uint *ref, ref_length, vertex_length, i, j, *count;
	udg_get_geometry(&vertex_length, &ref_length, &vertex, &ref, NULL);

	obj = fopen(file, "wt");
	if(obj != NULL)
	{
		count = malloc((sizeof *count) * vertex_length);
		for(i = 0; i < vertex_length; i++)
			count[i] = 0;
		for(i = 0; i < ref_length ; i++)
		{
			if(ref[i * 4] < vertex_length && ref[i * 4 + 1] < vertex_length && ref[i * 4 + 2] < vertex_length &&
				vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
			{
				count[ref[i * 4]]++;
				count[ref[i * 4 + 1]]++;
				count[ref[i * 4 + 2]]++;
				if(ref[i * 4 + 3] < vertex_length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					count[ref[i * 4 + 3]]++;
			}
		}
		fprintf(obj, "# Object outputed from Loq Airou by Eskil Steenberg\n\n");
		fprintf(obj, "g\n\n");
		for(i = 0; i < vertex_length ; i++)
			if(count[i] != 0)
				fprintf(obj, "v %f %f %f\n", vertex[i * 3], vertex[i * 3 + 1], vertex[i * 3 + 2]);

		j = 0;
		for(i = 0; i < vertex_length; i++)
		{
			if(    count[i] == 0)
				count[i] = j;
			else
				count[i] = j++;
		}
		fprintf(obj, "\ng SDS_Cage\n\n");
		for(i = 0; i < ref_length ; i++)
		{
			if(ref[i * 4] < vertex_length && ref[i * 4 + 1] < vertex_length && ref[i * 4 + 2] < vertex_length)
			{
				if(ref[i * 4 + 3] < vertex_length)
					fprintf(obj, "f %i %i %i %i\n", count[ref[i * 4]] + 1, count[ref[i * 4 + 1]] + 1, count[ref[i * 4 + 2]] + 1, count[ref[i * 4 + 3]] + 1);
				else
					fprintf(obj, "f %i %i %i\n", count[ref[i * 4]] + 1, count[ref[i * 4 + 1]] + 1, count[ref[i * 4 + 2]] + 1);
			}
		}
		fprintf(obj, "\ng\n\n");
		fclose(obj);
	}
}
/*
#include "gather.h"

GatherData *la_save_data = NULL;

void geometry_save_obj(void *user, char *file)
{
	la_save_data = gather_load("halon.obj", G_FORMAT_WAVEFRONT_OBJ);
}

extern void udg_create_func(uint connection, uint id, VNodeType type, void *user);

void geometry_load_update()
{
	if(la_save_data != NULL)
	{
		if(gather_node_save_verse_geometry(la_save_data))
		{
			gather_free(la_save_data);
			la_save_data = NULL;
			e_ns_set_node_create_func(udg_create_func, NULL);
		}
	}
}*/