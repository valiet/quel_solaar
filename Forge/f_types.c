#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "forge.h"

float f_randf(uint32 index)
{
	index = (index << 13) ^ index;
	return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f) * 0.5f;
}

double f_randd(uint32 index)
{
	index = (index << 13) ^ index;
	return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.5;
}

float f_randnf(uint32 index)
{
	index = (index << 13) ^ index;
	return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f) - 1.0f;
}

double f_randnd(uint32 index)
{
	index = (index << 13) ^ index;
	return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) - 1.0;
}

uint f_randi(uint32 index)
{
	index = (index << 13) ^ index;
	return ((index * (index * index * 15731 + 789221) + 1376312589) & 0x7fffffff);
}

void f_rgb_to_hsv(float *output, float r, float g, float b)
{
	if(r < g)
	{
		if(r < b)
		{
			if(g < b)
			{
				output[0] = (4.0f - (g - r) / (b - r)) / 6.0f;
				output[1] = 1.0f - r / b;
				output[2] = b;
			}else
			{
				output[0] = (2.0f + (b - r) / (g - r)) / 6.0f;
				output[1] = 1.0f - r / g;
				output[2] = g;
			}
		}else
		{
			output[0] = (2.0f - (r - b) / (g - b)) / 6.0f;
			output[1] = 1.0f - b / g;
			output[2] = g;
		}
	}else
	{
		if(r < b)
		{
			output[0] = (4.0f + (r - g) / (b - g)) / 6.0f;
			output[1] = 1.0f - g / b;
			output[2] = b;
		}else
		{
			if(g < b)
			{
				output[0] = (6.0f - (b - g) / (r - g)) / 6.0f;
				output[1] = 1.0f - g / r;
				output[2] = r;
			}else if(r == g && r == b)
			{
				output[0] = 0.0f;
				output[1] = 0.0f;
				output[2] = r;
			}else
			{

				output[0] = (0.0f + (g - b) / (r - b)) / 6.0f;
				output[1] = 1.0f - b / r;
				output[2] = r;
			}
		}
	}
}

void f_hsv_to_rgb(float *output, float h, float s, float v)
{
	if(h > 1 || h < 0.0f)
		h = h - (float)((int)h);
	if(s < 0.0f)
		s = 0.0f;
	if(s > 1.0f)
		s = 1.0f;
	if(v < 0.0f)
		v = 0.0f;
	if(v > 1.0f)
		v = 1.0f;

	s = 1.0f - s;
	h *= 6.0f;
	s *= v;

	switch((uint)h)
	{
		case 0 :
			output[0] = v;
			output[1] = s + (0.0f + h) * (v - s);
			output[2] = s;
		break;
		case 1 :
			output[0] = s + (2.0f - h) * (v - s);
			output[1] = v;
			output[2] = s;
		break;
		case 2 :
			output[0] = s;
			output[1] = v;
			output[2] = s + (h - 2.0f) * (v - s);
		break;
		case 3 :
			output[0] = s;
			output[1] = s + (4.0f - h) * (v - s);
			output[2] = v;
		break;
		case 4 :
			output[0] = s + (h - 4.0f) * (v - s);
			output[1] = s;
			output[2] = v;
		break;
		case 5 :
		case 6 :
			output[0] = v;
			output[1] = s;
			output[2] = s + (6.0f - h) * (v - s);
		break;
	}

/*	switch((uint)h)
	{
		case 0 :
			output[0] = v;
			output[1] = s + (0.0f + h) * (v - s);
			output[2] = s;
		break;
		case 1 :
			output[0] = s + (2.0f - h) * (v - s);
			output[1] = v;
			output[2] = s;
		break;
		case 2 :
			output[0] = s;
			output[1] = v;
			output[2] = s + (h - 2.0f) * (v - s);
		break;
		case 3 :
			output[0] = s;
			output[1] = s + (4.0f - h) * (v - s);
			output[2] = v;
		break;
		case 4 :
			output[0] = s + (h - 4.0f) * (v - s);
			output[1] = s;
			output[2] = v;
		break;
		default :
			output[0] = v;
			output[1] = s;
			output[2] = s + (6.0f - h) * (v - s);
		break;
	}*/
}

void f_xyz_to_rgb3(float *output, float x, float y, float z)
{
	float tmp[3];
	tmp[0] = x / 100.0f;
	tmp[1] = y / 100.0f;
	tmp[2] = z / 100.0f;

	output[0] = tmp[0] *  3.2406f + tmp[1] * -1.5372f + tmp[2] * -0.4986f;
	output[1] = tmp[0] * -0.9689f + tmp[1] *  1.8758f + tmp[2] *  0.0415f;
	output[2] = tmp[0] *  0.0557f + tmp[1] * -0.2040f + tmp[2] *  1.0570f;
}

#define f_WHITEPOINT_X	0.950456
#define f_WHITEPOINT_Y	1.0
#define f_WHITEPOINT_Z	1.088754

void f_rgb_to_xyz(float *xyz, float r, float g, float b)
{
	if(r <= 0.0404482362771076f)
		r = r / 12.92f;
	else
		r = (float)pow((r + 0.055f) / 1.055f, 2.4f);
	if(g <= 0.0404482362771076f)
		g = g / 12.92f;
	else
		g = (float)pow((g + 0.055f) / 1.055f, 2.4f);
	if(b <= 0.0404482362771076f)
		b = b / 12.92f;
	else
		b = (float)pow((b + 0.055) / 1.055f, 2.4f);
	xyz[0] = 0.4123955889674142161f * r + 0.3575834307637148171f * g + 0.1804926473817015735f * b;
	xyz[1] = 0.2125862307855955516f * r + 0.7151703037034108499f * g + 0.07220049864333622685f * b;
	xyz[2] = 0.01929721549174694484f * r + 0.1191838645808485318f * g + 0.9504971251315797660f * b;
}

void f_xyz_to_rgb(float *rgb, float x, float y, float z)
{	
	float min;	
	rgb[0] = 3.2406f * x - 1.5372f * y - 0.4986f * z;
	rgb[1] = -0.9689f * x + 1.8758f * y + 0.0415f * z;
	rgb[2] =  0.0557f * x - 0.2040f * y + 1.0570f * z;

	if(rgb[0] < rgb[1])
	{
		if(rgb[0] < rgb[2])
			min = rgb[0];
		else
			min = rgb[2];
	}else
	{
		if(rgb[1] < rgb[2])
			min = rgb[1];
		else
			min = rgb[2];
	}

	if(min < 0)
	{
		rgb[0] -= min;
		rgb[1] -= min;
		rgb[2] -= min;
	}
	
	if(rgb[0] <= 0.0031306684425005883f)
		rgb[0] = 12.92f * rgb[0];
	else
		rgb[0] = 1.055f * pow(rgb[0], 0.416666666666666667f) - 0.055f;
	
	if(rgb[1] <= 0.0031306684425005883f)
		rgb[1] = 12.92f * rgb[1];
	else
		rgb[1] = 1.055f * pow(rgb[1], 0.416666666666666667f) - 0.055f;

	if(rgb[2] <= 0.0031306684425005883f)
		rgb[2] = 12.92f * rgb[2];
	else
		rgb[2] = 1.055f * pow(rgb[2], 0.416666666666666667f) - 0.055f;
}

void f_xyz_to_lab(float *lab, float x, float y, float z)
{
	x /= f_WHITEPOINT_X;
	y /= f_WHITEPOINT_Y;
	z /= f_WHITEPOINT_Z;
	if(x >= 8.85645167903563082e-3f)
		x = pow(x, 0.333333333333333f);
	else
		x = (841.0f / 108.0f) * x + (4.0f / 29.0f);
	if(y >= 8.85645167903563082e-3f)
		y = pow(y, 0.333333333333333f);
	else
		y = (841.0f / 108.0f) * y + (4.0f / 29.0f);
	if(z >= 8.85645167903563082e-3f)
		z = pow(z, 0.333333333333333f);
	else
		z = (841.0f / 108.0f) * z + (4.0f / 29.0f);
	lab[0] = 116.0f * y - 16.0f;
	lab[1] = 500.0f * (x - y);
	lab[2] = 200.0f * (y - z);
}

void f_lab_to_xyz(float *xyz, float l, float a, float b)
{
	l = (l + 16.0f) / 116.0f;
	a = l + a / 500.0f;
	b = l - b / 200.0f;

	if(a >= 0.206896551724137931f)
		xyz[0] = f_WHITEPOINT_X * a * a * a;
	else
		xyz[0] = f_WHITEPOINT_X * (108.0f / 841.0f) * (a - (4.0f / 29.0f));

	if(l >= 0.206896551724137931f)
		xyz[1] = f_WHITEPOINT_Y * l * l * l;
	else
		xyz[1] = f_WHITEPOINT_Y * (108.0f / 841.0f) * (l - (4.0f / 29.0f));

	if(b >= 0.206896551724137931f)
		xyz[2] = f_WHITEPOINT_Z * b * b * b;
	else
		xyz[2] = f_WHITEPOINT_Z * (108.0f / 841.0f) * (b - (4.0f / 29.0f));
}

void f_rgb_to_lab(float *lab, float r, float g, float b)
{
	float tmp[3];
	f_rgb_to_xyz(tmp, r, g, b);
	f_xyz_to_lab(lab, tmp[0], tmp[1], tmp[2]);
}

void f_lab_to_rgb(float *rgb, float l, float a, float b)
{
	float tmp[3];
	f_lab_to_xyz(tmp, l, a, b);
	f_xyz_to_rgb(rgb, tmp[0], tmp[1], tmp[2]);
}



float f_splinef(float f, float v0, float v1, float v2, float v3)
{
	float inv;
	inv = 1.0f - f;
	return ((v3 * f + v2 * inv) * f + (v2 * f + v1 * inv) * inv) * f + ((v2 * f + v1 * inv) * f + (v1 * f + v0 * inv) * inv) * inv;
}

double f_splined(double f, double v0, double v1, double v2, double v3)
{
	double inv;
	inv = 1.0f - f;
	return ((v3 * f + v2 * inv) * f + (v2 * f + v1 * inv) * inv) * f + ((v2 * f + v1 * inv) * f + (v1 * f + v0 * inv) * inv) * inv;
}

void f_spline2df(float *out, float f, float *v0, float *v1, float *v2, float *v3)
{
	float inv;
	inv = 1.0f - f;
	out[0] = ((v3[0] * f + v2[0] * inv) * f + (v2[0] * f + v1[0] * inv) * inv) * f + ((v2[0] * f + v1[0] * inv) * f + (v1[0] * f + v0[0] * inv) * inv) * inv;
	out[1] = ((v3[1] * f + v2[1] * inv) * f + (v2[1] * f + v1[1] * inv) * inv) * f + ((v2[1] * f + v1[1] * inv) * f + (v1[1] * f + v0[1] * inv) * inv) * inv;
}

void f_spline2dd(double *out, double f, double *v0, double *v1, double *v2, double *v3)
{
	double inv;
	inv = 1.0f - f;
	out[0] = ((v3[0] * f + v2[0] * inv) * f + (v2[0] * f + v1[0] * inv) * inv) * f + ((v2[0] * f + v1[0] * inv) * f + (v1[0] * f + v0[0] * inv) * inv) * inv;
	out[1] = ((v3[1] * f + v2[1] * inv) * f + (v2[1] * f + v1[1] * inv) * inv) * f + ((v2[1] * f + v1[1] * inv) * f + (v1[1] * f + v0[1] * inv) * inv) * inv;
}

void f_spline3df(float *out, float f, float *v0, float *v1, float *v2, float *v3)
{
	float inv;
	inv = 1.0f - f;
	out[0] = ((v3[0] * f + v2[0] * inv) * f + (v2[0] * f + v1[0] * inv) * inv) * f + ((v2[0] * f + v1[0] * inv) * f + (v1[0] * f + v0[0] * inv) * inv) * inv;
	out[1] = ((v3[1] * f + v2[1] * inv) * f + (v2[1] * f + v1[1] * inv) * inv) * f + ((v2[1] * f + v1[1] * inv) * f + (v1[1] * f + v0[1] * inv) * inv) * inv;
	out[2] = ((v3[2] * f + v2[2] * inv) * f + (v2[2] * f + v1[2] * inv) * inv) * f + ((v2[2] * f + v1[2] * inv) * f + (v1[2] * f + v0[2] * inv) * inv) * inv;
}

void f_spline3dd(double *out, double f, double *v0, double *v1, double *v2, double *v3)
{

	double inv;
	inv = 1.0 - f;
	out[0] = ((v3[0] * f + v2[0] * inv) * f + (v2[0] * f + v1[0] * inv) * inv) * f + ((v2[0] * f + v1[0] * inv) * f + (v1[0] * f + v0[0] * inv) * inv) * inv;
	out[1] = ((v3[1] * f + v2[1] * inv) * f + (v2[1] * f + v1[1] * inv) * inv) * f + ((v2[1] * f + v1[1] * inv) * f + (v1[1] * f + v0[1] * inv) * inv) * inv;
	out[2] = ((v3[2] * f + v2[2] * inv) * f + (v2[2] * f + v1[2] * inv) * inv) * f + ((v2[2] * f + v1[2] * inv) * f + (v1[2] * f + v0[2] * inv) * inv) * inv;
}

void f_spline4df(float *out, float f, float *v0, float *v1, float *v2, float *v3)
{
	float inv;
	inv = 1.0f - f;
	out[0] = ((v3[0] * f + v2[0] * inv) * f + (v2[0] * f + v1[0] * inv) * inv) * f + ((v2[0] * f + v1[0] * inv) * f + (v1[0] * f + v0[0] * inv) * inv) * inv;
	out[1] = ((v3[1] * f + v2[1] * inv) * f + (v2[1] * f + v1[1] * inv) * inv) * f + ((v2[1] * f + v1[1] * inv) * f + (v1[1] * f + v0[1] * inv) * inv) * inv;
	out[2] = ((v3[2] * f + v2[2] * inv) * f + (v2[2] * f + v1[2] * inv) * inv) * f + ((v2[2] * f + v1[2] * inv) * f + (v1[2] * f + v0[2] * inv) * inv) * inv;
	out[3] = ((v3[3] * f + v2[3] * inv) * f + (v2[3] * f + v1[3] * inv) * inv) * f + ((v2[3] * f + v1[3] * inv) * f + (v1[3] * f + v0[3] * inv) * inv) * inv;
}

void f_spline4dd(double *out, double f, double *v0, double *v1, double *v2, double *v3)
{
	double inv;
	inv = 1.0 - f;
	out[0] = ((v3[0] * f + v2[0] * inv) * f + (v2[0] * f + v1[0] * inv) * inv) * f + ((v2[0] * f + v1[0] * inv) * f + (v1[0] * f + v0[0] * inv) * inv) * inv;
	out[1] = ((v3[1] * f + v2[1] * inv) * f + (v2[1] * f + v1[1] * inv) * inv) * f + ((v2[1] * f + v1[1] * inv) * f + (v1[1] * f + v0[1] * inv) * inv) * inv;
	out[2] = ((v3[2] * f + v2[2] * inv) * f + (v2[2] * f + v1[2] * inv) * inv) * f + ((v2[2] * f + v1[2] * inv) * f + (v1[2] * f + v0[2] * inv) * inv) * inv;
	out[3] = ((v3[3] * f + v2[3] * inv) * f + (v2[3] * f + v1[3] * inv) * inv) * f + ((v2[3] * f + v1[3] * inv) * f + (v1[3] * f + v0[3] * inv) * inv) * inv;
}

float f_wigglef(float f, float size)
{
	float v0, v1, v2, v3;
	uint seed;
	seed = (float)f;
	f -= (float)seed;
	seed *= 2;
	size *= 2.0;
	v0 = (f_randf(seed++) - 0.5) * size;
	v1 = (f_randf(seed++) - 0.5) * size;
	v3 = (f_randf(seed++) - 0.5) * size;
	v2 = v3 * 2.0 - (f_randf(seed++) - 0.5) * size;
	return f_spline(f, v0, v1, v2, v3);
}

double f_wiggled(double f, double size)
{
	double v0, v1, v2, v3;
	uint seed;
	seed = (double)f;
	f -= (double)seed;
	seed *= 2;
	size *= 2.0;
	v0 = (f_randd(seed++) - 0.5) * size;
	v1 = (f_randd(seed++) - 0.5) * size;
	v3 = (f_randd(seed++) - 0.5) * size;
	v2 = v3 * 2.0 - (f_randd(seed++) - 0.5) * size;
	return f_spline(f, v0, v1, v2, v3);
}

void f_wiggle2df(float *out, float f, float size)
{
	float v0[2], v1[2], v2[2], v3[2];
	uint seed;
	seed = (float)f;
	f -= (float)seed;
	seed *= 4;
	size *= 2.0;
	v0[0] = (f_randf(seed++) - 0.5) * size;
	v0[1] = (f_randf(seed++) - 0.5) * size;
	v1[0] = (f_randf(seed++) - 0.5) * size;
	v1[1] = (f_randf(seed++) - 0.5) * size;
	v3[0] = (f_randf(seed++) - 0.5) * size;
	v3[1] = (f_randf(seed++) - 0.5) * size;
	v2[0] = v3[0] * 2.0 - (f_randf(seed++) - 0.5) * size;
	v2[1] = v3[1] * 2.0 - (f_randf(seed++) - 0.5) * size;
	f_spline2df(out, f, v0, v1, v2, v3);
}

void f_wiggle2dd(double *out, double f, double size)
{
	double v0[2], v1[2], v2[2], v3[2];
	uint seed;
	seed = (double)f;
	f -= (double)seed;
	seed *= 4;
	size *= 2.0;
	v0[0] = (f_randd(seed++) - 0.5) * size;
	v0[1] = (f_randd(seed++) - 0.5) * size;
	v1[0] = (f_randd(seed++) - 0.5) * size;
	v1[1] = (f_randd(seed++) - 0.5) * size;
	v3[0] = (f_randd(seed++) - 0.5) * size;
	v3[1] = (f_randd(seed++) - 0.5) * size;
	v2[0] = v3[0] * 2.0 - (f_randd(seed++) - 0.5) * size;
	v2[1] = v3[1] * 2.0 - (f_randd(seed++) - 0.5) * size;
	f_spline2dd(out, f, v0, v1, v2, v3);
}

void f_wiggle3df(float *out, float f, float size)
{
	float v0[3], v1[3], v2[3], v3[3];
	uint seed;
	seed = (float)f;
	f -= (float)seed;
	seed *= 6;
	size *= 2.0;
	v0[0] = (f_randf(seed++) - 0.5) * size;
	v0[1] = (f_randf(seed++) - 0.5) * size;
	v0[2] = (f_randf(seed++) - 0.5) * size;
	v1[0] = (f_randf(seed++) - 0.5) * size;
	v1[1] = (f_randf(seed++) - 0.5) * size;
	v1[2] = (f_randf(seed++) - 0.5) * size;
	v3[0] = (f_randf(seed++) - 0.5) * size;
	v3[1] = (f_randf(seed++) - 0.5) * size;
	v3[2] = (f_randf(seed++) - 0.5) * size;
	v2[0] = v3[0] * 2.0 - (f_randf(seed++) - 0.5) * size;
	v2[1] = v3[1] * 2.0 - (f_randf(seed++) - 0.5) * size;
	v2[2] = v3[2] * 2.0 - (f_randf(seed++) - 0.5) * size;
	f_spline3df(out, f, v0, v1, v2, v3);
}

void f_wiggle3dd(double *out, double f, double size)
{
	double v0[3], v1[3], v2[3], v3[3];
	uint seed;
	seed = (double)f;
	f -= (double)seed;
	seed *= 6;
	size *= 2.0;
	v0[0] = (f_randd(seed++) - 0.5) * size;
	v0[1] = (f_randd(seed++) - 0.5) * size;
	v0[2] = (f_randd(seed++) - 0.5) * size;
	v1[0] = (f_randd(seed++) - 0.5) * size;
	v1[1] = (f_randd(seed++) - 0.5) * size;
	v1[2] = (f_randd(seed++) - 0.5) * size;
	v3[0] = (f_randd(seed++) - 0.5) * size;
	v3[1] = (f_randd(seed++) - 0.5) * size;
	v3[2] = (f_randd(seed++) - 0.5) * size;
	v2[0] = v3[0] * 2.0 - (f_randd(seed++) - 0.5) * size;
	v2[1] = v3[1] * 2.0 - (f_randd(seed++) - 0.5) * size;
	v2[2] = v3[2] * 2.0 - (f_randd(seed++) - 0.5) * size;
	f_spline3dd(out, f, v0, v1, v2, v3);
}

#define F_EPSILON 0.000001

boolean f_raycast_trif(float orig[3], float dir[3], float vert0[3], float vert1[3], float vert2[3], float *t, float *u, float *v)
{
	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float det, inv_det;

	edge1[0] = vert1[0] - vert0[0];
	edge1[1] = vert1[1] - vert0[1];
	edge1[2] = vert1[2] - vert0[2]; 
	edge2[0] = vert2[0] - vert0[0];
	edge2[1] = vert2[1] - vert0[1];
	edge2[2] = vert2[2] - vert0[2];
	pvec[0] = dir[1] * edge2[2] - dir[2] * edge2[1];
	pvec[1] = dir[2] * edge2[0] - dir[0] * edge2[2];
	pvec[2] = dir[0] * edge2[1] - dir[1] * edge2[0];
	det = edge1[0] * pvec[0] + edge1[1] * pvec[1] + edge1[2] * pvec[2];

	if(det > -F_EPSILON && det < F_EPSILON)
		return FALSE;
	inv_det = 1.0 / det;

	tvec[0] = orig[0] - vert0[0];
	tvec[1] = orig[1] - vert0[1];
	tvec[2] = orig[2] - vert0[2];

	*u = (tvec[0] * pvec[0] + tvec[1] * pvec[1] + tvec[2] * pvec[2]) * inv_det;
	if(*u < 0.0 || *u > 1.0)
		return FALSE;

	qvec[0] = tvec[1] * edge1[2] - tvec[2] * edge1[1];
	qvec[1] = tvec[2] * edge1[0] - tvec[0] * edge1[2];
	qvec[2] = tvec[0] * edge1[1] - tvec[1] * edge1[0];

	*v = (dir[0] * qvec[0] + dir[1] * qvec[1] + dir[2] * qvec[2]) * inv_det;
	if(*v < 0.0 || *u + *v > 1.0)
		return FALSE;

	*t = (edge2[0] * qvec[0] + edge2[1] * qvec[1] + edge2[2] * qvec[2]) * inv_det;
	return TRUE;
}


boolean f_raycast_tri_cullf(float orig[3], float dir[3], float vert0[3], float vert1[3], float vert2[3], float *t, float *u, float *v)
{
	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float det, inv_det;

	edge1[0] = vert1[0] - vert0[0];
	edge1[1] = vert1[1] - vert0[1];
	edge1[2] = vert1[2] - vert0[2]; 
	edge2[0] = vert2[0] - vert0[0];
	edge2[1] = vert2[1] - vert0[1];
	edge2[2] = vert2[2] - vert0[2];
	pvec[0] = dir[1] * edge2[2] - dir[2] * edge2[1];
	pvec[1] = dir[2] * edge2[0] - dir[0] * edge2[2];
	pvec[2] = dir[0] * edge2[1] - dir[1] * edge2[0];
	det = edge1[0] * pvec[0] + edge1[1] * pvec[1] + edge1[2] * pvec[2];
 
	if(det < F_EPSILON)
		return FALSE;

	tvec[0] = orig[0] - vert0[0];
	tvec[1] = orig[1] - vert0[1];
	tvec[2] = orig[2] - vert0[2];

	*u = tvec[0] * pvec[0] + tvec[1] * pvec[1] + tvec[2] * pvec[2];
	if(*u < 0.0 || *u > det)
		return FALSE;

	qvec[0] = tvec[1] * edge1[2] - tvec[2] * edge1[1];
	qvec[1] = tvec[2] * edge1[0] - tvec[0] * edge1[2];
	qvec[2] = tvec[0] * edge1[1] - tvec[1] * edge1[0];
	*v = dir[0] * qvec[0] + dir[1] * qvec[1] + dir[2] * qvec[2];
	if(*v < 0.0 || *u + *v > det)
		return FALSE;

	*t = edge2[0] * qvec[0] + edge2[1] * qvec[1] + edge2[2] * qvec[2];
	inv_det = 1.0 / det;
	*t *= inv_det;
	*u *= inv_det;
	*v *= inv_det;

	return TRUE;
}


boolean f_raycast_trid(double orig[3], double dir[3], double vert0[3], double vert1[3], double vert2[3], double *t, double *u, double *v)
{
	double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	double det, inv_det;

	edge1[0] = vert1[0] - vert0[0];
	edge1[1] = vert1[1] - vert0[1];
	edge1[2] = vert1[2] - vert0[2]; 
	edge2[0] = vert2[0] - vert0[0];
	edge2[1] = vert2[1] - vert0[1];
	edge2[2] = vert2[2] - vert0[2];
	pvec[0] = dir[1] * edge2[2] - dir[2] * edge2[1];
	pvec[1] = dir[2] * edge2[0] - dir[0] * edge2[2];
	pvec[2] = dir[0] * edge2[1] - dir[1] * edge2[0];
	det = edge1[0] * pvec[0] + edge1[1] * pvec[1] + edge1[2] * pvec[2];

	if(det > -F_EPSILON && det < F_EPSILON)
		return FALSE;
	inv_det = 1.0 / det;

	tvec[0] = orig[0] - vert0[0];
	tvec[1] = orig[1] - vert0[1];
	tvec[2] = orig[2] - vert0[2];

	*u = (tvec[0] * pvec[0] + tvec[1] * pvec[1] + tvec[2] * pvec[2]) * inv_det;
	if(*u < 0.0 || *u > 1.0)
		return FALSE;

	qvec[0] = tvec[1] * edge1[2] - tvec[2] * edge1[1];
	qvec[1] = tvec[2] * edge1[0] - tvec[0] * edge1[2];
	qvec[2] = tvec[0] * edge1[1] - tvec[1] * edge1[0];

	*v = (dir[0] * qvec[0] + dir[1] * qvec[1] + dir[2] * qvec[2]) * inv_det;
	if(*v < 0.0 || *u + *v > 1.0)
		return FALSE;

	*t = (edge2[0] * qvec[0] + edge2[1] * qvec[1] + edge2[2] * qvec[2]) * inv_det;
	return TRUE;
}


boolean f_raycast_tri_culld(double orig[3], double dir[3], double vert0[3], double vert1[3], double vert2[3], double *t, double *u, double *v)
{
	double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	double det, inv_det;

	edge1[0] = vert1[0] - vert0[0];
	edge1[1] = vert1[1] - vert0[1];
	edge1[2] = vert1[2] - vert0[2]; 
	edge2[0] = vert2[0] - vert0[0];
	edge2[1] = vert2[1] - vert0[1];
	edge2[2] = vert2[2] - vert0[2];
	pvec[0] = dir[1] * edge2[2] - dir[2] * edge2[1];
	pvec[1] = dir[2] * edge2[0] - dir[0] * edge2[2];
	pvec[2] = dir[0] * edge2[1] - dir[1] * edge2[0];
	det = edge1[0] * pvec[0] + edge1[1] * pvec[1] + edge1[2] * pvec[2];
 
	if(det < F_EPSILON)
		return FALSE;

	tvec[0] = orig[0] - vert0[0];
	tvec[1] = orig[1] - vert0[1];
	tvec[2] = orig[2] - vert0[2];

	*u = tvec[0] * pvec[0] + tvec[1] * pvec[1] + tvec[2] * pvec[2];
	if(*u < 0.0 || *u > det)
		return FALSE;

	qvec[0] = tvec[1] * edge1[2] - tvec[2] * edge1[1];
	qvec[1] = tvec[2] * edge1[0] - tvec[0] * edge1[2];
	qvec[2] = tvec[0] * edge1[1] - tvec[1] * edge1[0];
	*v = dir[0] * qvec[0] + dir[1] * qvec[1] + dir[2] * qvec[2];
	if(*v < 0.0 || *u + *v > det)
		return FALSE;

	*t = edge2[0] * qvec[0] + edge2[1] * qvec[1] + edge2[2] * qvec[2];
	inv_det = 1.0 / det;
	*t *= inv_det;
	*u *= inv_det;
	*v *= inv_det;

	return TRUE;
}

