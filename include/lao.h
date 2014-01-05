#ifndef LAO_H
#define LAO_H

#include <stdarg.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

/* Constants */

#define LAO_NAME "liblao"
#define LAO_VERSION "0.0.1"


/** \defgroup util Utility functions
 */

/** \defgroup bufferobjs OpenGL buffer objects
 */



/** \name Path constants
 *
 * These are the directories the game resources are loaded from
 * relative to the directory the working directory.
 * @{
 */
 /**\ingroup resources
*/


#define LAO_GFX_PATH "gfx"
#define LAO_SHADER_PATH "shader"
#define LAO_MODEL_PATH "models"

/**@}
 */

/* in case we are not using gnu extensions */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/** \brief VBO size
 *
 * The still constant size of the vbo. We have to fix this.
 *
 * \ingroup bufferobjs
 */

enum {
	LAO_VBO_SIZE = 8192 /* * sizeof(LaoVertex) */
};

/* Config */

/** \defgroup config Configuration loader
 *
 * These functions define a key-value format config reader for
 * different uses in the game.
 *
 * The format is
 *
 *		key = value
 *
 * where `value` can be of type int, float or string.
 *
 * The following example illustrates the use of the configuration parser.
 * You have to specify all the config key definitions and allocate an array
 * of LaoConfigKeyValues to hold the parsed values. The size of that array
 * has to be greater than the highest key index of your keys.
 *
 * 		enum {
 *				CFG_FULLSCREEN = 0,
 *				CFG_WINDOW_W,
 *				CFG_WINDOW_H,
 *				_CFG_KEY_COUNT
 *			};
 *
 * 		LaoConfigDef configdefs[] = {
 *				{CFGT_NUM, CFG_FULLSCREEN, "fullscreen", {.num = 0}},
 *				{CFGT_NUM, CFG_WINDOW_W, "window_w", {.num = 1024}},
 *				{CFGT_NUM, CFG_WINDOW_H, "window_h", {.num = 576}},
 *				{0,0,0,{.num = 0}}
 *			};
 *
 * 		LaoConfigValue config[_CFG_KEY_COUNT];
 *
 * Then you can load and access the config.
 *
 * 		lao_config_load(CONFIG_FILE, config, &configdefs);
 * 		init_video(config[CFG_WINDOW_W].num, config[CFG_WINDOW_H].num);
 * 		config[CFG_FULLSCREEN] = 0;
 *
 * At the end of your program you can save and free the configuration data.
 *
 * 		lao_config_save(CONFIG_FILE, config, &configdefs);
 * 		lao_config_free(config, &configdefs);
 *
 * I admit it's a bit strange, but this way you have O(1) r/w access with
 * minimal housekeeping to do.
 * @{
 */

typedef union LaoConfigValue LaoConfigValue;
/** The config parser will handle the values
 * as an array of LaoConfigValues.
 */
union LaoConfigValue {
	char *str; /**< string value */
	int num; /**< integer value */
	float flt; /**< floating point value */
};

/** Types of Config Values.
 */

enum LaoConfigKeyType {
	CFGT_NUM, /**< integer value */
	CFGT_STR, /**< string value */
	CFGT_FLT /**< floating point value */
};


typedef struct LaoConfigDef LaoConfigDef;
/** Config Key Definition.
 *
 * To start parsing, an null terminated array of LaoConfigDefs
 * has to be passed to the Parser. It is used to determine how the
 * keys in the config should be interpreted.
 */
struct LaoConfigDef {
	/** The \ref LaoConfigKeyType of the key.*/
	int type;
	/** The index in the output array. */
	int key;
	/** The key name inside the file. */
	char *name;
	/** A preset value for this config key. */
	LaoConfigValue preset;
};


/** Set all config values to their presets.
 * \param[out] confvals Array the config values are stored in.
 * \param[in] confdefs Array of config key definitions. Terminated by a zeroed.
 *
 * \warning The size of `confvals` has to greater than the highest
 * 			`ConfigDef::key` in the `confdefs` array.
 */
void lao_cfg_preset(LaoConfigValue *confvals, LaoConfigDef *confdefs);

/** Load a configuration file.
 * \param[in] filename Configuration filename.
 * \param[out] confvals Array to store the configuration values in.
 * \param[in] confdefs Array of configuration key definitions. Terminated by a zeroed element.
 *
 * \warning The size of `confvals` has to greater than the highest
 * 			`ConfigDef::key` in the `confdefs` array.
 */
void lao_cfg_load(char *filename, LaoConfigValue *confvals, LaoConfigDef *confdefs);

/** Save a configuration file.
 * \param[in] filename Configuration filename.
 * \param[in] confvals Array the config values are stored in.
 * \param[in] confdefs Array of configuration key definitions. Terminated by a zeroed element.
 *
 * \warning The size of `confvals` has to greater than the highest
 * 			`ConfigDef::key` in the `confdefs` array.
 */
void lao_cfg_save(char *filename, LaoConfigValue *confvals, LaoConfigDef *confdefs);

/** Free all loaded configuration data.
 * \param confvals Array the config values are stored in.
 * \param confdefs Array of configuration key definitions. Terminated by a zeroed element.
 *
 * \warning The size of `confvals` has to greater than the highest
 * 			`ConfigDef::key` in the `confdefs` array.
 */
void lao_cfg_free(LaoConfigValue *confvals, LaoConfigDef *confdefs);

/** @} */

/* Util/Native */
/** \defgroup native Native
 * Crossplatform function wrappers for different purposes.
 *
 * \ingroup util
 * @{
 */
/** \brief Cross-platform chdir.
 *
 * \param[in] path Path to change to.
 * \return 0 on success, -1 to indicate error.
 */
int lao_chdir(const char *path);
/**
 * @}
 */

/* Util/List */
/** \defgroup list List
 * \ingroup util
 *
 * A linked-list implementation. The data is handled as a void pointer
 * so your code has to know what kind of data is in there.
 *
 * If you have never written something like this yourself, you are
 * encouraged to read the source code so you know what this
 * implementation does.
 *
 * @{
 */

typedef struct LaoListElement LaoListElement;
struct LaoListElement {
	LaoListElement *next;
	LaoListElement *prev;

	void *data;
};

typedef struct LaoList LaoList;
/** Linked-List Structure.
 * @sa lao_list_init
 * @sa lao_list_free
 */
struct LaoList {
	/** Pointer to the first element. */
	LaoListElement *first;
	/** Number of elements. */
	int size;
};

/** Initialize a list.
 *
 * This has to be called before using a list.
 * \param list List to be initialized.
 */
void lao_list_init(LaoList *list);

/** Add list element at the beginning.
 * \param list List to be modified.
 * \param data Data to be associated with the new element.
 */
void lao_list_add(LaoList *list, void *data);
/** Insert a new list element after an arbitrary element.
 * \param list List to be modified.
 * \param after Add the new Element after this one.
 * \param data Data of the new Element.
 */
void lao_list_insert(LaoList *list, LaoListElement *after, void *data);
/** Remove an element of the list.
 * \param list List to be modified.
 * \param element Element to be removed.
 */
void lao_list_remove(LaoList *list, LaoListElement *element);

/** Free a list.
 *
 * \param list List to be freed.
 * \warning This does not free the data.
 */
void lao_list_free(LaoList *list);

/** @}
 */
/* Util/Linear Algebra */
/** \defgroup linalg Linear Algebra
 * \ingroup util
 *
 * Several helper functions for doing linear algebra.
 * The API is somewhat GLSL-like, but doesn't hide
 * the uglyness and cost of the operations. Have
 * fun coding.
 * @{
 */

typedef float Vec3[3];

/** 3D Vector addition. */
void vec3_add(Vec3 dest, const Vec3 a, const Vec3 b);
/** 3D Multiplication by a scalar. */
void vec3_scale(Vec3 a, float f);
/** 3D dot product. */
float vec3_dot(const Vec3 a, const Vec3 b);
/** Cross product. */
void cross(Vec3 dest, const Vec3 a, const Vec3 b);

/** 3D length. */
float vec3_length(const Vec3 a);
/** Normalize a vector. */
void vec3_norm(Vec3 a);

typedef float Mat4[4][4];
typedef float Mat3[3][3];

/** 3×3 identity matrix */
extern const Mat3 _identity3;
/** 4×4 identity matrix */
extern const Mat4 _identity4;

/** Transpose a 3×3 matrix */
void transpose3(Mat3 dest, const Mat3 m);
/** Transpose a 4×4 matrix */
void transpose4(Mat4 dest, const Mat4 m);

/** Multiply two 3×3 matrices */
void matmul3(Mat3 dest, const Mat3 a, const Mat3 b);
/** Multiply two 4×4 matrices */
void matmul4(Mat4 dest, const Mat4 a, const Mat4 b);

/** Multiply a 3×3 matrices with a 3×1 vector */
void matvec3(Vec3 dest, const Mat3 m, const Vec3 v);

/** Pad a 3×3 matrix to 4×4 by inserting 0 in the fourth
 * row and column and 1 in the bottom right corner.
 */
void matpadding(Mat4 dest, const Mat3 m);

/** Create a projection matrix.
 *
 * \param[out] mat Destination Matrix.
 * \param[in] ratio Width to height ratio of the desired frustum.
 * \param[in] angle Field of View angle in radians.
 * \param[in] near Distance of near clipping plane.
 * \param[in] far Distance of far clipping plane.
 */

void lao_set_perspective(Mat4 mat, float ratio, float angle, float near, float far);
/** @}
 */


/* Log */

/** \defgroup log Logging
 * Simple logging capabilities.
 *
 * @{
 */
/** Print a log message.
 */
void lao_log_msg(const char *msg, ...);
/** Print a warning log message.
 */
void lao_log_warn(const char *msg, ...);
/** Print a fatal log message and terminate
 * the program.
 */
void lao_log_fatal(const char *msg, ...);
/** @}
 */


/* GL */

/** \defgroup gl OpenGL extension loading
 *
 * This part is responsible for providing modern OpenGL functions to
 * your code. This is needed because of the structure of OpenGL. The
 * things you think are functions are usually function pointers
 * that have to be set at some point.
 * Context creation libraries like GLFW or SDL don't do this for you.
 * You can use GLEW, but that's usually an unnecessary code dependency.
 *
 * @{
 */

/** Load all needed OpenGL procaddresses. This should
 * be called after a Context was created but before anything
 * else is done.
 *
 * \warning Not calling this will cause you horrible segfaults.
 */
void lao_gl_init(void);

#ifdef __WIN32__
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLBLENDEQUATIONPROC glBlendEquation;
#endif

/* PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;*/

PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBINDBUFFERRANGEPROC glBindBufferRange;

PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLDELETEPROGRAMPROC glDeleteProgram;

PFNGLCREATESHADERPROC glCreateShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLDELETESHADERPROC glDeleteShader;

PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;

PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4FPROC glUniform4f;

PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM2IPROC glUniform2i;
PFNGLUNIFORM3IPROC glUniform3i;
PFNGLUNIFORM4IPROC glUniform4i;

PFNGLUNIFORM2FVPROC glUniform2fv;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;

PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

/** @}
 */

/* Resources */
/** \defgroup resources Resource loader
 *
 * This is a simple Resource loader. It supports loading OBJ models,
 * PNG textures and GLSL shaders.
 * 
 * @{
 */

/** Load all resources.
 \param basepath Path containing the resource directories. */
void lao_resources_load(const char *basepath);
/** Free all resources */
void lao_resources_free(void);

/* Resources/Textures */

typedef struct LaoTexture LaoTexture;
struct LaoTexture {	
	/** Texture name */
	char *name;
	
	/** Width of texture */
	int w;
	/** Height of texture */
	int h;
	/** OpenGL texture handle */
	GLuint gltex;
};

/** Get a texture from the Resource collection.
 *
 * If the texture is not found, the program will be terminated.
 *
 * \param name Texture name.
 * \returns Pointer to the texture.
 */
LaoTexture *lao_get_tex(const char *name);

/* Resources/LaoModels */

typedef struct LaoModel LaoModel;
struct LaoModel {
	/** Model name */
	char *name;
	
	/** \cond
	 */
	int offset;
	int count;
	int bindid;
	/** \endcond
	 */
};

LaoModel *lao_get_model(const char *name);

void lao_draw_model_p(LaoModel *model);
void lao_draw_model(const char *name);

/* Resources/Shaders */

/** \cond */
typedef struct LaoUniform LaoUniform;
/** \endcond */
typedef struct LaoShaderProg LaoShaderProg;
struct LaoShaderProg {
	/** Shader name */
	char *name;
	/** OpenGL shader program handle */
	GLuint prog;
	/**\cond */
	int unicount;
	LaoUniform *uniforms;
	/** \endcond */
};

/** Find the location of a uniform in a shader.
 *
 * OpenGL provides glGetUniformLocation for querying the locations
 * of uniforms you want to pass values to. Doing so, however, blocks the
 * rendering, so these values are cached.
 *
 * You may use it like this
 *
 * 		LaoShaderProg *shader = lao_get_shader("test");
 * 		glUseProgram(shader->prog);
 * 		glUniform1f(lao_uniloc(shader, "t"), t);
 *
 * \param shader Shader to be queried.
 * \param name Name of the Uniform.
 * \return Uniform location as it would be returned by glGetUniformLocation.
 */
int lao_uniloc(LaoShaderProg *shader, const char *name);
LaoShaderProg *lao_get_shader(const char *name);
/** @}
 */


/* Buffer Objects/VBO */

/** \defgroup bufferobjs OpenGL buffer objects
 * @{
 */

typedef struct LaoVBO LaoVBO;
struct LaoVBO {
	GLuint vbo;
	int size;
	/**\cond */
	int ids;
	LaoList alloc;
	/**\endcond */
};

typedef struct LaoVertex LaoVertex;
struct LaoVertex {
	/** Position vector */
	Vec3 x;
	/** Normal vector */
	Vec3 n;
	/** texture coordinate s */
	float s;
	/** texture coordinate t */
	float t;
};

/* Buffer Objects/UBO */

typedef struct LaoBufferObjs LaoBufferObjs;
struct LaoBufferObjs {
	LaoVBO vbo;
};

typedef struct LaoUboPrototype LaoUboPrototype;
struct LaoUboPrototype {
	char *name;
	int size;
	void (*init)(GLuint *);
};

extern LaoBufferObjs lao_bos;

/** Helper to create a Uniform Buffer Object.
 *
 * \param[out] ubo Destination handle.
 * \param[in] name Name of the uniform block.
 * \param[in] size Size of the Buffer Object.
 * \param[in] data Data to be filled in. May be NULL to leave blank.
 */
void lao_create_ubo(GLuint *ubo, const char *name, int size, void *data);

/** @} */

#endif
