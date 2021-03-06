/* Measure memccpy functions.
   Copyright (C) 2013-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#define TEST_MAIN
#define TEST_NAME "memccpy"
#include "bench-string.h"

void *simple_memccpy (void *, const void *, int, size_t);
void *stupid_memccpy (void *, const void *, int, size_t);

IMPL (stupid_memccpy, 0)
IMPL (simple_memccpy, 0)
IMPL (memccpy, 1)

void *
simple_memccpy (void *dst, const void *src, int c, size_t n)
{
  const char *s = src;
  char *d = dst;

  while (n-- > 0)
    if ((*d++ = *s++) == (char) c)
      return d;

  return NULL;
}

void *
stupid_memccpy (void *dst, const void *src, int c, size_t n)
{
  void *p = memchr (src, c, n);

  if (p != NULL)
    return mempcpy (dst, src, p - src + 1);

  memcpy (dst, src, n);
  return NULL;
}

typedef void *(*proto_t) (void *, const void *, int c, size_t);

static void
do_one_test (impl_t *impl, void *dst, const void *src, int c, size_t len,
	     size_t n)
{
  void *expect = len > n ? NULL : (char *) dst + len;
  size_t i, iters = INNER_LOOP_ITERS;
  timing_t start, stop, cur;

  if (CALL (impl, dst, src, c, n) != expect)
    {
      error (0, 0, "Wrong result in function %s %p %p", impl->name,
	     CALL (impl, dst, src, c, n), expect);
      ret = 1;
      return;
    }

  if (memcmp (dst, src, len > n ? n : len) != 0)
    {
      error (0, 0, "Wrong result in function %s", impl->name);
      ret = 1;
      return;
    }

  TIMING_NOW (start);
  for (i = 0; i < iters; ++i)
    {
      CALL (impl, dst, src, c, n);
    }
  TIMING_NOW (stop);

  TIMING_DIFF (cur, start, stop);

  TIMING_PRINT_MEAN ((double) cur, (double) iters);
}

static void
do_test (size_t align1, size_t align2, int c, size_t len, size_t n,
	 int max_char)
{
  size_t i;
  char *s1, *s2;

  align1 &= 7;
  if (align1 + len >= page_size)
    return;

  align2 &= 7;
  if (align2 + len >= page_size)
    return;

  s1 = (char *) (buf1 + align1);
  s2 = (char *) (buf2 + align2);

  for (i = 0; i < len - 1; ++i)
    {
      s1[i] = 32 + 23 * i % (max_char - 32);
      if (s1[i] == (char) c)
	--s1[i];
    }
  s1[len - 1] = c;
  for (i = len; i + align1 < page_size && i < len + 64; ++i)
    s1[i] = 32 + 32 * i % (max_char - 32);

  printf ("Length %4zd, n %4zd, char %d, alignment %2zd/%2zd:", len, n, c, align1, align2);

  FOR_EACH_IMPL (impl, 0)
    do_one_test (impl, s2, s1, c, len, n);

  putchar ('\n');
}

int
test_main (void)
{
  size_t i;

  test_init ();

  printf ("%28s", "");
  FOR_EACH_IMPL (impl, 0)
    printf ("\t%s", impl->name);
  putchar ('\n');

  for (i = 1; i < 8; ++i)
    {
      do_test (i, i, 12, 16, 16, 127);
      do_test (i, i, 23, 16, 16, 255);
      do_test (i, 2 * i, 28, 16, 16, 127);
      do_test (2 * i, i, 31, 16, 16, 255);
      do_test (8 - i, 2 * i, 1, 1 << i, 2 << i, 127);
      do_test (2 * i, 8 - i, 17, 2 << i, 1 << i, 127);
      do_test (8 - i, 2 * i, 0, 1 << i, 2 << i, 255);
      do_test (2 * i, 8 - i, i, 2 << i, 1 << i, 255);
    }

  for (i = 1; i < 8; ++i)
    {
      do_test (0, 0, i, 4 << i, 8 << i, 127);
      do_test (0, 0, i, 16 << i, 8 << i, 127);
      do_test (8 - i, 2 * i, i, 4 << i, 8 << i, 127);
      do_test (8 - i, 2 * i, i, 16 << i, 8 << i, 127);
    }

  return ret;
}

#include "../test-skeleton.c"
