#ifndef _INFOBOX_C
#define _INFOBOX_C

#include <functions/libwiki.c>

void
infobox_subtitle(char *url)
{
	lsprintf("\t<div class=\"infobox-subtitle\">%s</div>", url);
}

void
infobox_image(char *url, char *about)
{
	lsprintf("\t<div class=\"infobox-image\"><img src=\"/data/static/%s\" />", url);
	if (about != NULL) {
		lsprintf("%s", about);
	}
	lsprintf("\t</div>");
}

void
infobox_table_begin(char *title, char *tbclass, char *sttlclass)
{
	lsprintf("\t<table class=\"infobox-table %s\">", tbclass ? tbclass : "");
	if (title != NULL) {
		lsprintf("\t\t<div class=\"infobox-subtitle %s\">", sttlclass ? sttlclass : "");
		if (*title == '!')
			lsprintf("%s", title + 1);
		else
			href(title);
	}
	lsprintf("</div>");
}

#define infobox_table_row(LEFT, RIGHT) { \
	lsprintf("\t\t<tr class=\"infobox-table-row\">"); \
	lsprintf("\t\t\t<td class=\"infobox-table-left\">"); \
	{ LEFT; } \
	lsprintf("\t\t\t</td><td class=\"infobox-table-right\">"); \
	{ RIGHT; } \
	lsprintf("\t\t\t</td>"); \
	lsprintf("\t\t</tr>"); \
}

void
infobox_table_end(void)
{
	lsprintf("\t</table>");
}

#define infobox_table(TITLE, ...) { infobox_table_begin(TITLE, NULL, NULL); __VA_ARGS__; infobox_table_end(); }

/*****************************************************************************/

void
infobox_begin(void)
{
	lsprintf("<link rel=\"stylesheet\" href=\"/data/infobox.css?shasum=");
	system("sha1sum /var/www/tntpedia/html/data/infobox.css");
	lsprintf("\" />");
	lsprintf("<div class=\"infobox\">");
	lsprintf("\t<div class=\"infobox-title\">%.*s</div>", $(title).len, $(title).data);
}

void
infobox_end(void)
{
	lsprintf("</div>");
}

#define infobox(...) { infobox_begin(); __VA_ARGS__; infobox_end(); }

/* Infobox utilities *********************************************************/

#define infobox_twitch(OD, DO, NAZWA) { \
	infobox_table_begin ("Twitch", NULL, "infobox-twitch"); \
	infobox_table_row ( \
		(lsprintf("Okres sprawowania")), \
		(lsprintf("od %s do %s", (OD), (DO))) \
	); \
	infobox_table_row ( \
		(lsprintf("Nazwa użytkownika")), \
		(href("@https://twitch.tv/" NAZWA "|" NAZWA)) \
	); \
	infobox_table_end (); \
}

#define infobox_youtube(OD, DO, NAZWA) { \
	infobox_table_begin ( "YouTube", NULL, "infobox-youtube"); \
		infobox_table_row ( \
			(lsprintf("Okres sprawowania")), \
			(lsprintf("od %s do %s", (OD), (DO))) \
		); \
		infobox_table_row ( \
			(lsprintf("Nazwa użytkownika")), \
			(href("@https://youtube.com/" NAZWA)) \
		); \
	infobox_table_end(); \
}

#define infobox_kraj(NAZWA, STOLICA, ...) { \
	infobox_table ( NAZWA, \
		infobox_table_row ( \
			(lsprintf("Stolica")), \
			(href(STOLICA)) \
		); \
		__VA_ARGS__ \
	); \
}

#define infobox_portal_internetowy(ADRES, ...) { \
	infobox_table ( "Portal internetowy", \
		infobox_table_row ( \
			(lsprintf("Adres internetowy")), \
			(href(ADRES)) \
		); \
		__VA_ARGS__ \
	); \
}

#endif
