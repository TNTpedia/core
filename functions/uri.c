/*
The MIT License (MIT)

Copyright (c) 2014 Little Star Media, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef URI_C
#define URI_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Encodes a URI component by replacing each
 * instance of certain characters by one, two, three, or four escape
 * sequences representing the UTF-8 encoding of the character
 */

char *uri_encode (const char *);

/**
 * Decodes a URI component source from `uri_encode'
 */

char *uri_decode (const char *);

#define IN_URANGE(a,b,c) (a >= (unsigned int) b && a <= (unsigned int) c)

static inline int
needs_encoding (char ch, char next) {
  if (IN_URANGE(ch, 0xD800, 0xDBFF)) {
    if (!IN_URANGE(next, 0xDC00, 0xDFFF)) {
      return -1;
    }
  }

  // alpha capital/small
  if (IN_URANGE(ch, 0x0041, 0x005A) ||
      IN_URANGE(ch, 0x061, 0x007A)) {
    return 0;
  }

  // decimal digits
  if (IN_URANGE(ch, 0x0030, 0x0039)) {
    return 0;
  }

  // reserved chars
  // - _ . ! ~ * ' ( )
  if ('-' == ch || '_' == ch || '.' == ch ||
      '!' == ch || '~' == ch || '*' == ch ||
      '(' == ch || ')' == ch ) {
    return 0;
  }

  return 1;
}

char *
uri_encode (const char *src) {
  int i = 0;
  size_t size = 0;
  size_t len = 0;
  size_t msize = 0;
  char *enc = NULL;
  char tmp[4];
  char ch = 0;

  // chars length
  len = strlen(src);

#define push(c) (enc[size++] = c)

  for (; i < len; ++i) {
    switch (needs_encoding(src[i], src[i+1])) {
      case -1:
        // @TODO - handle with uri error
        free(enc);
        return NULL;

      case 0:
        msize++;
        break;

      case 1:
        msize = (msize + 3); // %XX
        break;
    }
  }

  // alloc with probable size
  enc = (char *) malloc((sizeof(char) * msize) + 1);
  if (NULL == enc) { return NULL; }

  // reset
  i = 0;

  // encode
  while (len--) {
    ch = src[i++];
    // if high surrogate ensure next is
    // low surrogate for valid surrogate
    // pair
    if (needs_encoding(ch, src[i])) {
      // encode
      sprintf(tmp, "%%%x", ch & 0xff);

      // prefix
      push(tmp[0]);

      // codec
      push(toupper(tmp[1]));
      push(toupper(tmp[2]));
    } else {
      push(ch);
    }
  }

  enc[size] = '\0';

#undef push

  return enc;
}

#undef IN_URANGE

char *
uri_decode (const char *src) {
  int i = 0;
  size_t size = 0;
  size_t len = 0;
  char *dec = NULL;
  char tmp[3];
  char ch = 0;

  // chars len
  len = strlen(src);

  // alloc
  dec = (char *) malloc(len + 1);

#define push(c) (dec[size++] = c)

  // decode
  while (len--) {
    ch = src[i++];

    // if prefix `%' then read byte and decode
    if ('%' == ch) {
      tmp[0] = src[i++];
      tmp[1] = src[i++];
      tmp[2] = '\0';
      push(strtol(tmp, NULL, 16));
    } else {
      push(ch);
    }
  }

  dec[size] = '\0';

#undef push

  return dec;
}

#endif
