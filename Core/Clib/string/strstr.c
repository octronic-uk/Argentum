/* 
 * Copyright (C) 2014, Galois, Inc.
 * This sotware is distributed under a standard, three-clause BSD license.
 * Please see the file LICENSE, distributed with this software, for specific
 * terms and conditions.
 */
#include <string.h>
#include <stdlib.h>

char* string_within_string(const char *str1, const char *str2)
{
  size_t len_str2 = string_length(str2);
  char *cur;

  for (cur = (char*)str1; cur != NULL; cur = string_char(cur, *str2)) 
  {
    if (!string_n_compare(cur, str2, len_str2)) 
    {
      break;
    }
    cur++;
  }

  return cur;
}