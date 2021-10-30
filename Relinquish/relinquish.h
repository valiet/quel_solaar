#if defined __cplusplus
extern "C" {
#endif
/*  -------------- Reliquish --------------------   
Relinquish is a Rendering library that wraps arround OpenGL. It is intended to smooth out some of the roughness of OpenGL. Instead of beeing a state machine, it has a object model where state is encapsulated in to objects and then boud. This counteracts the common problems of state leaks. Reliqush also provides many convinient features such as shader parsing and a matrix stack.

Reliquish is written to create a base line of hardware abstraction that allows the user to write portable graphics code that is backwards compatible and should run on any OpenGL 2.0 / DX9 level hardware. It also fully supports OpenGL ES 2.0 so any code written using Relinqush should easily be portable to mobile/embedded devices. The future plan is to port Reliqush over to Vulcan once it becomes available.

If you are writing a high-end 3D engine, Reliquish may not provide all the functionality one would need. Relinquish has been designed to be used for things like UIs and tools where it is important to have a strong foundation of stabillity, portability and long therm compatibility rather then access the the latest hardware features. To facilitate the mixing and matching of Relinqush dependent code and other OpenGL code the library has the abillity to reset and restore its state, in order to play nice with other code.
   */



/* ------ Initialization ------
 */

extern boolean r_init(void *(*glGetProcAddr)(const char* proc)); /* r_init initializes Relinquish and returns TRUE if it sucseeds and FAIL if the hardware does not support the required features. As of right now the Reliquish requires  GL_ARB_shading_language_100 and  GL_ARB_framebuffer_object. The funtion requires a function pointer to the systems procAddressGet in order to be able to obtain extention pointers */

extern boolean r_extension_test(const char *string); /* r_extention test returns TRUE or FALSE is any given Extention is available. This funcetion should not be needed as all funtionality in Relinquish should be available. */
extern void *r_extension_get_address(const char* proc); /* r_extention_get_address returns the function pointer to any function in OpenGL. This funcetion should not be needed as all funtionality needed to use Relinquish should be encapsulated. */


/* ------ Vertex array data ------
Relinquish has an ecapsulated array storage object. The  vertex array will if possible be saved in GPU memory. In order to create a vertex array object one must first define the vertext format. The data is always stored in an inteleaved array form maximum cash coherency. The shader attributes will automaticaly be matched to the vertex array by order. 

Example:

Vertex Shader code:

attribute vec3 vertex;
attribute vec2 uv;
attribute vec4 color;

Can then be feed by a array defined like this:

SUIFormats vertex_format_types[3] = {SUI_FLOAT, SUI_UINT16, SUI_UINT8};
uint vertex_format_size[3] = {3, 2, 4};
pool = r_array_allocate(uvertex_count, vertex_format_types, vertex_format_size, vertex_format_count, 0);

Vertex arrys are generaly reffered as "pools" beacuse they can be used to store multiple sets of geometries with the same vertext format. Drawing multiple objects form the same pools can be significantly faster then drawing data from different pools. All pools can be used directly, but to facilitate the use of a single pool for multiple objects one can allocate Sections of pools. Even with out using sections, parts of pools can be updated and drawn. The advantage of using sections is that Relinquish can itself breporm defragmentation of larg pools where sections are being continiusly allocated and freed.
*/


/* ------ Vertex array data ------
*/

typedef enum{
	R_SIT_BOOL,
	R_SIT_VBOOL2,
	R_SIT_VBOOL3,
	R_SIT_VBOOL4,
	R_SIT_INT,
	R_SIT_VINT2,
	R_SIT_VINT3,
	R_SIT_VINT4,
	R_SIT_UINT,
	R_SIT_VUINT2,
	R_SIT_VUINT3,
	R_SIT_VUINT4,
	R_SIT_FLOAT,
	R_SIT_VEC2,
	R_SIT_VEC3,
	R_SIT_VEC4,
	R_SIT_DOUBLE,
	R_SIT_VDOUBLE2,
	R_SIT_VDOUBLE3,
	R_SIT_VDOUBLE4,
	R_SIT_MAT2,
	R_SIT_MAT2X3,
	R_SIT_MAT2X4,
	R_SIT_MAT3X2,
	R_SIT_MAT3,
	R_SIT_MAT3X4,
	R_SIT_MAT4X2,
	R_SIT_MAT4X3,
	R_SIT_MAT4,
	R_SIT_DMAT2,
	R_SIT_DMAT2X3,
	R_SIT_DMAT2X4,
	R_SIT_DMAT3X2,
	R_SIT_DMAT3,
	R_SIT_DMAT3X4,
	R_SIT_DMAT4X2,
	R_SIT_DMAT4X3,
	R_SIT_DMAT4,
	R_SIT_SAMPLER_1D,
	R_SIT_SAMPLER_2D,
	R_SIT_SAMPLER_3D,
	R_SIT_SAMPLER_CUBE,
	R_SIT_SAMPLER_RECTANGLE,
	R_SIT_SAMPLER_1D_ARRAY,
	R_SIT_SAMPLER_2D_ARRAY,
	R_SIT_SAMPLER_CUBE_MAP_ARRAY,
	R_SIT_SAMPLER_BUFFER,
	R_SIT_SAMPLER_2D_MULTISAMPLE,
	R_SIT_SAMPLER_2D_MULTISAMPLE_ARRAY,
	R_SIT_COUNT
}RInputType;

#ifndef RELINQUISH_INTERNAL

typedef void RShader;

#endif

/* ------ Array data  ------
This structure describes a single pointer */

typedef enum{
	SUI_INT8,
	SUI_UINT8,
	SUI_INT16,
	SUI_UINT16,
	SUI_INT32,
	SUI_UINT32,
	SUI_FLOAT,
	SUI_DOUBLE
}SUIFormats;


extern uint 	r_array_vertex_size(SUIFormats *vertex_format_types, uint *vertex_format_size, uint vertex_format_count); /* computes size in bytes of a specific vertext format. */

typedef enum{
	R_PRIMITIVE_POINTS = 0x0000, /*Draw point primitives.*/
	R_PRIMITIVE_LINES = 0x0001, /*Draw Line primitives.*/
	R_PRIMITIVE_TRIANGLES = 0x0004 /*Draw Triangle primitives*/
}RPrimitive;

extern void 	*r_array_allocate(uint vertex_count, SUIFormats *vertex_format_types, uint *vertex_format_size, uint vertex_format_count, uint reference_count); /* Allocates a pool of vertex_count number of vertices */
extern void		*r_array_allocate_from_shader(RShader *s, uint vertex_count, uint reference_count, boolean in);
extern void		r_array_free(void *pool); /* Frees a vertex array pool. */
extern boolean	r_array_defrag(void *pool); /* Moves one section of vertex data in a pool in order to defragmentate it. The funtion returns TRUE if a section was moved, and FALSE if there are no gaps in the vertex array left to collaps. In order to refragmet a pool entierly, simply call: while(r_array_defrag(pool));*/
extern void 	*r_array_section_allocate_vertex(void *pool, uint size); /* Allocates a section from pool consisting of size number of vertices. Will return a handle to the section or NULL if the pool does not have enough space to left.*/  
extern void 	*r_array_section_allocate_reference(void *pool, uint size);/* Allocates a section from pool consisting of size number of 32bit reference values. Will return a handle to the section or NULL if the pool does not have enough space to left.*/ 
extern void		r_array_section_free(void *pool, void *section); /* REturn the memory used by a section to the pool and free the handle. */

extern void		r_array_load_vertex(void *pool, void *section, void *data, uint start, uint length); /* loads interleaved vertex data in to a pool or section of a pool. If section is NULL vertex data will be loaded directly in to the pool. Start and count indicate the where the data is inserted and how long the data is. Both are counted in number of veritices. */ 
extern void		r_array_load_reference(void *pool, void *section, void *referencing_section, uint *data, uint length); /* loads an array of 32 bit uint data in to a section of a pool. Start and count indicate the where the data is inserted and how long the data is. Both are counted in number of vertex refferences. */ 

extern uint		r_array_get_size(void *pool); /* Returns the size in vertices of a pool */
extern uint		r_array_get_used(void *pool); /* Returns the number of vertices that have been allocated in to sections of a pool */
extern uint		r_array_get_left(void *pool); /* Returns the number of vertices that have not yet been allocated in to sections of a pool */

extern uint		r_array_get_vertex_size(void *pool); /* Returns the size in vertices of a pool. */
extern uint		r_array_section_get_vertex_count(void *section); /* returns the number of vertices of storage in a section. */

extern void		r_array_debug_print_out(void *pool); /* Prints out the vertex format of a pool of standard out. (For debug purpouses)*/


/* ------ Draw calls  ------
These calls are beeing used to draw geometry. All drawcalls have the abillity to draw multiple objects at once. Each individual object will get its instance uniform block from either an array in user memory or a array pool. It is recomended to build up arrays of ojects and draw them in a single call rather them making many individual calls. Memory arrays are recomended for dynamic data created by the CPU, where as a array object is faster if the data is static or is generated by the GPU.*/

extern void		r_array_draw(void *pool, void *section, RPrimitive primitive_type, uint vertex_start, uint vertex_count, uint8 *uniforms, void *array_pool, uint count); /*Draws one or more identical objects reading vertex data from "pool", section is an optinal vertex or refference section from pool. Primitive_type is the primitive beeing drawn. Vertex start is the first vertex in the pool/section to be drawn. vertex_count is the maximum number of veritices drawn for each instance. If the number of vertices in the pool/section is less then vertex_count, then only the availabe veriteces will be drawn. Uniforms is a pointer to an array of uniform data. The data needs to be the size of the instance block (that can be obtained by calling r_shader_uniform_block_size) times the number of instances. array_pool is an alternative way of feeding the draw call with unifom data using a array pool. count is the numer of instances beeing drawn.*/ 
extern void		r_array_sections_draw(void *pool, void **section, RPrimitive primitive_type, uint8 *uniforms, void *array_pool, uint count); /*Draws multiple objects reading vertex data from "pool". Section is an array of pointers to sections allocated from the pool. All sectons can only contain vertex data. Primitive_type is the primitive beeing drawn. Uniforms is a pointer to an array of uniform data. The data needs to be the size of the instance block (that can be obtained by calling r_shader_uniform_block_size) times the number of instances. Array_pool is an alternative way of feeding the draw call with unifom data using a array pool. Count is the number of objects being drawn. */ 
extern void		r_array_references_draw(void *pool, void **section, RPrimitive primitive_type, void *uniforms, void *array_pool, uint count); /*Draws multiple objects reading vertex data from "pool". Section is an array of pointers to sections allocated from the pool. All sectons can only contain reference data. Primitive_type is the primitive beeing drawn. Uniforms is a pointer to an array of uniform data. The data needs to be the size of the instance block (that can be obtained by calling r_shader_uniform_block_size) times the number of instances. Array_pool is an alternative way of feeding the draw call with unifom data using a array pool. Count is the number of objects being drawn. */ 

extern void		r_array_vertex_render(void *in_pool, void *out_pool); /* Renders a vertex pool to another ppol. All vertices gets processed by the currently bound shaders. The output vertex array pools format needs toi match the format of the outupt of the vertex/geopmetry shaders. */

extern void		r_array_section_draw(void *pool, void *section, RPrimitive primitive_type, uint vertex_start, uint vertex_count); /* deprecated drawcall replaced by r_array_draw.*/
extern void		r_array_reference_draw(void *pool, void *section, RPrimitive primitive_type, uint vertex_start, uint vertex_count); /* deprecated drawcall replaced by r_array_draw.*/

/* ---- Matrix management ------
Reliquish has a comprehensive projection matrix system that in many ways mirrors the OpenGL 1.x matrix stack. Instead of having a single matrix users can allocate as many matrices as needed and then bind and unbind them for each call. Relinquish also has a default Matrix that can be accessed by bunding matrix NULL. All matrix operations talk a matrix pointer. if this matrix pointer is set to NULL, the currently bound Matrix will be used. Any shader using uniforms of type mat4 with names "NormalMatrix", "ModelViewMatrix", "ProjectionMatrix", and "ModelViewProjectionMatrix" will automaticaly be set in any uniform memory not beeing feed form a pointer of array object. Using the function  r_shader_uniform_matrix_set a block of memory beeing prepared to be used as a instance unifom block can have its matrices set from a Relinquish matrix stack.*/

#define R_MATRIX_STACK_SIZE 8

typedef struct{
	float matrix[R_MATRIX_STACK_SIZE][16];
	float projection[16];
	float aspect;
	float frustum[4];
	boolean multiplied;
	float modelviewprojection[16];	
	uint current;
}RMatrix;

extern void		r_matrix_identity(RMatrix *matrix); /*Clears and initializes the matrix.*/
extern void		r_matrix_push(RMatrix *matrix); /* Push the matrix stack one level.*/
extern void		r_matrix_pop(RMatrix *matrix); /* Pops the matrix one level.*/
extern void		r_matrix_matrix_mult(RMatrix *matrix, float *mult); /* multiplies a 4by4 float matrix with the current matrix.*/
extern void		r_matrix_rotate(RMatrix *matrix, float angle, float x, float y, float z); /* Rotates the matrix angle degrees around the x, y, and z axis.*/
extern void		r_matrix_scale(RMatrix *matrix, float x, float y, float z); /*Scales the matrix in x, y, and z.*/
extern void		r_matrix_translate(RMatrix *matrix, float x, float y, float z); /*Translates the matrix in x, y, and z.*/
extern void		r_matrix_normalize_scale(RMatrix *matrix); /*Normalises the scale of the matrix.*/

extern void		r_matrix_frustum(RMatrix *matrix, float left, float right, float bottom, float top, float znear, float zfar); /*Sets up a frustum for a pespective projection matrix*/
extern void		r_matrix_ortho(RMatrix *matrix, float left, float right, float bottom, float top, float znear, float zfar); /*Sets up a frustum for a ortogonal projection*/

extern void		r_matrix_set(RMatrix *matrix); /*Sets the currently active matrix.*/
extern RMatrix		*r_matrix_get();/* Returns a pointer to the currently bound matrix. It will return a pointer to the default matrix if matrix NULL is bound. */

extern void r_matrix_projection_screend(RMatrix *matrix, double *output, double x, double y, double z);/* transforms double precission x, y, and z from worldspace coordinates in to screen space coordinates. */
extern void r_matrix_projection_screenf(RMatrix *matrix, float *output, float x, float y, float z);/* transforms single precission x, y, and z from worldspace coordinates in to screen space coordinates.*/
extern void r_matrix_projection_worldf(RMatrix *matrix, float *output, float x, float y, float z);/* transforms single precission x, y, and z from screen space coordinates to world corrdinates.*/
extern void r_matrix_projection_vertexd(RMatrix *matrix, double *output, double *vertex, double x, double y);/* transforms double precission screen space x and y at the depth of vertex (in world coordinates), on to world coordinates.*/
extern void r_matrix_projection_vertexf(RMatrix *matrix, float *output, float *vertex, float x, float y);/* transforms single precission screen space x and y at the depth of vertex (in world coordinates), on to world coordinates.*/


extern boolean r_matrix_projection_surfacef(RMatrix *matrix, float *output, float *pos, uint axis, float x, float y);
extern boolean r_matrix_projection_axisf(RMatrix *matrix, float *output, float *pos, uint axis, float x, float y);
extern boolean r_matrix_projection_vectorf(RMatrix *matrix, float *output, float *pos, float *vec, float x, float y);

extern void r_viewport(uint x_start, uint y_start, uint x_end, uint y_end); /* sets, in pixes the view port where Relinquish will draw.*/

/*-------- Shader management -------
The shader object stores a shader, and assosiated state. Relinquish will edit the shaders to create and vectorize uniform blocks to make it possible to speed up rendring if the hardware supports it. Relinquish always tries to use the most efective renedering path for the hardware available. All uniform data can be set either indivudualy as state stored in the shader or be given as memory blocks.*/

extern RShader	*r_shader_create(char *debug_output, uint output_size, char **shaders, uint *stages, uint stage_count, char *name, char *instance_block); /* Creates a shader from anumbert of shaders. The fuinction returns NULL if it fails to build a shader. The function accepts a pointer to a debug_output buffer of output_size to wich any error message will be written. shaders is an array of poinerts to the null terminated tesxt strings storing the GLSL shader code. Stages indicates the type of each shader, and stage_count the number of shaders and stages to be compiled. Name, is the name of the shader and is only used for debugging purpouses. instance_block is the name of the uniform block (if there are more then one) that should be vectorized so that mult instance draws can be done with different uniform values. If NULL is given the first uniform block is used. If the shader doesnt have any uniform blocks one will be created automaticly that includes all uniforms. It is recomended not to have any textures in the instance uniform block for preformance reasons.*/
extern RShader	*r_shader_create_simple(char *debug_output, uint output_size, char *vertex, char *fragment, char *name); /*This is aconvinience function wrapping r_shader_create that assumes onliu one vertex and one fragment shader. */
extern RShader	*r_shader_create_from_file(char *debug_output, uint output_size, char *vertex_file, char *fragment_file, char *name); /* Same as r_shader_create_simple, but loads the two shaders from files. */
extern void		r_shader_destroy(RShader *shader); /* Destroy a shader */
extern boolean	r_shader_set(RShader *shader); /* Set the current shader to be used when drawing */

extern void		r_shader_texture_set(RShader *shader, uint slot, uint texture_id); /* Binds a texture to a shader. "Slot" corresponds to the texture sample in thje order that they appear in the shader. It is recomended to use r_shader_uniform_texture_set instead of this function. */

/*-------- Uniform query -------
Allows an application to query the available uniform blocks, their members and */
extern uint		r_shader_uniform_block_count(RShader *shader);  /* Returns the number of uniform blocks used by a shader.*/
extern uint		r_shader_uniform_block_size(RShader *shader, uint block); /* Returns the size of a specific uniform Block in bytes.*/
extern uint		r_shader_uniform_block_member_count(RShader *shader, uint block); /*Lists the number of members a specific block has. */
extern char		*r_shader_uniform_block_member_name(RShader *shader, uint block, uint member); /*returns the name of a member uniform from a block*/
extern RInputType r_shader_uniform_block_member_type(RShader *shader, uint block, uint member); /*returns the type of a member uniform from a block*/
extern uint		r_shader_uniform_block_member_offset(RShader *shader, uint block, uint member); /*returns the offset of a member uniform from a block*/

extern uint		r_shader_uniform_block_location(RShader *shader, char *name); /* Returns the offset, in number of bytes where a specific value will be read in a uniform block */

/*-------- Uniform blocks -------
In order to construct a uniform block, Relinquish provides a API to query the shader about the size and content of each uniform block.*/
extern void		r_shader_uniform_data_set(RShader *shader, void *data, uint block_id); /*Over writes the shaders internal storage of unifoms for a block with data provided. Is potentialy significantly faster the individualy setting the values.*/
extern void		r_shader_uniform_matrix_set(RShader *shader, uint8 *uniform_data, uint block_id, RMatrix *matrix);/*If a memory block contains any of the predefined matrix uniforms, this function can automaticly sett the current uniform data state in to a user provided memory block. */
extern uint64	r_shader_uniform_texture_pointer_get(uint texture_id); /* Allows the user to converta texture id in to a 64 bit value that can be set directly in to a memory block. */

/*-------- Uniform State -------
In order to construct a uniform block, Relinquish provides a API to query the shader about the size and content of each uniform block.*/

extern uint		r_shader_uniform_location(RShader *shader, char *name); /* Allows the application to query the location of a uniform value in a shader. Note that fis is a different location then the block offset */
extern void		r_shader_uniform_texture_set(RShader *shader, uint location, uint64 texture_id); /*Sets a texture address (obtained using r_shader_uniform_texture_pointer_get) in unifomr memory state*/
extern void		r_shader_mat2v_set(RShader *shader, uint location, float *matrix); /*Sets a mat2 value in the uniform state. */
extern void		r_shader_mat3v_set(RShader *shader, uint location, float *matrix); /*Sets a mat3 value in the uniform state. */
extern void		r_shader_mat4v_set(RShader *shader, uint location, float *matrix); /*Sets a mat4 value in the uniform state. */

extern void		r_shader_vec4_set(RShader *shader, uint location, float v0, float v1, float v2, float v3);/*Sets a vec4 value in the uniform state. */
extern void		r_shader_vec3_set(RShader *shader, uint location, float v0, float v1, float v2);/*Sets a vec3 value in the uniform state. */
extern void		r_shader_vec2_set(RShader *shader, uint location, float v0, float v1);/*Sets a vec2 value in the uniform state. */
extern void		r_shader_float_set(RShader *shader, uint location, float f);/*Sets a float value in the uniform state. */

extern void		r_shader_int_set(RShader *shader, uint location, int i); /*Sets a int value in the uniform state. */
extern void		r_shader_int2_set(RShader *shader, uint location, int i0, int i1); /*Sets a ivec2 value in the uniform state. */
extern void		r_shader_int3_set(RShader *shader, uint location, int i0, int i1, int i2);/*Sets a ivec3 value in the uniform state. */
extern void		r_shader_int4_set(RShader *shader, uint location, int i0, int i1, int i2, int i3);/*Sets a ivec4 value in the uniform state. */


extern uint		*r_shader_input_fomat_components_get(RShader *s);
extern uint		r_shader_input_fomat_count_get(RShader *s);
extern uint		*r_shader_output_fomat_components_get(RShader *s);
extern uint		r_shader_output_fomat_count_get(RShader *s);

/* ------- Shader state ------------
Each shader also stores fiuxed function state that is automaticaly set when a shader is bound.*/

extern void		r_shader_state_set_depth_test(RShader *shader, uint depth_test); /* Sets the shaders depth opperation.*/
extern void		r_shader_state_set_cull_face(RShader *shader, uint cull_face); /* Sets the shader back face cullingh state.*/
extern void		r_shader_state_set_blend_mode(RShader *shader, uint blend_source, uint blend_destination); /* Controls the shaders source and destinating blend factor. */
extern void		r_shader_state_set_offset(RShader *shader, float offset_factor, float offset_units); /* setts the depth offset_factor and units for multipas rendring. */
extern void		r_shader_state_set_alpha_to_coverage(RShader *shader, boolean alpha_to_coverage); /*Enabeles of disables Alpha to coverage (not valable on all hardware).*/
extern void		r_shader_state_set_mask(RShader *shader, boolean red, boolean green, boolean blue, boolean alpha, boolean depth); /* Controms the shaders chanel mask where the rendering of individual components can be turned on and off.*/ 



/* ------- Shader Debugging ------------
These functions can be used to introspect a shader or a uniform block in order to help with debugging. These functions are not intended for release code but rathere as handy tools for debugging.*/

extern void		r_shader_debug_print_shader(RShader *shader);/*Prints out of stdout the currents state of the shader including, name its inputs and outputs, uniforms and unifomr blocks as will as the current uniform state.*/
extern void		r_shader_debug_print_uniform_buffer(RShader *shader, uint8 *uniform_buffer, char *text, uint buffer_id); /* prints out the contnent of a user provided uniform buffer wiuth buffer_id. If uniform_buffer is set to NULL the shaders interanl uniform buffer storage will be printed. If uniform_buffer is set to -1 all internal unifomr buffers will be printed. If provided , the param text will provide a headline form the print out.*/




/* ------- Draw primitives ------------
To make it easy toi get up and running and for debugging Reliquish provides a number of built in shaders and primitives. */

typedef enum{
	P_SP_COLOR_UNIFORM, /* a single colored (accessed using the uniform "color") surface transformed by "ModelViewProjectionMatrix". it wiull use the first component of the vertex array as its position. */
	P_SP_COLOR_VERTEX, /* Same as P_SP_COLOR_UNIFORM, but will use the second component of the vertex array as surface color instead of a uniform. */
	P_SP_TEXTURE, /*Draws a textured surface where the first array indicates position and the second uv.*/
	P_SP_COLORED_UNIFORM_TEXTURE /*Same as P_SP_TEXTURE but adds the uniform "color" to shade the textured surface. */
}RShaderPresets;

extern RShader	*r_shader_presets_get(RShaderPresets preset); /* This function returns the pointer to one of the built in shaders.*/


/* ------- Surface primitives ------------
To make it easy to get up and running and for debugging Reliquish provides a number of built in surface primitives. */

extern void		r_primitive_surface(float pos_x, float pos_y, float pos_z, float size_x, float size_y, float red, float green, float blue, float alpha); /* draws a mono colored rectangle. */
extern void		r_primitive_image(float pos_x, float pos_y, float pos_z, float size_x, float size_y, float u_start, float v_start, float u_end, float v_end, uint texture_id, float red, float green, float blue, float alpha); /*draws a textured and shaded rectangle.*/

/* ------- Line primitives ------------
To make it easy to get up and running and for debugging Reliquish provides a number of calls that draw individual line segments. The line segments are stored in an internal buffer in order to be able to draw many line segments with a single drawcall. It is therefore imprtant to call r_primitive_line_flush(); to flush the internal buffer. Since the internal buffer has a limited size any primitive line call may cause Relinquish to flush all lines. It is therefor important to have the same projection matrix bound both during line calls and flushing. */

extern void		r_primitive_line_3d(float start_x, float start_y, float start_z, float end_x, float end_y, float end_z, float red, float green, float blue, float alpha); /* draws a monocolored 3D line from start to end. After calling any number of this of other r_primitive_line_... calls, a call to r_primitive_line_flush() is required */
extern void		r_primitive_line_2d(float start_x, float start_y, float end_x, float end_y, float red, float green, float blue, float alpha); /* draws a monocolored 2D line from start to end. After calling any number of this of other r_primitive_line_... calls, a call to r_primitive_line_flush() is required */
extern void		r_primitive_line_fade_3d(float start_x, float start_y, float start_z, float end_x, float end_y, float end_z, float start_red, float start_green, float start_blue, float start_alpha, float end_red, float end_green, float end_blue, float end_alpha); /* draws a fading 3d line from start to end. After calling any number of this of other r_primitive_line_... calls, a call to r_primitive_line_flush() is required */
extern void		r_primitive_line_fade_2d(float start_x, float start_y, float end_x, float end_y, float start_red, float start_green, float start_blue, float start_alpha, float end_red, float end_green, float end_blue, float end_alphaa);/* draws a fading 2D line from start to end. After calling any number of this of other r_primitive_line_... calls, a call to r_primitive_line_flush() is required */
extern void		r_primitive_line_flush(); /*Flushed all r_primitive_line... calls that has not yet been drawn. */
extern void		r_primitive_line_clear(); /*Clears the internal line buffer but without drawing any of the lines. Since the size of the internal buffer is unknown the use of this function is discurraged.*/


/* ------- Sprite primitives ------------
Line primitive Sprites.*/


extern void		r_primitive_sprite(float pos_x, float pos_y, float pos_z, uint texture_id, float size_x, float size_y, float red, float green, float blue, float alpha);
extern void		r_primitive_sprite_uv(float pos_x, float pos_y, float pos_z, uint texture_id, float size_x, float size_y, float u_start, float v_start, float u_end, float v_end, float red, float green, float blue, float alpha);
extern void		r_primitive_sprite_flush();


/* ------- Image and texture API ------------
Line primitive Sprites.*/


typedef enum{
	R_IF_R_UINT8, /* Single chanel texture format with 8 bit integer storage corresponding to a float in GLSL */
	R_IF_RG_UINT8, /* Dual chanel texture format with 8 bit integer storage corresponding to a vec2 in GLSL */
	R_IF_RGB_UINT8, /* Tripple chanel texture format with 8 bit integer storage corresponding to a vec3 in GLSL */
	R_IF_RGBA_UINT8, /* Quadruple chanel texture format with 8 bit integer storage corresponding to a vec4 in GLSL */
	R_IF_R_FLOAT16,/* Single chanel texture format with 16 bit floating point storage corresponding to a float in GLSL */
	R_IF_RG_FLOAT16, /* Dual chanel texture format with 16 bit floating point storage corresponding to a vec2 in GLSL */
	R_IF_RGB_FLOAT16, /* Tripple chanel texture format with 16 bit floating point storage corresponding to a vec3 in GLSL */
	R_IF_RGBA_FLOAT16, /* Quadruple chanel texture format with 16 bit floating point storage corresponding to a vec4 in GLSL */
	R_IF_R_FLOAT32,/* Single chanel texture format with 32 bit floating point storage corresponding to a float in GLSL */
	R_IF_RG_FLOAT32, /* Dual chanel texture format with 32 bit floating point storage corresponding to a vec2 in GLSL */
	R_IF_RGB_FLOAT32, /* Tripple chanel texture format with 32 bit floating point storage corresponding to a vec3 in GLSL */
	R_IF_RGBA_FLOAT32, /* Quadruple chanel texture format with 32 bit floating point storage corresponding to a vec4 in GLSL */
	R_IF_DEPTH16, /*16 Bit depth map.*/
	R_IF_DEPTH24, /*24 Bit depth map.*/
	R_IF_DEPTH32, /*32 Bit depth map.*/
	R_IF_DEPTH24_STENCIL8, /*24 Bit depth map interleaved with a 8 bit stencil map.*/
}PImageFormat;

extern uint		r_texture_allocate(PImageFormat format, uint x, uint y, uint z, boolean filter, boolean tile, void *data); /* Allocates an image. If z is set to 1 the image will be 2D, if the z  is set to R_IB_IMAGE_CUBE a cube map is created where all six sides are x*y resolution. filter, and tiled are booleans that indicates if the resulting image should be filerd and tiled respectivly. The data pointer is a user provided pointer of data to fill up the texture. If data is set to NULL, Relinquish will generate a pattern as a defalut image to aid in debugging.*/
extern void		r_texture_free(uint texture_id); /*Frees a texture. */

#define R_IB_IMAGE_CUBE -1

/* Frame buffer objects */

extern uint		r_framebuffer_allocate(uint *buffers, uint buffer_count, uint depth_buffer); /* Creates a FBO form an array of textures and/or a depth/stencil buffer. All buffers has to be of the same size. it will return -1 if the different texture and depth formats are not renderable or compatible.*/
extern void		r_framebuffer_free(uint fbo); /*Free a Frame buffer object. */
extern void		r_framebuffer_bind(uint id); /* Binds a frame buffer object*/
extern void		r_framebuffer_clear(float red, float green, float blue, float alpha, boolean color, boolean depth_stencil); /* Clear the current frame buffer oblect or screenbuffer.*/


#if defined __cplusplus
}
#endif