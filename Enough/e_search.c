#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"

boolean e_text_search(const char *text, const char *search)
{
	uint i, j;
	for(i = 0; text[i] != 0; i++)
	{
		if(text[i] == search[0] || search[0] + 32 == text[i] || search[0] - 32 == text[i])
		{
			for(j = 0; text[i + j] != 0 && search[j] != 0 && (text[i + j] == search[j] || (text[i + j] > 64 && text[i + j] < 91 && text[i + j] + 32 == search[j]) || (search[j] > 64 && search[j] < 91 && search[j] + 32 == text[i + j])); j++);
			if(search[j] == 0)
				return TRUE;
		}
	}
	for(i = 0; text[i] != 0; i++)
	{
		if(text[i] == search[0])
		{
			for(j = 0; text[i + j] != 0 && search[j] != 0 && (text[i + j] == search[j]); j++);
			if(search[j] == 0)
				return TRUE;
		}
	}
	return FALSE;
}

boolean e_search_node(ENode *node, char *search)
{
	uint16 i, j;
	if(search[0] == 0)
		return TRUE;

	if(e_text_search(e_ns_get_node_name(node), search))
		return TRUE;

	for(i = e_ns_get_next_tag_group(node, 0); i != (uint16)-1; i = e_ns_get_next_tag_group(node, i + 1))
	{
		if(e_text_search(e_ns_get_tag_group(node, i), search))
			return TRUE;
		for(j = e_ns_get_next_tag(node, i, 0); j != (uint16)-1; j = e_ns_get_next_tag(node, i, j + 1))
		{
			if(e_text_search(e_ns_get_tag_name(node, i, j), search))
				return TRUE;
			if(VN_TAG_STRING == e_ns_get_tag_type(node, i, j))
			{
				VNTag *tag;
				tag = e_ns_get_tag(node, i, j);
				if(e_text_search(tag->vstring, search))
					return TRUE;
			}
		}
	}
	switch(e_ns_get_node_type(node))
	{
		case V_NT_OBJECT :
			{
				double light[3];
				EObjLink *link;
				uint i, j, k;
				char **names;
				for(link = e_nso_get_next_link(node, 0); link != NULL && link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
					if(e_text_search(e_nso_get_link_name(link), search))
						return TRUE;

				for(i = e_nso_get_next_method_group(node, 0); i != (uint16)-1 ; i = e_nso_get_next_method_group(node, i + 1))
				{
					if(e_text_search(e_nso_get_method_group(node, i), search))
						return TRUE;
					for(j = e_nso_get_next_method(node, i, 0); j != (uint16)-1; j = e_nso_get_next_method(node, i, j + 1))
					{
						if(e_text_search(e_nso_get_method(node, i, j), search))
							return TRUE;
						names = e_nso_get_method_param_names(node, i, j);
						for(k = 0; k < e_nso_get_method_param_count(node, i, j); k++)
							if(e_text_search(names[k], search))
								return TRUE;
					}
				}
				e_nso_get_light(node, light);
				if(light[0] + light[0] + light[0] > 0.0001)
					if(e_text_search("light", search))
						return TRUE;
			}
		break;
		case V_NT_GEOMETRY :
			{
				EGeoLayer *layer;
				for(layer = e_nsg_get_layer_next(node, 0); layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
					if(e_text_search(e_nsg_get_layer_name(layer), search))
						return TRUE;
				if(e_text_search(e_nsg_get_layer_crease_vertex_name(node), search))
					return TRUE;
				if(e_text_search(e_nsg_get_layer_crease_edge_name(node), search))
					return TRUE;
			}
		break;
		case V_NT_MATERIAL :
			{
				for(i = e_nsm_get_fragment_next(node, 0); i != (VNMFragmentID)-1; i = e_nsm_get_fragment_next(node, i + 1))
				{
					VNMFragmentType type;
					VMatFrag *frag;
					char *frag_type[VN_M_FT_OUTPUT + 1] = {"VN_M_FT_COLOR", "VN_M_FT_LIGHT", "VN_M_FT_REFLECTION", "VN_M_FT_TRANSPARENCY", "VN_M_FT_VOLUME", "VN_M_FT_GEOMETRY", "VN_M_FT_TEXTURE", "VN_M_FT_NOISE", "VN_M_FT_BLENDER", "VN_M_FT_MATRIX", "VN_M_FT_RAMP", "VN_M_FT_ANIMATION", "VN_M_FT_ALTERNATIVE", "VN_M_FT_OUTPUT"};
			
					frag = e_nsm_get_fragment(node, i);
					type = e_nsm_get_fragment_type(node, i);
					if(e_text_search(frag_type[type], search))
						return TRUE;
					switch(type)
					{	
						case VN_M_FT_COLOR :
							break;
						case VN_M_FT_LIGHT :
							if(e_text_search(frag->light.brdf_r, search))
								return TRUE;
							if(e_text_search(frag->light.brdf_g, search))
								return TRUE;
							if(e_text_search(frag->light.brdf_b, search))
								return TRUE;
							break;
						case VN_M_FT_REFLECTION :
							break;
						case VN_M_FT_TRANSPARENCY :
							break;
						case VN_M_FT_VOLUME :
							break;
						case VN_M_FT_GEOMETRY :
							if(e_text_search(frag->geometry.layer_r, search))
								return TRUE;
							if(e_text_search(frag->geometry.layer_g, search))
								return TRUE;
							if(e_text_search(frag->geometry.layer_b, search))
								return TRUE;
							break;
						case VN_M_FT_TEXTURE :
							if(e_text_search(frag->texture.layer_r, search))
								return TRUE;
							if(e_text_search(frag->texture.layer_g, search))
								return TRUE;
							if(e_text_search(frag->texture.layer_b, search))
								return TRUE;
							break;
						case VN_M_FT_NOISE :
							break;
						case VN_M_FT_BLENDER :
							{
								char *blend_modes[7] = {"VN_M_BLEND_FADE", "VN_M_BLEND_ADD", "VN_M_BLEND_SUBTRACT", "VN_M_BLEND_MULTIPLY", "VN_M_BLEND_DIVIDE", "VN_M_BLEND_DOT"};
								if(e_text_search(blend_modes[frag->blender.type], search))
									return TRUE;
							}
							break;
						case VN_M_FT_MATRIX :
							break;
						case VN_M_FT_RAMP :
							break;
						case VN_M_FT_ANIMATION :
						case VN_M_FT_ALTERNATIVE :
							break;
						case VN_M_FT_OUTPUT :
							if(e_text_search(frag->output.label, search))
								return TRUE;
							break;
					}
				}
			}
		break;
		case V_NT_BITMAP :
			{
				EGeoLayer *layer;
				for(layer = e_nsb_get_layer_next(node, 0); layer != NULL; layer = e_nsb_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
					if(e_text_search(e_nsb_get_layer_name(layer), search))
						return TRUE;
			}
		break;
		case V_NT_TEXT :
			{
				ETextBuffer *buffer;
				if(e_text_search(e_nst_get_language(node), search))
					return TRUE;
				for(buffer = e_nst_get_buffer_next(node, 0); buffer != NULL; buffer = e_nst_get_buffer_next(node, e_nst_get_buffer_id(buffer) + 1))
				{
					if(e_text_search(e_nst_get_buffer_name(buffer), search))
						return TRUE;
					if(e_text_search(e_nst_get_buffer_data(node, buffer), search))
						return TRUE;
				}
			}
		break;
		case V_NT_CURVE :
			{
				ECurve *curve;
				for(curve = e_nsc_get_curve_next(node, 0); curve != NULL; curve = e_nsc_get_curve_next(node, e_nsc_get_curve_id(curve) + 1))
					if(e_text_search(e_nsc_get_curve_name(curve), search))
						return TRUE;
			}
		break;
		case V_NT_AUDIO :
			{
				EAudioBuffer *buffer;
				EAudioStream *stream;
				for(buffer = e_nsa_get_buffer_next(node, 0); buffer != NULL; buffer = e_nsa_get_buffer_next(node, e_nsa_get_buffer_id(buffer) + 1))
					if(e_text_search(e_nsa_get_buffer_name(buffer), search))
						return TRUE;
				for(stream = e_nsa_get_stream_next(node, 0); stream != NULL; stream = e_nsa_get_stream_next(node, e_nsa_get_buffer_id(stream) + 1))
					if(e_text_search(e_nsa_get_stream_name(stream), search))
						return TRUE;
			}
		break;
	}

	return FALSE;
}
