/**
QuickSort implementation as a single file header by Bhathiya Perera

MIT License
Copyright (c) 2022 Bhathiya Perera

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

// ======================================================================= //
//                             References                                  //
// ======================================================================= //

## Reference - quick sort for integer
https://stackoverflow.com/a/55011578

Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
Copyright (C) 2019 chqrlie
Based on public-domain C implementation by Darel Rex Finley.

## Reference - quick sort generic
https://github.com/ismdeep/sort-algos-c/blob/master/include/sort-algos/quick-sort.c
https://github.com/ismdeep/sort-algos-c/blob/master/include/sort-base.h

MIT License
Copyright (c) 2020 ismdeep


## Reference - memswap
https://gist.github.com/JadenGeller/3ab6fbc3f75690103ea6
Unknown license, asked in above link
*/
#ifndef YK__SORT
#define YK__SORT
#include <stddef.h>
/**
 * Comparison function, returns 0 if equal > 0 for larger and < 0 for smaller
 */
typedef int (*yk__compare_function)(const void *, const void *);
/**
 * Perform a quick sort for given array
 * @param arr array
 * @param item_size single item size
 * @param elements number of elements
 * @param cmp_func compare function
 * @return 0 if successful, -1 if partial
 */
int yk__quicksort(void *arr, size_t item_size, size_t elements,
                  yk__compare_function cmp_func);
/**
 * Perform a quick sort for given array (no malloc)
 * @param arr array
 * @param item_size single item size
 * @param elements number of elements
 * @param cmp_func compare function
 * @param single_elem_buffer buffer for a single temporary element (item_size can be held)
 * @return 0 if successful, -1 if partial
 */
int yk__quicksort_ex(void *arr, size_t item_size, size_t elements,
                     yk__compare_function cmp_func, void *single_elem_buffer);
#ifdef YK__SORT_IMPLEMENTATION
#include <stdlib.h>// malloc
#include <string.h>// memcpy
#define YK__SORT_MAX_LEVELS 48
/**
 * Swap memory between a and b of given item_size
 * @param a pointer to position a
 * @param b pointer to position b
 * @param item_size single element size
 */
void yk__memswap(void *a, void *b, size_t item_size) {
  char *a_swap = (char *) a;
  char *b_swap = (char *) b;
  char *a_end = a_swap + item_size;
  while (a_swap < a_end) {
    char temp = *a_swap;
    *a_swap = *b_swap;
    *b_swap = temp;
    a_swap++;
    b_swap++;
  }
}
/**
 * Swap two elements of an array
 * @param arr array
 * @param pos1 position 1
 * @param pos2 position 2
 * @param item_size size of a single element
 */
#define yk__generic_swap(arr, pos1, pos2, item_size)                           \
  do {                                                                         \
    yk__memswap(((char *) arr) + ((pos1) * (item_size)),                       \
                ((char *) arr) + ((pos2) * (item_size)), item_size);           \
  } while (0)
/**
 * Get an element of an array
 * @param arr array
 * @param pos position in array
 * @param item_size size of a single element
 * @return
 */
#define yk__generic_get(arr, pos, item_size)                                   \
  ((void *) (((char *) arr) + ((pos) * (item_size))))
int yk__quicksort_ex(void *arr, size_t item_size, size_t elements,
                     yk__compare_function cmp_func, void *single_elem_buffer) {
  size_t beg[YK__SORT_MAX_LEVELS], end[YK__SORT_MAX_LEVELS], L, R;
  int i = 0;
  beg[0] = 0;
  end[0] = elements;
  // NOTE: You can probably change this to a some MAX_SIZE char[]
  void *piv = single_elem_buffer;
  while (i >= 0) {
    L = beg[i];
    R = end[i];
    if (R - L > 1) {
      size_t M = L + ((R - L) >> 1);
      // T piv = arr[M];
      memcpy(piv, yk__generic_get(arr, M, item_size), item_size);
      // arr[M] = arr[L];
      memcpy(yk__generic_get(arr, M, item_size),
             yk__generic_get(arr, L, item_size), item_size);
      if (i == YK__SORT_MAX_LEVELS - 1) { return -1; }
      R--;
      while (L < R) {
        // while (arr[R] >= piv && L < R)
        while (cmp_func(yk__generic_get(arr, R, item_size), piv) >= 0 &&
               L < R) {
          R--;
        }
        if (L < R) { yk__generic_swap(arr, L++, R, item_size); }
        // while (arr[L] <= piv && L < R)
        while (cmp_func(yk__generic_get(arr, L, item_size), piv) <= 0 &&
               L < R) {
          L++;
        }
        if (L < R) { yk__generic_swap(arr, R--, L, item_size); }
      }
      // arr[L] = piv;
      memcpy(yk__generic_get(arr, L, item_size), piv, item_size);
      M = L + 1;
      // while (L > beg[i] && arr[L - 1] == piv)
      while (L > beg[i] &&
             cmp_func(yk__generic_get(arr, L - 1, item_size), piv) == 0) {
        L--;
      }
      // while (M < end[i] && arr[M] == piv)
      while (M < end[i] &&
             cmp_func(yk__generic_get(arr, M, item_size), piv) == 0) {
        M++;
      }
      if (L - beg[i] > end[i] - M) {
        beg[i + 1] = M;
        end[i + 1] = end[i];
        end[i++] = L;
      } else {
        beg[i + 1] = beg[i];
        end[i + 1] = L;
        beg[i++] = M;
      }
    } else {
      i--;
    }
  }
  return 0;
}
int yk__quicksort(void *arr, size_t item_size, size_t elements,
                  yk__compare_function cmp_func) {
  char *single_elem_buffer = malloc(item_size + 1);
  if (NULL == single_elem_buffer) return -1;
  single_elem_buffer[item_size] = 0;
  int return_value =
      yk__quicksort_ex(arr, item_size, elements, cmp_func, single_elem_buffer);
  free(single_elem_buffer);
  return return_value;
}
#endif
#endif