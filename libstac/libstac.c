#include "libstac.h"

ssize_t
print(char *what)
{
	return write(fd, what, strlen(what));
}

ssize_t
nprint(char *what, size_t n)
{
	return write(fd, what, n);
}

int
lsprintf(char *fmt, ...)
{
	int x;
	va_list ap;

	va_start(ap, fmt);
	x = vdprintf(fd, fmt, ap);
	va_end(ap);
	return x;
}
