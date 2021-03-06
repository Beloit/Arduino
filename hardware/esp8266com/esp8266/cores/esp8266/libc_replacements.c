/* 
 libc_replacements.c - replaces libc functions with functions
 from Espressif SDK

 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 Modified 03 April 2015 by Markus Sattler

 */

#include <stddef.h>
#include <stdarg.h>

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"


void* malloc(size_t size) {
	return os_malloc(size);
}

void free(void* ptr) {
	os_free(ptr);
}

void* realloc(void* ptr, size_t size) {
	return os_realloc(ptr, size);
}

int printf(const char* format, ...) {
	int ret;
	va_list arglist;
	va_start(arglist, format);
	ret = ets_vprintf(format, arglist);
	va_end(arglist);
	return ret;
}

int sprintf(char* buffer, const char* format, ...) {
	int ret;
	va_list arglist;
	va_start(arglist, format);
	ret = ets_vsprintf(buffer, format, arglist);
	va_end(arglist);
	return ret;
}

int snprintf(char* buffer, size_t size, const char* format, ...) {
	int ret;
	va_list arglist;
	va_start(arglist, format);
	ret = ets_vsnprintf(buffer, size, format, arglist);
	va_end(arglist);
	return ret;
}

int vsnprintf(char * buffer, size_t size, const char * format, va_list arg) {
	return ets_vsnprintf(buffer, size, format, arg);
}

int memcmp(const void *s1, const void *s2, size_t n) {
	return ets_memcmp(s1, s2, n);
}

void* memcpy(void *dest, const void *src, size_t n) {
	return ets_memcpy(dest, src, n);
}

void* memset(void *s, int c, size_t n) {
	return ets_memset(s, c, n);
}

int strcmp(const char *s1, const char *s2) {
	return ets_strcmp(s1, s2);
}

char* strcpy(char *dest, const char *src) {
	return ets_strcpy(dest, src);
}

size_t strlen(const char *s) {
	return ets_strlen(s);
}

int strncmp(const char *s1, const char *s2, size_t len) {
	return ets_strncmp(s1, s2, len);
}

char *strncpy(char * dest, const char * src, size_t n) {
	return ets_strncpy(dest, src, n);
}

char *ets_strstr(const char *haystack, const char *needle) {
	return strstr(haystack, needle);
}

char *strchr(const char * str, int character) {
	while(1) {
		if(*str == 0x00) {
			return NULL;
		}
		if(*str == (char) character) {
			return (char *) str;
		}
		str++;
	}
}

char *strrchr(const char * str, int character) {
	char * ret = NULL;
	while(1) {
		if(*str == 0x00) {
			return ret;
		}
		if(*str == (char) character) {
			ret = (char *) str;
		}
		str++;
	}
}

char *strcat(char * dest, const char * src) {
	return strncat(dest, src, strlen(src));
}

char *strncat(char * dest, const char * src, size_t n) {
	uint32_t offset = strlen(dest);
	for(uint32_t i = 0; i < n; i++) {
		*(dest + i + offset) = *(src + i);
		if(*(src + i) == 0x00) {
			break;
		}
	}
	return dest;
}

char *strtok_r(char * str, const char * delimiters, char ** temp) {
	static char * ret = NULL;
	char * start = NULL;
	char * end = NULL;
	uint32_t size = 0;

	if(str == NULL) {
		start = *temp;
	} else {
		start = str;
	}

	if(start == NULL) {
		return NULL;
	}

	end = start;

	while(1) {
		for(uint16_t i = 0; i < strlen(delimiters); i++) {
			if(*end == *(delimiters + i)) {
				break;
			}
		}
		end++;
		if(*end == 0x00) {
			break;
		}
	}

	*temp = end;

	if(ret != NULL) {
		free(ret);
	}

	size = (end - start);
	ret = (char *) malloc(size);
	strncpy(ret, start, size);
	return ret;
}

// ##########################################################################
//                             ctype functions
// ##########################################################################

int isalnum(int c) {
	if(isalpha(c) || isdigit(c)) {
		return 1;
	}
	return 0;
}

int isalpha(int c) {
	if(islower(c) || isupper(c)) {
		return 1;
	}
	return 0;
}

int iscntrl(int c) {
	if(c <= 0x1F || c == 0x7F) {
		return 1;
	}
	return 0;
}

int isdigit(int c) {
	if(c >= '0' && c <= '9') {
		return 1;
	}
	return 0;
}
int isgraph(int c) {
	if(isprint(c) && c != ' ') {
		return 1;
	}
	return 0;
}

int islower(int c) {
	if(c >= 'a' && c <= 'z') {
		return 1;
	}
	return 0;
}

int isprint(int c) {
	if(!iscntrl(c)) {
		return 1;
	}
	return 0;
}

int ispunct(int c) {
	if(isgraph(c) && !isalnum(c)) {
		return 1;
	}
	return 0;
}

int isspace(int c) {
	switch(c) {
		case 0x20: // ' '
		case 0x09: // '\t'
		case 0x0a: // '\n'
		case 0x0b: // '\v'
		case 0x0c: // '\f'
		case 0x0d: // '\r'
			return 1;
	}
	return 0;
}

int isupper(int c) {
	if(c >= 'A' && c <= 'Z') {
		return 1;
	}
	return 0;
}

int isxdigit(int c) {
	if(c >= 'A' && c <= 'F') {
		return 1;
	}
	if(c >= 'a' && c <= 'f') {
		return 1;
	}
	if(isdigit(c)) {
		return 1;
	}
	return 0;
}

int tolower(int c) {
	if(isupper(c)) {
		c += 0x20;
	}
	return c;
}

int toupper(int c) {
	if(islower(c)) {
		c -= 0x20;
	}
	return c;
}

int isblank(int c) {
	switch(c) {
		case 0x20: // ' '
		case 0x09: // '\t'
			return 1;
	}
	return 0;
}

// ##########################################################################

