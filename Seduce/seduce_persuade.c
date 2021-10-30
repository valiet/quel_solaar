
#include <math.h>

#include "forge.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"

void sp_settings_pre(SUIViewElement *element)
{
/*	char text[32] = {0};
	element[0].type = S_VET_SPLIT;
	element[0].text = "geometry";

	element[1].type = S_VET_INTEGER;
	element[1].text = "Tesselation";
	element[1].param.integer = p_geo_get_sds_level();

	element[2].type = S_VET_INTEGER;
	element[2].text = "Poly Cap";
	element[2].param.integer = p_geo_get_sds_poly_cap();

	element[3].type = S_VET_SLIDER;
	element[3].text = "Complexity";
	element[3].param.slider = sqrt(p_geo_get_sds_mesh_factor() / 1000.0);

	element[4].type = S_VET_INTEGER;
	element[4].text = "Force Tesselation";
	element[4].param.integer = p_geo_get_sds_force_level();

	element[5].type = S_VET_SPLIT;
	element[5].text = "render to texture";

	element[6].type = S_VET_BOOLEAN;
	element[6].text = "Enable";
	element[6].param.checkbox = p_render_get_fbo_enable();

	element[7].type = S_VET_REAL;
	element[7].text = "Impostor Size";
	element[7].param.real = p_render_get_impostor_size();

	element[8].type = S_VET_INTEGER;
	element[8].text = "Resolution";
	element[8].param.integer = p_render_get_impostor_resolution();

	element[9].type = S_VET_BOOLEAN;
	element[9].text = "Draw Shadows";
	element[9].param.checkbox = p_render_get_shadows();

	element[10].type = S_VET_BOOLEAN;
	element[10].text = "Draw Wireframe";
	element[10].param.checkbox = p_render_get_wireframe();

	element[11].type = S_VET_BOOLEAN;
	element[11].text = "Use HDRI";
	element[11].param.checkbox = p_th_get_sds_use_hdri();

	element[12].type = S_VET_BOOLEAN;
	element[12].text = "Draw flares";
	element[12].param.checkbox = p_render_get_flare();

	element[13].type = S_VET_INTEGER;
	element[13].text = "Light Count";
	element[13].param.integer = p_render_get_light_count();*/
}

void sp_settings_init()
{
	p_geo_set_sds_level(sui_get_setting_int("P_BASE_TESSELATION", 3));
	p_geo_set_sds_poly_cap(sui_get_setting_int("P_POLY_CAP", 100000));
	p_geo_set_sds_mesh_factor(sui_get_setting_double("P_FACTOR_TESSELATION", 1));
	p_render_set_fbo_enable(sui_get_setting_int("P_FBO_ENABLE", TRUE));
	p_geo_set_sds_force_level(sui_get_setting_int("P_FORCE_TESSELATION", 0));
	p_render_set_impostor_size(sui_get_setting_double("P_IMPOSTOR_SIZE", 0.03));
	p_render_set_impostor_resolution(sui_get_setting_int("P_IMPOSTOR_RESOLUTION", 128));
	p_render_set_shadows(sui_get_setting_int("P_RENDER_SHADOWS", TRUE));
	p_render_set_wireframe(sui_get_setting_int("P_RENDER_WIREFRAME", FALSE));
	p_th_set_sds_use_hdri(sui_get_setting_int("P_HDRI_TEXTURE", FALSE));
	p_render_set_flare(sui_get_setting_int("P_RENDER_FLARE", TRUE));
	p_render_set_light_count(sui_get_setting_int("P_LIGHT_COUNT", 2));
	
}

void sp_settings_post(SUIViewElement *element)
{
/*	sui_set_setting_int("P_BASE_TESSELATION", element[1].param.integer);
	p_geo_set_sds_level(element[1].param.integer);
	sui_set_setting_int("P_POLY_CAP", element[2].param.integer);
	p_geo_set_sds_poly_cap(element[2].param.integer);
	sui_set_setting_double("P_FACTOR_TESSELATION", element[3].param.slider * element[3].param.slider * 1000.0);
	p_geo_set_sds_mesh_factor(element[3].param.slider * element[3].param.slider * 1000.0);
	sui_set_setting_int("P_FORCE_TESSELATION", element[4].param.integer);
	p_geo_set_sds_force_level(element[4].param.integer);
	sui_set_setting_int("P_FBO_ENABLE", element[6].param.checkbox);
	p_render_set_fbo_enable(element[6].param.checkbox);
	sui_set_setting_double("P_IMPOSTOR_SIZE", element[7].param.real);
	p_render_set_impostor_size(element[7].param.real);
	sui_set_setting_int("P_IMPOSTOR_RESOLUTION", element[8].param.integer);
	p_render_set_impostor_resolution(element[8].param.integer);
	sui_set_setting_int("P_RENDER_SHADOWS", element[9].param.integer);
	p_render_set_shadows(element[9].param.checkbox);
	sui_set_setting_int("P_RENDER_WIREFRAME", element[10].param.checkbox);
	p_render_set_wireframe(element[10].param.checkbox);
	sui_set_setting_int("P_HDRI_TEXTURE", element[11].param.checkbox);
	if(element[11].param.checkbox != p_th_get_sds_use_hdri())
	{
		p_th_set_sds_use_hdri(element[11].param.checkbox);
		p_context_update();
	}
	sui_set_setting_int("P_RENDER_FLARE", element[12].param.checkbox);
	p_render_set_flare(element[12].param.checkbox);
	sui_set_setting_int("P_LIGHT_COUNT", element[13].param.integer);
	p_render_set_light_count(element[13].param.integer);*/
	
}
/*

void sp_settings_post(SUIViewElement *element)
{
	p_geo_set_sds_level(element[1].param.integer);
	p_geo_set_sds_poly_cap(element[2].param.integer);
	p_geo_set_sds_mesh_factor(element[3].param.slider * element[3].param.slider * 1000.0);
	p_geo_set_sds_force_level(element[4].param.integer);
	p_render_set_impostor_size(element[6].param.real);
	p_render_set_impostor_resolution(element[7].param.integer);
	p_render_set_shadows(element[8].param.checkbox);
	p_render_set_wireframe(element[9].param.checkbox);
	if(element[10].param.checkbox != p_th_get_sds_use_hdri())
	{
		p_th_set_sds_use_hdri(element[10].param.checkbox);
		p_context_update();
	}
	p_render_set_flare(element[11].param.checkbox);
	p_render_set_light_count(element[12].param.checkbox);
	
}*/
