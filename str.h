#ifndef _STR_H
#define _STR_H

typedef struct {
	char *data;
	size_t len;
} String;

static int Strcmp(String a, String b);
static ssize_t Strtok(String string, String *out, char c);

static int
Strcmp(String a, String b)
{
	if (a.len != b.len) return -1;
	return strncmp(a.data, b.data, a.len);
}

static ssize_t
Strtok(String string, String *out, char c)
{
	char *tmpptr = string.data;
	while ((tmpptr - string.data) < string.len) {
		if (*(++tmpptr) == c) break;
	}
	string.len = tmpptr - string.data;
	*out = string;
	return (tmpptr - string.data);
}

static String
Strtrim(String str)
{
	int i = 0;
	while (isspace(*(str.data)) && i < str.len) {
		++str.data;
		--str.len;
		++i;
	}
	while (isspace(*(str.data + (str.len - 1))))
		--str.len;
	return str;
}

#endif
