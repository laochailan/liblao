#include "shader.h"

#include <lao.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "resource.h"
#include "util/util.h"

static void print_info_log(GLuint shader) {
	int len = 0, alen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	
	if(len > 1) {
		char *log = malloc(len);
		memset(log, 0, len);
		glGetShaderInfoLog(shader, len, &alen, log);
		printf("%s\n", log);
		free(log);
	}
}

static void print_program_info_log(GLuint program) {
	int len = 0, alen = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
	
	if(len > 1) {
		char *log = malloc(len);
		memset(log, 0, len);
		glGetProgramInfoLog(program, len, &alen, log);
		printf("%s\n", log);
		free(log);
	}
}

static char *read_all(char *filename, int *size) {
	char *text;
	FILE *file = fopen(filename, "r");
	
	if(file == NULL)
		lao_log_fatal("read_all(): failed opening '%s'", filename);

	fseek(file, 0, SEEK_END);
	*size = ftell(file)+1;
	fseek(file, 0, SEEK_SET);
	
	if(*size == 0)
		lao_log_warn("read_all(): file '%s' empty!", filename);
	
	text = malloc(*size+1);
	memset(text, 0, *size+1);
	fread(text, *size, 1, file);
		
	fclose(file);
	
	return text;
}

static void parse_link_include(_LaoShaderContext *sc, _LaoShaderObj *s, char **src, int *size) {
	char *c = *src;
	
	char link[] = "%link";
	char include[] = "%include";
	char mainf[] = "main(";
	
	int lc = 0, ic = 0, mc = 0;
	int i = 0;
	
	char *new = malloc(*size);
	do {
		if(*c == link[lc])
			lc++;
		else
			lc = 0;
		
		if(*c == include[ic])
			ic++;
		else
			ic = 0;
		
		if(*c == mainf[mc])
			mc++;
		else
			mc = 0;
		
		if(mc == sizeof(mainf)-1)
			s->main = 1;
		
		if(lc == sizeof(link)-1 || ic == sizeof(include)-1) {
			char *beg, *end;
			int offset = 0;
			
			if(c[1] != ' ')
				lao_log_fatal("%s: missing space after %%%s statement", s->name, lc == sizeof(link)-1 ? "link" : "include");
			
			beg = c+2;
			end = beg;
			while(*end != '\n' && *end != '\0')
				end++;
			
			if(lc == sizeof(link)-1) {
				s->links = realloc(s->links, sizeof(char *)*(++s->nlinks));
				s->links[s->nlinks-1] = malloc(end-beg+1);
				strncpy(s->links[s->nlinks-1], beg, end-beg);
				s->links[s->nlinks-1][end-beg] = 0;
				
				offset = lc;
			} else {
				int j;
				offset = ic;
				
				for(j = 0; j < sc->nheaders; j++) {
					if(strncmp(beg, sc->hname[j], end-beg) == 0) {
						int hlen = strlen(sc->hsrc[j]);
						*size += hlen;
						new = realloc(new, *size);
						strcpy(new+i-offset, sc->hsrc[j]);
						i += hlen;
					}
				}
				
				if(i == sc->nheaders) {
					char *buf = malloc(end-beg+1);
					strncpy(buf, beg, end-beg);
					buf[end-beg] = 0;
					lao_log_fatal("%s: failed to include '%s': header not found", s->name, buf);
				}				
			}
			
			i -= offset;
			*size -= end - c + offset;
			c = end;			
		}
		
		new[i++] = *c;
	} while(*c && c++);
	
	free(*src);
	*src = new;	
}

static void parse_parts(char *src, int partnum, int num) {
	char *part = src;
	
	while((part = strstr(part, "%part"))) {
		int match = 0;
		char *np = part, *next, *endpart;
		
		if(part != src && part[-1] != '\n' && part[-1] != '\r') {
			part += 5;
			continue;
		}
		
		np += 5;
		if(*np != ' ')
			lao_log_fatal("missing space after %%part statement");
		np++;
		next = np;
		do {
			int n;
			
			np = next;
			n = strtol(np, &next, 10);
			
			if(!isspace(*next))
				lao_log_fatal("expected numbers after %%part");
			
			if(n > num)
				lao_log_fatal("invalid %%part number '%d'", n);
			
			if(n == partnum+1)
				match = 1;			
		} while(*next != '\n' && np != next);
		
		endpart = strstr(part, "%endpart");
		if(!endpart)
			lao_log_fatal("expected %%endpart after %%part");
		
		if(match) {
			part[0] = '/';
			part[1] = '/';
						
			endpart[0] = '/';
			endpart[1] = '/';
		} else {
			memset(part, ' ', endpart-part+8);
		}
		
		part = endpart + 8;
	}
}

static char **parse_splits(char *path, char *src, int *num) {
	char *split, *last;
	char **snames = NULL;
	*num = 0;
	if((split = strstr(src, "%split")) && (split == src || split[-1] == '\n' || split[-1] == '\r')) {
				
		if(split[6] != ' ')
			lao_log_fatal("%s: missing space after %%split statement.", path);
		if(strstr(split+6, "%split"))
			lao_log_fatal("%s: only one %%split per file allowed.", path);
		memset(split, '/', 2);
		split += 6;
		last = split;
		
		while(*split != '\n') {
			split++;
			if(*split == ' ' || *split == '\n' || *split == '\0') {
				if(split != last+1) {				
					snames = realloc(snames, sizeof(char *)*(++*num));
					snames[*num-1] = malloc(split-last+1);
					strncpy(snames[*num-1], last+1, split-last-1);
					snames[*num-1][split-last-1] = 0;
				}
				last = split;
			}
		}
	} else {
		snames = malloc(sizeof(char *));
		(*num)++;		
		snames[0] = _lao_get_res_name(LAO_SHADER_PATH, path);
	}
	
	return snames;
}

void _lao_compile_shader(_LaoShaderContext *sc, char *path) {
	char **snames;
	int num = 0;
	
	char *src;
	char typec;
	int size;
	int i;
		
	src = read_all(path, &size);
	
	snames = parse_splits(path, src, &num);
	sc->nshaders += num;
	sc->shaders = realloc(sc->shaders, sizeof(_LaoShaderObj)*sc->nshaders);
	
	for(i = 0; i < num; i++) {
		char *srccpy;
		
		_LaoShaderObj *s = &sc->shaders[sc->nshaders-num+i];
		memset(s, 0, sizeof(_LaoShaderObj));
		s->name = snames[i];
				
		typec = s->name[strlen(s->name)-1];
		
		switch(typec) {
		case 'v':
			s->type = GL_VERTEX_SHADER;
			break;
		case 'f':
			s->type = GL_FRAGMENT_SHADER;
			break;
		case 'g':
			s->type = GL_GEOMETRY_SHADER;
			break;
		case 'h':
			continue;
		default:
			lao_log_warn("%s: expected *.[vfgh] in part name. part ignored.", s->name);
			continue;
		}
		
		lao_log_msg("%s -> %s", path, s->name);
		
		stralloc(&srccpy, src);
		
		parse_link_include(sc, s, &srccpy, &size);
		parse_parts(srccpy, i, num);
				
		s->obj = glCreateShader(s->type);
		glShaderSource(s->obj, 1, (const GLchar * const*) &srccpy, &size);
		glCompileShader(s->obj);
		print_info_log(s->obj);
		free(srccpy);
	}
	
	
	free(src);
	free(snames);
}

static void load_header(_LaoShaderContext *sc, char *path) {
	char **snames;
	int num = 0;
		
	char *src;
	int size;
	int i;
		
	src = read_all(path, &size);
	
	snames = parse_splits(path, src, &num);
		
	for(i = 0; i < num; i++) {
		int gi;
		if(snames[i][strlen(snames[i])-1] != 'h') {
			free(snames[i]);
			continue;
		}
		
		sc->nheaders++;
		sc->hname = realloc(sc->hname, sizeof(char *)*sc->nheaders);
		sc->hsrc = realloc(sc->hsrc, sizeof(char *)*sc->nheaders);
		gi =  sc->nheaders - 1;
		
		sc->hname[gi] = snames[i];
		lao_log_msg("-> %s", sc->hname[gi]);
		
		stralloc(&sc->hsrc[gi], src);
				
		parse_parts(sc->hsrc[gi], i, num);
	}
	
	free(src);
	free(snames);
}

void _lao_load_shaders(char *path, LaoShaderProg **dest, int *nsha) {
	_LaoShaderContext sc;
	memset(&sc, 0, sizeof(_LaoShaderContext));
	
	lao_log_msg("loading shader headers...");
	_lao_recurse_dir(path, ".glsl", (void (*)(void *, const char*))load_header, &sc);
	lao_log_msg("compiling shaders...");
	
	_lao_recurse_dir(path, ".glsl", (void (*)(void *, const char*))_lao_compile_shader, &sc);
	
	lao_log_msg("linking shaders...");
	_lao_link_shaders(dest, nsha, &sc);
	_lao_free_shadercontext(&sc);
}


static _LaoShaderObj *get_shobj(_LaoShaderContext *sc, char *name) {
	int i;
	for(i = 0; i < sc->nshaders; i++)
		if(strcmp(sc->shaders[i].name, name) == 0)
			return &sc->shaders[i];
	
	lao_log_fatal("failed to link '%s': no such shader.", name);
	return NULL;
}

static void add_shobj(_LaoShaderContext *sc, _LaoShaderObj ***list, int *size, _LaoShaderObj *s) {
	int i;
	for(i = 0; i < *size; i++)
		if((*list)[i] == s)
			lao_log_fatal("%s: circular linking dependency on '%s'.", (*list)[0]->name, s->name);
	
	*list = realloc(*list, sizeof(_LaoShaderObj *)*(++*size));
	(*list)[*size-1] = s;
	
	for(i = 0; i < s->nlinks; i++) {
		add_shobj(sc, list, size, get_shobj(sc, s->links[i]));
	}
}

void _lao_link_shaders(LaoShaderProg **dest, int *nsha, _LaoShaderContext *sc) {
	int i, j;
	for(i = 0; i < sc->nshaders; i++) {
		_LaoShaderObj *s = &sc->shaders[i];
		
		if(s->type == GL_VERTEX_SHADER && s->main) {
			_LaoShaderObj **list = 0;
			int size = 0;
			LaoShaderProg *p;
			
			*dest = realloc(*dest, sizeof(LaoShaderProg)*(++*nsha));
			p = *dest+*nsha-1;
			memset(p, 0, sizeof(LaoShaderProg));
			stralloc(&p->name, s->name);
			p->name[strlen(p->name)-2] = 0;
			
			lao_log_msg("-> %s", p->name);
			add_shobj(sc, &list, &size, s);
						
			p->prog = glCreateProgram();
			for(j = 0; j < size; j++) {
				if(!list[j]->obj) {
					lao_log_warn("object '%s' is not linkable", list[j]->name);
					continue;
				}
				glAttachShader(p->prog, list[j]->obj);				
			}
			
			glLinkProgram(p->prog);
			print_program_info_log(p->prog);
			
			free(list);
			
			_lao_cache_uniforms(p);
		}
	}	
}

void _lao_free_shadercontext(_LaoShaderContext *sc) {
	int i;
	
	for(i = 0; i < sc->nshaders; i++)
		_lao_free_shaderobj(&sc->shaders[i]);
	for(i = 0; i < sc->nheaders; i++) {
		free(sc->hsrc[i]);
		free(sc->hname[i]);
	}
	
	free(sc->shaders);
	free(sc->hsrc);
	free(sc->hname);
}

void _lao_free_shader(LaoShaderProg *sha) {
	int i;
	free(sha->name);
	glDeleteProgram(sha->prog);	
	
	for(i = 0; i < sha->unicount; i++)
		free(sha->uniforms[i].name);
	free(sha->uniforms);
}

void _lao_free_shaderobj(_LaoShaderObj *sha) {
	int i;
	for(i = 0; i < sha->nlinks; i++)
		free(sha->links[i]);
	free(sha->links);
	free(sha->name);
	
	glDeleteShader(sha->obj);
}

void _lao_cache_uniforms(LaoShaderProg *sha) {
	int i, maxlen = 0;
	GLint tmpi;
	GLenum tmpt;
	
	glGetProgramiv(sha->prog, GL_ACTIVE_UNIFORMS, &sha->unicount);
	glGetProgramiv(sha->prog, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);
	
	sha->uniforms = calloc(sizeof(LaoUniform), sha->unicount);
	
	for(i = 0; i < sha->unicount; i++) {
		sha->uniforms[i].name = malloc(maxlen);
		memset(sha->uniforms[i].name, 0, maxlen);
		
		glGetActiveUniform(sha->prog, i, maxlen, NULL, &tmpi, &tmpt, sha->uniforms[i].name);
		sha->uniforms[i].location = glGetUniformLocation(sha->prog, sha->uniforms[i].name);
	}
}

int lao_uniloc(LaoShaderProg *sha, const char *name) {
	int i;
	for(i = 0; i < sha->unicount; i++)
		if(strcmp(sha->uniforms[i].name, name) == 0)
			return sha->uniforms[i].location;
		
	return -1;
}

LaoShaderProg *lao_get_shader(const char *name) {
	int i;
		
	for(i = 0; i < _lao_resources.nsha; i++) {
		if(strcmp(_lao_resources.shaders[i].name, name) == 0)
			return &_lao_resources.shaders[i];
	}
	
	lao_log_fatal("get_shader(): shader '%s' not found.", name);
	return NULL;
}
