#include <lao.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lao_log_msg(const char *fmt, ...) {
	va_list ap;
	
	char *buf = malloc(5+strlen(fmt));
	strcpy(buf, "-- ");
	strcat(buf, fmt);
	strcat(buf, "\n");
	
	va_start(ap, fmt);
	vprintf(buf, ap);
	va_end(ap);
	free(buf);
}

void lao_log_warn(const char *fmt, ...) {
	va_list ap;

	char *buf = malloc(11+strlen(fmt));
	strcpy(buf, "warning: ");
	strcat(buf, fmt);
	strcat(buf, "\n");
		
	va_start(ap, fmt);
	vfprintf(stderr, buf, ap);
	va_end(ap);
	free(buf);
}

void lao_log_fatal(const char *fmt, ...) {
	va_list ap;

	char *buf = malloc(9+strlen(fmt));
	strcpy(buf, "error: ");
	strcat(buf, fmt);
	strcat(buf, "\n");
		
	va_start(ap, fmt);
	vfprintf(stderr, buf, ap);
	va_end(ap);
	free(buf);
	
	exit(1);
}
