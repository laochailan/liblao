#include "texture.h"

#include <lao.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <png.h>
#include <stdlib.h>
#include "resource.h"

void _lao_load_texture(LaoTexture *tex, const char *filename) {
	int w, h, channels, color_type, i;
	uint8_t *pixels;
	FILE *fp = fopen(filename, "rb");
	
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers;
	
	if(!fp)
		lao_log_fatal("load_texture(): error opening '%s'", filename);	
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);				
	info_ptr = png_create_info_struct(png_ptr);
	
	png_init_io(png_ptr, fp);
	
	png_read_png(png_ptr, info_ptr, 0, NULL);
	
	w = png_get_image_width(png_ptr, info_ptr);
	h = png_get_image_height(png_ptr, info_ptr);
	channels = png_get_channels(png_ptr, info_ptr);
	
	row_pointers = png_get_rows(png_ptr, info_ptr);
	
	pixels = malloc(h*w*channels);
	
	for(i = 0; i < h; i++)
		memcpy(&pixels[i*w*channels], row_pointers[h-i-1], channels*w);
	
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	
	glGenTextures(1, &tex->gltex);
	glBindTexture(GL_TEXTURE_2D, tex->gltex);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	switch(channels) {
	case 1:
		color_type = GL_RED;
		break;
	case 2:
		color_type = GL_RG;
		break;
	case 3:
		color_type = GL_RGB;
		break;
	case 4:
		color_type = GL_RGBA;
		break;
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, channels, w, h, 0, color_type, GL_UNSIGNED_BYTE, pixels);
	
	tex->w = w;
	tex->h = h;
	tex->name = _lao_get_res_name(LAO_GFX_PATH, filename);
	lao_log_msg("%s -> %s", filename, tex->name);
	free(pixels);
}

void _lao_free_texture(LaoTexture *tex) {
	free(tex->name);
	glDeleteTextures(1, &tex->gltex);
}

LaoTexture *lao_get_tex(const char *name) {
	int i;
	for(i = 0; i < _lao_resources.ntex; i++)
		if(strcmp(_lao_resources.textures[i].name, name) == 0)
			return &_lao_resources.textures[i];
	
	lao_log_fatal("get_tex(): texture '%s' not found.", name);
	return NULL;
}
