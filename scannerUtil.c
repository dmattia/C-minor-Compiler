//Implementation of ScannerUtil.h
#include "scannerUtil.h"
#include <string.h>
#include <stdlib.h>

// Prepares a string for printing
// If any escape characters are found as
// two chars, they are joined together
const char* fix_escape_chars(const char* str) {
	int i, size = strlen(str), j = 0;
	char* fixed = malloc(size);
	for(i=0; i <= size; ++i) {
		if(str[i] == '\\') {
	        	switch(str[i+1]) {
	        	        case 'a':
	                                fixed[j++] = '\a';
	                                break;
	                        case 'b':
	                                fixed[j++] = '\b';
	                                break;
	                        case 'f':
	                                fixed[j++] = '\f';
	                                break;
	                        case 'r':
	                                fixed[j++] = '\r';
	                                break;
	                        case 'v':
	                                fixed[j++] = '\v';
	                                break;
	                        case 'n':
	                                fixed[j++] = '\n';
	                                break;
	                        case 't':
	                                fixed[j++] = '\t';
	                                break;
	                        case '\\':
	                                fixed[j++] = '\\';
	                                break;
	                        case '\'':
	                                fixed[j++] = '\'';
	                                break;
	                        case '\"':
	                                fixed[j++] = '\"';
	                                break;
	                        case '\?':
	                                fixed[j++] = '\?';
	                                break;
	                        default:
	                                --i;
	                                ++size;
	                                fixed[j++] = str[i];
	                }
			++i;
			size--;
		} else {
			fixed[j++] = str[i];
		}
	}
	return fixed;
}

const char* remove_quotes(char* quotes) {
        int i, size = strlen(quotes)-2;
        char* without = (char*)malloc(size);
        for(i = 1; i <= strlen(quotes)-2; ++i) {
                without[i-1] = quotes[i];
        }
        without[size] = '\0';
        return without;
}
