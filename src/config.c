/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2014, Lukas Weber <laochailan@web.de>
 */

#include <lao.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "util/util.h"

enum {
	CONFIG_LOAD_BUFSIZE = 512
};

void lao_cfg_preset(LaoConfigValue *config, LaoConfigDef *configdefs) {
	LaoConfigDef *e;
	for(e = configdefs; e->name; e++) {
		config[e->key] = e->preset;
	}
}	

static LaoConfigDef* lao_cfg_findentry(char *name, LaoConfigDef *configdefs) {
	LaoConfigDef *e = configdefs;
	do if(!strcmp(e->name, name)) return e; while((++e)->name);
	return NULL;
}

static void lao_cfg_set(char *key, char *val, LaoConfigValue *config, LaoConfigDef *configdefs) {
	LaoConfigDef *e = lao_cfg_findentry(key, configdefs);
	
	if(!e) {
		lao_log_warn("lao_cfg_set(): unknown key '%s'", key);
		return;
	}
	
	switch(e->type) {
		case CFGT_NUM:
			config[e->key].num = strtol(val, 0, 10);
			break;		
		case CFGT_STR:
			free(config[e->key].str);
			stralloc(&config[e->key].str, val);
			break;		
		case CFGT_FLT:
			config[e->key].flt = strtod(val, 0);
			break;
	}
}

void lao_cfg_save(char *filename, LaoConfigValue *config, LaoConfigDef *configdefs) {
	FILE *out = fopen(filename, "w");
	LaoConfigDef *e = configdefs;
	
	if(!out) {
		lao_log_warn("lao_cfg_save(): couldn't open '%s'", filename);
		return;
	}
	
	fputs("# This file was generated automatically \n", out);
	
	do switch(e->type) {
		case CFGT_NUM:
			fprintf(out, "%s = %i\n", e->name, config[e->key].num);
			break;		
		case CFGT_STR:
			fprintf(out, "%s = %s\n", e->name, config[e->key].str);
			break;		
		case CFGT_FLT:
			fprintf(out, "%s = %f\n", e->name, config[e->key].flt);
	} while((++e)->name);
	
	fclose(out);
	lao_log_msg("saved config '%s'", filename);
}

#define SYNTAXERROR { lao_log_warn("lao_cfg_load(): syntax error on line %i, aborted! [%s:%i]\n", line, __FILE__, __LINE__); goto end; }
#define BUFFERERROR { lao_log_warn("lao_cfg_load(): string exceed the limit of %i, aborted! [%s:%i]", CONFIG_LOAD_BUFSIZE, __FILE__, __LINE__); goto end; }

void lao_cfg_load(char *filename, LaoConfigValue *config, LaoConfigDef *configdefs) {
	FILE *in = fopen(filename, "r");
		
	int c, i = 0, found, line = 0;
	char buf[CONFIG_LOAD_BUFSIZE];
	char key[CONFIG_LOAD_BUFSIZE];
	char val[CONFIG_LOAD_BUFSIZE];
		
	lao_cfg_preset(config, configdefs);
	if(!in) {
		lao_log_warn("lao_cfg_load(): couldn't open '%s'", filename);
		return;
	}
		
	while((c = fgetc(in)) != EOF) {
		if(c == '#' && !i) {
			i = 0;
			while(fgetc(in) != '\n');
		} else if(c == ' ') {
			if(!i) SYNTAXERROR
			
			buf[i] = 0;
			i = 0;
			strcpy(key, buf);
			
			found = 0;
			while((c = fgetc(in)) != EOF) {
				if(c == '=') {
					if(++found > 1) SYNTAXERROR
				} else if(c != ' ') {
					if(!found || c == '\n') SYNTAXERROR
					
					do {
						if(c == '\n') {
							if(!i) SYNTAXERROR
							
							buf[i] = 0;
							i = 0;
							strcpy(val, buf);
							found = 0;
							++line;
							
							lao_cfg_set(key, val, config, configdefs);
							break;
						} else {
							buf[i++] = c;
							if(i == CONFIG_LOAD_BUFSIZE)
								BUFFERERROR
						}
					} while((c = fgetc(in)) != EOF);
					
					break;
				}
			} if(found) SYNTAXERROR
		} else {
			buf[i++] = c;
			if(i == CONFIG_LOAD_BUFSIZE)
				BUFFERERROR
		}
	}

end:
	fclose(in);
	lao_log_msg("loaded config '%s'", filename);
}

#undef BUFFERERROR
#undef SYNTAXERROR

void lao_cfg_free(LaoConfigValue *confvals, LaoConfigDef *confdefs) {
	LaoConfigDef *e;
	for(e = confdefs; e->name; e++)
		if(e->type == CFGT_STR)
			free(confvals[e->key].str);
}
