#ifndef _LIBWIKI_C
#define _LIBWIKI_C

#include <sys/stat.h>

void
h(int level, const char *content)
{
	setenv("libwiki_h_content", content, 1);
	lsprintf("<h%d id=\"", level);
	system("echo \"$libwiki_h_content\" | sed 's/\\s/_/g;s/\"/_/g'");
	echo("\">");
	system("echo \"$libwiki_h_content\"");
	lsprintf("</h%d>", level);
}

void
href(char *const cstr)
{
	char *str, *url, *title, *path, *tmp;
	int external;
	struct stat sb;
	/* strlen + 3 ("in/") + 5 (".stac") + 1 (null terminator) */
	if (*(str = cstr) == '@') {
		++str;
		external = 1;
	} else external = 0;
	path = malloc(strlen(str) + 3 + 5 + 1);
	strcpy(url = strcpy(path, "in/") + 3, str);
	if ((tmp = strchr(url, '|')) != NULL) *(tmp++) = '\0'; else tmp = url;
	strcpy(title = malloc(strlen(tmp) + 1), tmp);
	strcpy(url + strlen(url), ".stac");
	if (!external) {
		if (*url >= 'a' && *url <= 'z')
			*url -= 0x20;
	}

	tmp = path;
	while ((tmp = strchr(tmp, ' ')) != NULL)
		*tmp = '_';
	lsprintf("<a href=\"%.*s\" class=\"%s\">%s</a>",
			strlen(url) - 5, url,
			external ? "external" : (((stat(path, &sb) == -1) ? "dead" : "")),
			title == NULL ? str : title);
	free(title);
	free(path);
}

void
table_display(void *header, void *body, size_t columns, size_t rows)
{
	size_t i, j;
	echo("<table class=\"table\">");
	echo("	<tr>");
	for (i = 0; i < columns; ++i) {
		echo("		<th>");
		lsprintf("			%s", *((char **)header + i));
		echo("		</th>");
	}
	echo("	</tr>");
	for (i = 0; i < rows; ++i) {
		echo("	<tr>");
		for (j = 0; j < columns; ++j) {
			echo("		<td>");
			lsprintf("			%s", *((char ***)body + ((i * columns) + j)));
			echo("		</td>");
		}
		echo("	</tr>");
	}
	echo("</table>");
}

#define table_header(...) \
	char *__table_header[] = { __VA_ARGS__ }; \

#define table(HEADER, ...) { \
	table_header HEADER; \
	char *__table_rest[][sizeof __table_header / sizeof *__table_header] = { \
		__VA_ARGS__ \
	}; \
	table_display ( __table_header, \
			__table_rest, \
			sizeof __table_header / sizeof *__table_header, \
			sizeof __table_rest / sizeof *__table_rest); \
}

#endif
