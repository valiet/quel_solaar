
#include "verse.h"
#include "forge.h"
#include "enough.h"

/*
 -------- VERSE EXPORT TOOL KIT -------------

VETK is code vritten on top of Enough to make it easy to export data to game engine type of applications.

*/


#define VETK_VP_VERTEX_X	"x.vertex"
#define VETK_VP_VERTEX_Y	"y.vertex"
#define VETK_VP_VERTEX_Z	"z.vertex"
#define VETK_VP_NORMAL_X	"x.normal"
#define VETK_VP_NORMAL_Y	"y.normal"
#define VETK_VP_NORMAL_Z	"z.normal"
#define VETK_VP_COLOR_R		"color_r"
#define VETK_VP_COLOR_G		"color_g"
#define VETK_VP_COLOR_B		"color_b"
#define VETK_VP_MAPPING_U	"map_u"
#define VETK_VP_MAPPING_V	"map_v"
#define VETK_VP_MAPPING_W	"map_w"
#define VETK_VP_EMPTY		"longer_then_a_layer_name_ever_can_be"

typedef struct{
	uint *neighbor; // each polygon edges neighboring edge
	float *vertex; // interleaved vertex arrays
	uint *ref; // reference data, first quads, followed by tris
	uint quad_length; // number of quad corners (quad count * four)
	uint tri_length; // number of tri corners (tri count * three)
	uint vertex_length;	 // number of vertices
}VETKQuadTri;

/*
	vetk_create_mesh takes a Enough geometry node and optimizes it in to a VETKQuadTri. will return NULL if no valid polys are found in the node

	-node: geometry node
	-vertex_params: list of strings describing the content o0f the packed vertex format. use layer names or VETK_VP_... defines 
	-param_count: number of vertex params 
	-reference: set to TRUE if you want a reference array and a vertex array.
	-quads: set to TRUE if you want both quads and tris, set to FALSE if you only want tris
*/

extern VETKQuadTri *vetk_create_mesh(ENode *node, char **vertex_params, uint param_count, boolean reference, boolean quads);

/*
	vetk_create_mesh frees a VETKQuadTri anmd its members.
*/

extern void			vetk_destroy_mesh(VETKQuadTri *mesh);



/* ----------------------------- OpenGL EXAMPLE CODE ------------------------------------------ 


--------------------1, drawing unindexed tris


#define VERTEX_PARAM_COUNT 12

char *vertex_params[VERTEX_PARAM_COUNT] = {VETK_VP_VERTEX_X, VETK_VP_VERTEX_Y, VETK_VP_VERTEX_Z, VETK_VP_NORMAL_X, VETK_VP_NORMAL_Y, VETK_VP_NORMAL_Z, VETK_VP_COLOR_R, VETK_VP_COLOR_G, VETK_VP_COLOR_B, VETK_VP_MAPPING_U, VETK_VP_MAPPING_V, VETK_VP_EMPTY};
VETKQuadTri *mesh;

mesh = vetk_create_mesh(node, vertex_params, VERTEX_PARAM_COUNT, TRUE, FALSE); // create optimized data
if(mesh != NULL)
{
	glEnableClientState(GL_VERTEX_ARRAY); // enable arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, mesh->vertex); // set pointers to arrays
	glNormalPointer(GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[3]);
	glColorPointer(3, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[6]);
	glColorPointer(2, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[9]);	

	glDrawArrays(GL_TRIANGLES, 0, mesh->tris_length); // draw
	vetk_destroy_mesh(mesh); // destroy mesh
}

--------------------2, drawing indexed tris


#define VERTEX_PARAM_COUNT 12

char *vertex_params[VERTEX_PARAM_COUNT] = {VETK_VP_VERTEX_X, VETK_VP_VERTEX_Y, VETK_VP_VERTEX_Z, VETK_VP_NORMAL_X, VETK_VP_NORMAL_Y, VETK_VP_NORMAL_Z, VETK_VP_COLOR_R, VETK_VP_COLOR_G, VETK_VP_COLOR_B, VETK_VP_MAPPING_U, VETK_VP_MAPPING_V, VETK_VP_EMPTY};
VETKQuadTri *mesh;

mesh = vetk_create_mesh(node, vertex_params, VERTEX_PARAM_COUNT, TRUE, FALSE); // create optimized data
if(mesh != NULL)
{
	glEnableClientState(GL_VERTEX_ARRAY); // enable arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, mesh->vertex); // set pointers to arrays
	glNormalPointer(GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[3]);
	glColorPointer(3, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[6]);
	glColorPointer(2, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[9]);	

	glDrawElements(GL_TRIANGLES, mesh->tri_length, GL_UNSIGNED_INT, mesh->ref); // draw
	vetk_destroy_mesh(mesh); // destroy mesh
}

--------------------3, drawing unindexed quads and tris


#define VERTEX_PARAM_COUNT 12

char *vertex_params[VERTEX_PARAM_COUNT] = {VETK_VP_VERTEX_X, VETK_VP_VERTEX_Y, VETK_VP_VERTEX_Z, VETK_VP_NORMAL_X, VETK_VP_NORMAL_Y, VETK_VP_NORMAL_Z, VETK_VP_COLOR_R, VETK_VP_COLOR_G, VETK_VP_COLOR_B, VETK_VP_MAPPING_U, VETK_VP_MAPPING_V, VETK_VP_EMPTY};
VETKQuadTri *mesh;

mesh = vetk_create_mesh(node, vertex_params, VERTEX_PARAM_COUNT, FALSE, TRUE); // create optimized data
if(mesh != NULL)
{
	glEnableClientState(GL_VERTEX_ARRAY); // enable arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, mesh->vertex); // set pointers to arrays to begining for tris
	glNormalPointer(GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[3]);
	glColorPointer(3, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[6]);
	glColorPointer(2, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[9]);	

	glDrawArrays(GL_QUADS, 0, mesh->quad_length); // draw quads

	glVertexPointer(3, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, mesh->vertex[mesh->quad_length * VERTEX_PARAM_COUNT]); // set pointers to arrays to begining for tris
	glNormalPointer(GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[mesh->quad_length * VERTEX_PARAM_COUNT + 3]);
	glColorPointer(3, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[mesh->quad_length * VERTEX_PARAM_COUNT + 6]);
	glColorPointer(2, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[mesh->quad_length * VERTEX_PARAM_COUNT + 9]);	

	glDrawArrays(GL_TRIANGLES, 0, mesh->tris_length); // draw tris
	vetk_destroy_mesh(mesh); // destroy mesh
}

--------------------4, drawing indexed quads and tris


#define VERTEX_PARAM_COUNT 12

char *vertex_params[VERTEX_PARAM_COUNT] = {VETK_VP_VERTEX_X, VETK_VP_VERTEX_Y, VETK_VP_VERTEX_Z, VETK_VP_NORMAL_X, VETK_VP_NORMAL_Y, VETK_VP_NORMAL_Z, VETK_VP_COLOR_R, VETK_VP_COLOR_G, VETK_VP_COLOR_B, VETK_VP_MAPPING_U, VETK_VP_MAPPING_V, VETK_VP_EMPTY};
VETKQuadTri *mesh;

mesh = vetk_create_mesh(node, vertex_params, VERTEX_PARAM_COUNT, TRUE, TRUE); // create optimized data
if(mesh != NULL)
{
	glEnableClientState(GL_VERTEX_ARRAY); // enable arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, mesh->vertex); // set pointers to arrays to begining for tris
	glNormalPointer(GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[3]);
	glColorPointer(3, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[6]);
	glColorPointer(2, GL_FLOAT, sizeof(float) * VERTEX_PARAM_COUNT, &mesh->vertex[9]);	

	glDrawElements(GL_QUADS, mesh->quad_length, GL_UNSIGNED_INT, mesh->ref); // draw
	glDrawElements(GL_TRIANGLES, mesh->tri_length, GL_UNSIGNED_INT, &mesh->ref[mesh->quad_length]); // draw
	vetk_destroy_mesh(mesh); // destroy mesh
}
*/