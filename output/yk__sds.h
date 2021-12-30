
/*
Copyright (c) 2006-2014, Salvatore Sanfilippo <antirez at gmail dot com>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef YK__SDS_SINGLE_HEADER
#define YK__SDS_SINGLE_HEADER
#ifndef __SDS_H
#define __SDS_H
#define YK__SDS_MAX_PREALLOC (1024 * 1024)
extern const char *YK__SDS_NOINIT;
#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>
typedef char *yk__sds;
struct __attribute__((__packed__)) yk__sdshdr5 {
  unsigned char flags;
  char buf[];
};
struct __attribute__((__packed__)) yk__sdshdr8 {
  uint8_t len;
  uint8_t alloc;
  unsigned char flags;
  char buf[];
};
struct __attribute__((__packed__)) yk__sdshdr16 {
  uint16_t len;
  uint16_t alloc;
  unsigned char flags;
  char buf[];
};
struct __attribute__((__packed__)) yk__sdshdr32 {
  uint32_t len;
  uint32_t alloc;
  unsigned char flags;
  char buf[];
};
struct __attribute__((__packed__)) yk__sdshdr64 {
  uint64_t len;
  uint64_t alloc;
  unsigned char flags;
  char buf[];
};
#define YK__SDS_TYPE_5 0
#define YK__SDS_TYPE_8 1
#define YK__SDS_TYPE_16 2
#define YK__SDS_TYPE_32 3
#define YK__SDS_TYPE_64 4
#define YK__SDS_TYPE_MASK 7
#define YK__SDS_TYPE_BITS 3
#define YK__SDS_HDR_VAR(T, s)                                                  \
  struct yk__sdshdr##T *sh = (void *) ((s) - (sizeof(struct yk__sdshdr##T)));
#define YK__SDS_HDR(T, s)                                                      \
  ((struct yk__sdshdr##T *) ((s) - (sizeof(struct yk__sdshdr##T))))
#define YK__SDS_TYPE_5_LEN(f) ((f) >> YK__SDS_TYPE_BITS)
static inline size_t yk__sdslen(const yk__sds s) {
  unsigned char flags = s[-1];
  switch (flags & YK__SDS_TYPE_MASK) {
    case YK__SDS_TYPE_5:
      return YK__SDS_TYPE_5_LEN(flags);
    case YK__SDS_TYPE_8:
      return YK__SDS_HDR(8, s)->len;
    case YK__SDS_TYPE_16:
      return YK__SDS_HDR(16, s)->len;
    case YK__SDS_TYPE_32:
      return YK__SDS_HDR(32, s)->len;
    case YK__SDS_TYPE_64:
      return YK__SDS_HDR(64, s)->len;
  }
  return 0;
}
static inline size_t yk__sdsavail(const yk__sds s) {
  unsigned char flags = s[-1];
  switch (flags & YK__SDS_TYPE_MASK) {
    case YK__SDS_TYPE_5: {
      return 0;
    }
    case YK__SDS_TYPE_8: {
      YK__SDS_HDR_VAR(8, s);
      return sh->alloc - sh->len;
    }
    case YK__SDS_TYPE_16: {
      YK__SDS_HDR_VAR(16, s);
      return sh->alloc - sh->len;
    }
    case YK__SDS_TYPE_32: {
      YK__SDS_HDR_VAR(32, s);
      return sh->alloc - sh->len;
    }
    case YK__SDS_TYPE_64: {
      YK__SDS_HDR_VAR(64, s);
      return sh->alloc - sh->len;
    }
  }
  return 0;
}
static inline void yk__sdssetlen(yk__sds s, size_t newlen) {
  unsigned char flags = s[-1];
  switch (flags & YK__SDS_TYPE_MASK) {
    case YK__SDS_TYPE_5: {
      unsigned char *fp = ((unsigned char *) s) - 1;
      *fp = YK__SDS_TYPE_5 | (newlen << YK__SDS_TYPE_BITS);
    } break;
    case YK__SDS_TYPE_8:
      YK__SDS_HDR(8, s)->len = newlen;
      break;
    case YK__SDS_TYPE_16:
      YK__SDS_HDR(16, s)->len = newlen;
      break;
    case YK__SDS_TYPE_32:
      YK__SDS_HDR(32, s)->len = newlen;
      break;
    case YK__SDS_TYPE_64:
      YK__SDS_HDR(64, s)->len = newlen;
      break;
  }
}
static inline void yk__sdsinclen(yk__sds s, size_t inc) {
  unsigned char flags = s[-1];
  switch (flags & YK__SDS_TYPE_MASK) {
    case YK__SDS_TYPE_5: {
      unsigned char *fp = ((unsigned char *) s) - 1;
      unsigned char newlen = YK__SDS_TYPE_5_LEN(flags) + inc;
      *fp = YK__SDS_TYPE_5 | (newlen << YK__SDS_TYPE_BITS);
    } break;
    case YK__SDS_TYPE_8:
      YK__SDS_HDR(8, s)->len += inc;
      break;
    case YK__SDS_TYPE_16:
      YK__SDS_HDR(16, s)->len += inc;
      break;
    case YK__SDS_TYPE_32:
      YK__SDS_HDR(32, s)->len += inc;
      break;
    case YK__SDS_TYPE_64:
      YK__SDS_HDR(64, s)->len += inc;
      break;
  }
}
static inline size_t yk__sdsalloc(const yk__sds s) {
  unsigned char flags = s[-1];
  switch (flags & YK__SDS_TYPE_MASK) {
    case YK__SDS_TYPE_5:
      return YK__SDS_TYPE_5_LEN(flags);
    case YK__SDS_TYPE_8:
      return YK__SDS_HDR(8, s)->alloc;
    case YK__SDS_TYPE_16:
      return YK__SDS_HDR(16, s)->alloc;
    case YK__SDS_TYPE_32:
      return YK__SDS_HDR(32, s)->alloc;
    case YK__SDS_TYPE_64:
      return YK__SDS_HDR(64, s)->alloc;
  }
  return 0;
}
static inline void yk__sdssetalloc(yk__sds s, size_t newlen) {
  unsigned char flags = s[-1];
  switch (flags & YK__SDS_TYPE_MASK) {
    case YK__SDS_TYPE_5:
      break;
    case YK__SDS_TYPE_8:
      YK__SDS_HDR(8, s)->alloc = newlen;
      break;
    case YK__SDS_TYPE_16:
      YK__SDS_HDR(16, s)->alloc = newlen;
      break;
    case YK__SDS_TYPE_32:
      YK__SDS_HDR(32, s)->alloc = newlen;
      break;
    case YK__SDS_TYPE_64:
      YK__SDS_HDR(64, s)->alloc = newlen;
      break;
  }
}
yk__sds yk__sdsnewlen(const void *init, size_t initlen);
yk__sds yk__sdsnew(const char *init);
yk__sds yk__sdsempty(void);
yk__sds yk__sdsdup(const yk__sds s);
void yk__sdsfree(yk__sds s);
yk__sds yk__sdsgrowzero(yk__sds s, size_t len);
yk__sds yk__sdscatlen(yk__sds s, const void *t, size_t len);
yk__sds yk__sdscat(yk__sds s, const char *t);
yk__sds yk__sdscatsds(yk__sds s, const yk__sds t);
yk__sds yk__sdscpylen(yk__sds s, const char *t, size_t len);
yk__sds yk__sdscpy(yk__sds s, const char *t);
yk__sds yk__sdscatvprintf(yk__sds s, const char *fmt, va_list ap);
#ifdef __GNUC__
yk__sds yk__sdscatprintf(yk__sds s, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
yk__sds yk__sdscatprintf(yk__sds s, const char *fmt, ...);
#endif
yk__sds yk__sdscatfmt(yk__sds s, char const *fmt, ...);
yk__sds yk__sdstrim(yk__sds s, const char *cset);
void yk__sdsrange(yk__sds s, ssize_t start, ssize_t end);
void yk__sdsupdatelen(yk__sds s);
void yk__sdsclear(yk__sds s);
int yk__sdscmp(const yk__sds s1, const yk__sds s2);
yk__sds *yk__sdssplitlen(const char *s, ssize_t len, const char *sep,
                         int seplen, int *count);
void yk__sdsfreesplitres(yk__sds *tokens, int count);
void yk__sdstolower(yk__sds s);
void yk__sdstoupper(yk__sds s);
yk__sds yk__sdsfromlonglong(long long value);
yk__sds yk__sdscatrepr(yk__sds s, const char *p, size_t len);
yk__sds *yk__sdssplitargs(const char *line, int *argc);
yk__sds yk__sdsmapchars(yk__sds s, const char *from, const char *to,
                        size_t setlen);
yk__sds yk__sdsjoin(char **argv, int argc, char *sep);
yk__sds yk__sdsjoinsds(yk__sds *argv, int argc, const char *sep, size_t seplen);
yk__sds yk__sdsMakeRoomFor(yk__sds s, size_t addlen);
void yk__sdsIncrLen(yk__sds s, ssize_t incr);
yk__sds yk__sdsRemoveFreeSpace(yk__sds s);
size_t yk__sdsAllocSize(yk__sds s);
void *yk__sdsAllocPtr(yk__sds s);
void *yk__sds_malloc(size_t size);
void *yk__sds_realloc(void *ptr, size_t size);
void yk__sds_free(void *ptr);
#ifdef REDIS_TEST
int yk__sdsTest(int argc, char *argv[]);
#endif
#endif
#endif /* YK__SDS_SINGLE_HEADER */
#ifdef YK__SDS_IMPLEMENTATION
#define yk__s_malloc malloc
#define yk__s_realloc realloc
#define yk__s_free free
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const char *YK__SDS_NOINIT = "YK__SDS_NOINIT";
static inline int yk__sdsHdrSize(char type) {
  switch (type & YK__SDS_TYPE_MASK) {
    case YK__SDS_TYPE_5:
      return sizeof(struct yk__sdshdr5);
    case YK__SDS_TYPE_8:
      return sizeof(struct yk__sdshdr8);
    case YK__SDS_TYPE_16:
      return sizeof(struct yk__sdshdr16);
    case YK__SDS_TYPE_32:
      return sizeof(struct yk__sdshdr32);
    case YK__SDS_TYPE_64:
      return sizeof(struct yk__sdshdr64);
  }
  return 0;
}
static inline char yk__sdsReqType(size_t string_size) {
  if (string_size < 1 << 5) return YK__SDS_TYPE_5;
  if (string_size < 1 << 8) return YK__SDS_TYPE_8;
  if (string_size < 1 << 16) return YK__SDS_TYPE_16;
#if (LONG_MAX == LLONG_MAX)
  if (string_size < 1ll << 32) return YK__SDS_TYPE_32;
  return YK__SDS_TYPE_64;
#else
  return YK__SDS_TYPE_32;
#endif
}
yk__sds yk__sdsnewlen(const void *init, size_t initlen) {
  void *sh;
  yk__sds s;
  char type = yk__sdsReqType(initlen);
  if (type == YK__SDS_TYPE_5 && initlen == 0) type = YK__SDS_TYPE_8;
  int hdrlen = yk__sdsHdrSize(type);
  unsigned char *fp;
  sh = yk__s_malloc(hdrlen + initlen + 1);
  if (sh == NULL) return NULL;
  if (init == YK__SDS_NOINIT) init = NULL;
  else if (!init)
    memset(sh, 0, hdrlen + initlen + 1);
  s = (char *) sh + hdrlen;
  fp = ((unsigned char *) s) - 1;
  switch (type) {
    case YK__SDS_TYPE_5: {
      *fp = type | (initlen << YK__SDS_TYPE_BITS);
      break;
    }
    case YK__SDS_TYPE_8: {
      YK__SDS_HDR_VAR(8, s);
      sh->len = initlen;
      sh->alloc = initlen;
      *fp = type;
      break;
    }
    case YK__SDS_TYPE_16: {
      YK__SDS_HDR_VAR(16, s);
      sh->len = initlen;
      sh->alloc = initlen;
      *fp = type;
      break;
    }
    case YK__SDS_TYPE_32: {
      YK__SDS_HDR_VAR(32, s);
      sh->len = initlen;
      sh->alloc = initlen;
      *fp = type;
      break;
    }
    case YK__SDS_TYPE_64: {
      YK__SDS_HDR_VAR(64, s);
      sh->len = initlen;
      sh->alloc = initlen;
      *fp = type;
      break;
    }
  }
  if (initlen && init) memcpy(s, init, initlen);
  s[initlen] = '\0';
  return s;
}
yk__sds yk__sdsempty(void) { return yk__sdsnewlen("", 0); }
yk__sds yk__sdsnew(const char *init) {
  size_t initlen = (init == NULL) ? 0 : strlen(init);
  return yk__sdsnewlen(init, initlen);
}
yk__sds yk__sdsdup(const yk__sds s) { return yk__sdsnewlen(s, yk__sdslen(s)); }
void yk__sdsfree(yk__sds s) {
  if (s == NULL) return;
  yk__s_free((char *) s - yk__sdsHdrSize(s[-1]));
}
void yk__sdsupdatelen(yk__sds s) {
  size_t reallen = strlen(s);
  yk__sdssetlen(s, reallen);
}
void yk__sdsclear(yk__sds s) {
  yk__sdssetlen(s, 0);
  s[0] = '\0';
}
yk__sds yk__sdsMakeRoomFor(yk__sds s, size_t addlen) {
  void *sh, *newsh;
  size_t avail = yk__sdsavail(s);
  size_t len, newlen;
  char type, oldtype = s[-1] & YK__SDS_TYPE_MASK;
  int hdrlen;
  if (avail >= addlen) return s;
  len = yk__sdslen(s);
  sh = (char *) s - yk__sdsHdrSize(oldtype);
  newlen = (len + addlen);
  if (newlen < YK__SDS_MAX_PREALLOC) newlen *= 2;
  else
    newlen += YK__SDS_MAX_PREALLOC;
  type = yk__sdsReqType(newlen);
  if (type == YK__SDS_TYPE_5) type = YK__SDS_TYPE_8;
  hdrlen = yk__sdsHdrSize(type);
  if (oldtype == type) {
    newsh = yk__s_realloc(sh, hdrlen + newlen + 1);
    if (newsh == NULL) return NULL;
    s = (char *) newsh + hdrlen;
  } else {
    newsh = yk__s_malloc(hdrlen + newlen + 1);
    if (newsh == NULL) return NULL;
    memcpy((char *) newsh + hdrlen, s, len + 1);
    yk__s_free(sh);
    s = (char *) newsh + hdrlen;
    s[-1] = type;
    yk__sdssetlen(s, len);
  }
  yk__sdssetalloc(s, newlen);
  return s;
}
yk__sds yk__sdsRemoveFreeSpace(yk__sds s) {
  void *sh, *newsh;
  char type, oldtype = s[-1] & YK__SDS_TYPE_MASK;
  int hdrlen, oldhdrlen = yk__sdsHdrSize(oldtype);
  size_t len = yk__sdslen(s);
  size_t avail = yk__sdsavail(s);
  sh = (char *) s - oldhdrlen;
  if (avail == 0) return s;
  type = yk__sdsReqType(len);
  hdrlen = yk__sdsHdrSize(type);
  if (oldtype == type || type > YK__SDS_TYPE_8) {
    newsh = yk__s_realloc(sh, oldhdrlen + len + 1);
    if (newsh == NULL) return NULL;
    s = (char *) newsh + oldhdrlen;
  } else {
    newsh = yk__s_malloc(hdrlen + len + 1);
    if (newsh == NULL) return NULL;
    memcpy((char *) newsh + hdrlen, s, len + 1);
    yk__s_free(sh);
    s = (char *) newsh + hdrlen;
    s[-1] = type;
    yk__sdssetlen(s, len);
  }
  yk__sdssetalloc(s, len);
  return s;
}
size_t yk__sdsAllocSize(yk__sds s) {
  size_t alloc = yk__sdsalloc(s);
  return yk__sdsHdrSize(s[-1]) + alloc + 1;
}
void *yk__sdsAllocPtr(yk__sds s) {
  return (void *) (s - yk__sdsHdrSize(s[-1]));
}
void yk__sdsIncrLen(yk__sds s, ssize_t incr) {
  unsigned char flags = s[-1];
  size_t len;
  switch (flags & YK__SDS_TYPE_MASK) {
    case YK__SDS_TYPE_5: {
      unsigned char *fp = ((unsigned char *) s) - 1;
      unsigned char oldlen = YK__SDS_TYPE_5_LEN(flags);
      assert((incr > 0 && oldlen + incr < 32) ||
             (incr < 0 && oldlen >= (unsigned int) (-incr)));
      *fp = YK__SDS_TYPE_5 | ((oldlen + incr) << YK__SDS_TYPE_BITS);
      len = oldlen + incr;
      break;
    }
    case YK__SDS_TYPE_8: {
      YK__SDS_HDR_VAR(8, s);
      assert((incr >= 0 && sh->alloc - sh->len >= incr) ||
             (incr < 0 && sh->len >= (unsigned int) (-incr)));
      len = (sh->len += incr);
      break;
    }
    case YK__SDS_TYPE_16: {
      YK__SDS_HDR_VAR(16, s);
      assert((incr >= 0 && sh->alloc - sh->len >= incr) ||
             (incr < 0 && sh->len >= (unsigned int) (-incr)));
      len = (sh->len += incr);
      break;
    }
    case YK__SDS_TYPE_32: {
      YK__SDS_HDR_VAR(32, s);
      assert((incr >= 0 && sh->alloc - sh->len >= (unsigned int) incr) ||
             (incr < 0 && sh->len >= (unsigned int) (-incr)));
      len = (sh->len += incr);
      break;
    }
    case YK__SDS_TYPE_64: {
      YK__SDS_HDR_VAR(64, s);
      assert((incr >= 0 && sh->alloc - sh->len >= (uint64_t) incr) ||
             (incr < 0 && sh->len >= (uint64_t) (-incr)));
      len = (sh->len += incr);
      break;
    }
    default:
      len = 0;
  }
  s[len] = '\0';
}
yk__sds yk__sdsgrowzero(yk__sds s, size_t len) {
  size_t curlen = yk__sdslen(s);
  if (len <= curlen) return s;
  s = yk__sdsMakeRoomFor(s, len - curlen);
  if (s == NULL) return NULL;
  memset(s + curlen, 0, (len - curlen + 1));
  yk__sdssetlen(s, len);
  return s;
}
yk__sds yk__sdscatlen(yk__sds s, const void *t, size_t len) {
  size_t curlen = yk__sdslen(s);
  s = yk__sdsMakeRoomFor(s, len);
  if (s == NULL) return NULL;
  memcpy(s + curlen, t, len);
  yk__sdssetlen(s, curlen + len);
  s[curlen + len] = '\0';
  return s;
}
yk__sds yk__sdscat(yk__sds s, const char *t) {
  return yk__sdscatlen(s, t, strlen(t));
}
yk__sds yk__sdscatsds(yk__sds s, const yk__sds t) {
  return yk__sdscatlen(s, t, yk__sdslen(t));
}
yk__sds yk__sdscpylen(yk__sds s, const char *t, size_t len) {
  if (yk__sdsalloc(s) < len) {
    s = yk__sdsMakeRoomFor(s, len - yk__sdslen(s));
    if (s == NULL) return NULL;
  }
  memcpy(s, t, len);
  s[len] = '\0';
  yk__sdssetlen(s, len);
  return s;
}
yk__sds yk__sdscpy(yk__sds s, const char *t) {
  return yk__sdscpylen(s, t, strlen(t));
}
#define YK__SDS_LLSTR_SIZE 21
int yk__sdsll2str(char *s, long long value) {
  char *p, aux;
  unsigned long long v;
  size_t l;
  v = (value < 0) ? -value : value;
  p = s;
  do {
    *p++ = '0' + (v % 10);
    v /= 10;
  } while (v);
  if (value < 0) *p++ = '-';
  l = p - s;
  *p = '\0';
  p--;
  while (s < p) {
    aux = *s;
    *s = *p;
    *p = aux;
    s++;
    p--;
  }
  return l;
}
int yk__sdsull2str(char *s, unsigned long long v) {
  char *p, aux;
  size_t l;
  p = s;
  do {
    *p++ = '0' + (v % 10);
    v /= 10;
  } while (v);
  l = p - s;
  *p = '\0';
  p--;
  while (s < p) {
    aux = *s;
    *s = *p;
    *p = aux;
    s++;
    p--;
  }
  return l;
}
yk__sds yk__sdsfromlonglong(long long value) {
  char buf[YK__SDS_LLSTR_SIZE];
  int len = yk__sdsll2str(buf, value);
  return yk__sdsnewlen(buf, len);
}
yk__sds yk__sdscatvprintf(yk__sds s, const char *fmt, va_list ap) {
  va_list cpy;
  char staticbuf[1024], *buf = staticbuf, *t;
  size_t buflen = strlen(fmt) * 2;
  if (buflen > sizeof(staticbuf)) {
    buf = yk__s_malloc(buflen);
    if (buf == NULL) return NULL;
  } else {
    buflen = sizeof(staticbuf);
  }
  while (1) {
    buf[buflen - 2] = '\0';
    va_copy(cpy, ap);
    vsnprintf(buf, buflen, fmt, cpy);
    va_end(cpy);
    if (buf[buflen - 2] != '\0') {
      if (buf != staticbuf) yk__s_free(buf);
      buflen *= 2;
      buf = yk__s_malloc(buflen);
      if (buf == NULL) return NULL;
      continue;
    }
    break;
  }
  t = yk__sdscat(s, buf);
  if (buf != staticbuf) yk__s_free(buf);
  return t;
}
yk__sds yk__sdscatprintf(yk__sds s, const char *fmt, ...) {
  va_list ap;
  char *t;
  va_start(ap, fmt);
  t = yk__sdscatvprintf(s, fmt, ap);
  va_end(ap);
  return t;
}
yk__sds yk__sdscatfmt(yk__sds s, char const *fmt, ...) {
  size_t initlen = yk__sdslen(s);
  const char *f = fmt;
  long i;
  va_list ap;
  s = yk__sdsMakeRoomFor(s, initlen + strlen(fmt) * 2);
  va_start(ap, fmt);
  f = fmt;
  i = initlen;
  while (*f) {
    char next, *str;
    size_t l;
    long long num;
    unsigned long long unum;
    if (yk__sdsavail(s) == 0) { s = yk__sdsMakeRoomFor(s, 1); }
    switch (*f) {
      case '%':
        next = *(f + 1);
        f++;
        switch (next) {
          case 's':
          case 'S':
            str = va_arg(ap, char *);
            l = (next == 's') ? strlen(str) : yk__sdslen(str);
            if (yk__sdsavail(s) < l) { s = yk__sdsMakeRoomFor(s, l); }
            memcpy(s + i, str, l);
            yk__sdsinclen(s, l);
            i += l;
            break;
          case 'i':
          case 'I':
            if (next == 'i') num = va_arg(ap, int);
            else
              num = va_arg(ap, long long);
            {
              char buf[YK__SDS_LLSTR_SIZE];
              l = yk__sdsll2str(buf, num);
              if (yk__sdsavail(s) < l) { s = yk__sdsMakeRoomFor(s, l); }
              memcpy(s + i, buf, l);
              yk__sdsinclen(s, l);
              i += l;
            }
            break;
          case 'u':
          case 'U':
            if (next == 'u') unum = va_arg(ap, unsigned int);
            else
              unum = va_arg(ap, unsigned long long);
            {
              char buf[YK__SDS_LLSTR_SIZE];
              l = yk__sdsull2str(buf, unum);
              if (yk__sdsavail(s) < l) { s = yk__sdsMakeRoomFor(s, l); }
              memcpy(s + i, buf, l);
              yk__sdsinclen(s, l);
              i += l;
            }
            break;
          default:
            s[i++] = next;
            yk__sdsinclen(s, 1);
            break;
        }
        break;
      default:
        s[i++] = *f;
        yk__sdsinclen(s, 1);
        break;
    }
    f++;
  }
  va_end(ap);
  s[i] = '\0';
  return s;
}
yk__sds yk__sdstrim(yk__sds s, const char *cset) {
  char *start, *end, *sp, *ep;
  size_t len;
  sp = start = s;
  ep = end = s + yk__sdslen(s) - 1;
  while (sp <= end && strchr(cset, *sp)) sp++;
  while (ep > sp && strchr(cset, *ep)) ep--;
  len = (sp > ep) ? 0 : ((ep - sp) + 1);
  if (s != sp) memmove(s, sp, len);
  s[len] = '\0';
  yk__sdssetlen(s, len);
  return s;
}
void yk__sdsrange(yk__sds s, ssize_t start, ssize_t end) {
  size_t newlen, len = yk__sdslen(s);
  if (len == 0) return;
  if (start < 0) {
    start = len + start;
    if (start < 0) start = 0;
  }
  if (end < 0) {
    end = len + end;
    if (end < 0) end = 0;
  }
  newlen = (start > end) ? 0 : (end - start) + 1;
  if (newlen != 0) {
    if (start >= (ssize_t) len) {
      newlen = 0;
    } else if (end >= (ssize_t) len) {
      end = len - 1;
      newlen = (start > end) ? 0 : (end - start) + 1;
    }
  } else {
    start = 0;
  }
  if (start && newlen) memmove(s, s + start, newlen);
  s[newlen] = 0;
  yk__sdssetlen(s, newlen);
}
void yk__sdstolower(yk__sds s) {
  size_t len = yk__sdslen(s), j;
  for (j = 0; j < len; j++) s[j] = tolower(s[j]);
}
void yk__sdstoupper(yk__sds s) {
  size_t len = yk__sdslen(s), j;
  for (j = 0; j < len; j++) s[j] = toupper(s[j]);
}
int yk__sdscmp(const yk__sds s1, const yk__sds s2) {
  size_t l1, l2, minlen;
  int cmp;
  l1 = yk__sdslen(s1);
  l2 = yk__sdslen(s2);
  minlen = (l1 < l2) ? l1 : l2;
  cmp = memcmp(s1, s2, minlen);
  if (cmp == 0) return l1 > l2 ? 1 : (l1 < l2 ? -1 : 0);
  return cmp;
}
yk__sds *yk__sdssplitlen(const char *s, ssize_t len, const char *sep,
                         int seplen, int *count) {
  int elements = 0, slots = 5;
  long start = 0, j;
  yk__sds *tokens;
  if (seplen < 1 || len < 0) return NULL;
  tokens = yk__s_malloc(sizeof(yk__sds) * slots);
  if (tokens == NULL) return NULL;
  if (len == 0) {
    *count = 0;
    return tokens;
  }
  for (j = 0; j < (len - (seplen - 1)); j++) {
    if (slots < elements + 2) {
      yk__sds *newtokens;
      slots *= 2;
      newtokens = yk__s_realloc(tokens, sizeof(yk__sds) * slots);
      if (newtokens == NULL) goto cleanup;
      tokens = newtokens;
    }
    if ((seplen == 1 && *(s + j) == sep[0]) ||
        (memcmp(s + j, sep, seplen) == 0)) {
      tokens[elements] = yk__sdsnewlen(s + start, j - start);
      if (tokens[elements] == NULL) goto cleanup;
      elements++;
      start = j + seplen;
      j = j + seplen - 1;
    }
  }
  tokens[elements] = yk__sdsnewlen(s + start, len - start);
  if (tokens[elements] == NULL) goto cleanup;
  elements++;
  *count = elements;
  return tokens;
cleanup : {
  int i;
  for (i = 0; i < elements; i++) yk__sdsfree(tokens[i]);
  yk__s_free(tokens);
  *count = 0;
  return NULL;
}
}
void yk__sdsfreesplitres(yk__sds *tokens, int count) {
  if (!tokens) return;
  while (count--) yk__sdsfree(tokens[count]);
  yk__s_free(tokens);
}
yk__sds yk__sdscatrepr(yk__sds s, const char *p, size_t len) {
  s = yk__sdscatlen(s, "\"", 1);
  while (len--) {
    switch (*p) {
      case '\\':
      case '"':
        s = yk__sdscatprintf(s, "\\%c", *p);
        break;
      case '\n':
        s = yk__sdscatlen(s, "\\n", 2);
        break;
      case '\r':
        s = yk__sdscatlen(s, "\\r", 2);
        break;
      case '\t':
        s = yk__sdscatlen(s, "\\t", 2);
        break;
      case '\a':
        s = yk__sdscatlen(s, "\\a", 2);
        break;
      case '\b':
        s = yk__sdscatlen(s, "\\b", 2);
        break;
      default:
        if (isprint(*p)) s = yk__sdscatprintf(s, "%c", *p);
        else
          s = yk__sdscatprintf(s, "\\x%02x", (unsigned char) *p);
        break;
    }
    p++;
  }
  return yk__sdscatlen(s, "\"", 1);
}
int is_hex_digit(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}
int hex_digit_to_int(char c) {
  switch (c) {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'a':
    case 'A':
      return 10;
    case 'b':
    case 'B':
      return 11;
    case 'c':
    case 'C':
      return 12;
    case 'd':
    case 'D':
      return 13;
    case 'e':
    case 'E':
      return 14;
    case 'f':
    case 'F':
      return 15;
    default:
      return 0;
  }
}
yk__sds *yk__sdssplitargs(const char *line, int *argc) {
  const char *p = line;
  char *current = NULL;
  char **vector = NULL;
  *argc = 0;
  while (1) {
    while (*p && isspace(*p)) p++;
    if (*p) {
      int inq = 0;
      int insq = 0;
      int done = 0;
      if (current == NULL) current = yk__sdsempty();
      while (!done) {
        if (inq) {
          if (*p == '\\' && *(p + 1) == 'x' && is_hex_digit(*(p + 2)) &&
              is_hex_digit(*(p + 3))) {
            unsigned char byte;
            byte =
                (hex_digit_to_int(*(p + 2)) * 16) + hex_digit_to_int(*(p + 3));
            current = yk__sdscatlen(current, (char *) &byte, 1);
            p += 3;
          } else if (*p == '\\' && *(p + 1)) {
            char c;
            p++;
            switch (*p) {
              case 'n':
                c = '\n';
                break;
              case 'r':
                c = '\r';
                break;
              case 't':
                c = '\t';
                break;
              case 'b':
                c = '\b';
                break;
              case 'a':
                c = '\a';
                break;
              default:
                c = *p;
                break;
            }
            current = yk__sdscatlen(current, &c, 1);
          } else if (*p == '"') {
            if (*(p + 1) && !isspace(*(p + 1))) goto err;
            done = 1;
          } else if (!*p) {
            goto err;
          } else {
            current = yk__sdscatlen(current, p, 1);
          }
        } else if (insq) {
          if (*p == '\\' && *(p + 1) == '\'') {
            p++;
            current = yk__sdscatlen(current, "'", 1);
          } else if (*p == '\'') {
            if (*(p + 1) && !isspace(*(p + 1))) goto err;
            done = 1;
          } else if (!*p) {
            goto err;
          } else {
            current = yk__sdscatlen(current, p, 1);
          }
        } else {
          switch (*p) {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
            case '\0':
              done = 1;
              break;
            case '"':
              inq = 1;
              break;
            case '\'':
              insq = 1;
              break;
            default:
              current = yk__sdscatlen(current, p, 1);
              break;
          }
        }
        if (*p) p++;
      }
      vector = yk__s_realloc(vector, ((*argc) + 1) * sizeof(char *));
      vector[*argc] = current;
      (*argc)++;
      current = NULL;
    } else {
      if (vector == NULL) vector = yk__s_malloc(sizeof(void *));
      return vector;
    }
  }
err:
  while ((*argc)--) yk__sdsfree(vector[*argc]);
  yk__s_free(vector);
  if (current) yk__sdsfree(current);
  *argc = 0;
  return NULL;
}
yk__sds yk__sdsmapchars(yk__sds s, const char *from, const char *to,
                        size_t setlen) {
  size_t j, i, l = yk__sdslen(s);
  for (j = 0; j < l; j++) {
    for (i = 0; i < setlen; i++) {
      if (s[j] == from[i]) {
        s[j] = to[i];
        break;
      }
    }
  }
  return s;
}
yk__sds yk__sdsjoin(char **argv, int argc, char *sep) {
  yk__sds join = yk__sdsempty();
  int j;
  for (j = 0; j < argc; j++) {
    join = yk__sdscat(join, argv[j]);
    if (j != argc - 1) join = yk__sdscat(join, sep);
  }
  return join;
}
yk__sds yk__sdsjoinsds(yk__sds *argv, int argc, const char *sep,
                       size_t seplen) {
  yk__sds join = yk__sdsempty();
  int j;
  for (j = 0; j < argc; j++) {
    join = yk__sdscatsds(join, argv[j]);
    if (j != argc - 1) join = yk__sdscatlen(join, sep, seplen);
  }
  return join;
}
void *yk__sds_malloc(size_t size) { return yk__s_malloc(size); }
void *yk__sds_realloc(void *ptr, size_t size) {
  return yk__s_realloc(ptr, size);
}
void yk__sds_free(void *ptr) { yk__s_free(ptr); }
#if defined(SDS_TEST_MAIN)
#include "limits.h"
#include "testhelp.h"
#include <stdio.h>
#define UNUSED(x) (void) (x)
int yk__sdsTest(void) {
  {
    yk__sds x = yk__sdsnew("foo"), y;
    test_cond("Create a string and obtain the length",
              yk__sdslen(x) == 3 && memcmp(x, "foo\0", 4) == 0) yk__sdsfree(x);
    x = yk__sdsnewlen("foo", 2);
    test_cond("Create a string with specified length",
              yk__sdslen(x) == 2 && memcmp(x, "fo\0", 3) == 0) x =
        yk__sdscat(x, "bar");
    test_cond("Strings concatenation",
              yk__sdslen(x) == 5 && memcmp(x, "fobar\0", 6) == 0);
    x = yk__sdscpy(x, "a");
    test_cond("yk__sdscpy() against an originally longer string",
              yk__sdslen(x) == 1 && memcmp(x, "a\0", 2) == 0) x =
        yk__sdscpy(x, "xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk");
    test_cond("yk__sdscpy() against an originally shorter string",
              yk__sdslen(x) == 33 &&
                  memcmp(x, "xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk\0", 33) == 0)
        yk__sdsfree(x);
    x = yk__sdscatprintf(yk__sdsempty(), "%d", 123);
    test_cond("yk__sdscatprintf() seems working in the base case",
              yk__sdslen(x) == 3 && memcmp(x, "123\0", 4) == 0) yk__sdsfree(x);
    x = yk__sdsnew("--");
    x = yk__sdscatfmt(x, "Hello %s World %I,%I--", "Hi!", LLONG_MIN, LLONG_MAX);
    test_cond("yk__sdscatfmt() seems working in the base case",
              yk__sdslen(x) == 60 &&
                  memcmp(x,
                         "--Hello Hi! World -9223372036854775808,"
                         "9223372036854775807--",
                         60) == 0) printf("[%s]\n", x);
    yk__sdsfree(x);
    x = yk__sdsnew("--");
    x = yk__sdscatfmt(x, "%u,%U--", UINT_MAX, ULLONG_MAX);
    test_cond("yk__sdscatfmt() seems working with unsigned numbers",
              yk__sdslen(x) == 35 &&
                  memcmp(x, "--4294967295,18446744073709551615--", 35) == 0)
        yk__sdsfree(x);
    x = yk__sdsnew(" x ");
    yk__sdstrim(x, " x");
    test_cond("yk__sdstrim() works when all chars match", yk__sdslen(x) == 0)
        yk__sdsfree(x);
    x = yk__sdsnew(" x ");
    yk__sdstrim(x, " ");
    test_cond("yk__sdstrim() works when a single char remains",
              yk__sdslen(x) == 1 && x[0] == 'x') yk__sdsfree(x);
    x = yk__sdsnew("xxciaoyyy");
    yk__sdstrim(x, "xy");
    test_cond("yk__sdstrim() correctly trims characters",
              yk__sdslen(x) == 4 && memcmp(x, "ciao\0", 5) == 0) y =
        yk__sdsdup(x);
    yk__sdsrange(y, 1, 1);
    test_cond("yk__sdsrange(...,1,1)",
              yk__sdslen(y) == 1 && memcmp(y, "i\0", 2) == 0) yk__sdsfree(y);
    y = yk__sdsdup(x);
    yk__sdsrange(y, 1, -1);
    test_cond("yk__sdsrange(...,1,-1)",
              yk__sdslen(y) == 3 && memcmp(y, "iao\0", 4) == 0) yk__sdsfree(y);
    y = yk__sdsdup(x);
    yk__sdsrange(y, -2, -1);
    test_cond("yk__sdsrange(...,-2,-1)",
              yk__sdslen(y) == 2 && memcmp(y, "ao\0", 3) == 0) yk__sdsfree(y);
    y = yk__sdsdup(x);
    yk__sdsrange(y, 2, 1);
    test_cond("yk__sdsrange(...,2,1)",
              yk__sdslen(y) == 0 && memcmp(y, "\0", 1) == 0) yk__sdsfree(y);
    y = yk__sdsdup(x);
    yk__sdsrange(y, 1, 100);
    test_cond("yk__sdsrange(...,1,100)",
              yk__sdslen(y) == 3 && memcmp(y, "iao\0", 4) == 0) yk__sdsfree(y);
    y = yk__sdsdup(x);
    yk__sdsrange(y, 100, 100);
    test_cond("yk__sdsrange(...,100,100)",
              yk__sdslen(y) == 0 && memcmp(y, "\0", 1) == 0) yk__sdsfree(y);
    yk__sdsfree(x);
    x = yk__sdsnew("foo");
    y = yk__sdsnew("foa");
    test_cond("yk__sdscmp(foo,foa)", yk__sdscmp(x, y) > 0) yk__sdsfree(y);
    yk__sdsfree(x);
    x = yk__sdsnew("bar");
    y = yk__sdsnew("bar");
    test_cond("yk__sdscmp(bar,bar)", yk__sdscmp(x, y) == 0) yk__sdsfree(y);
    yk__sdsfree(x);
    x = yk__sdsnew("aar");
    y = yk__sdsnew("bar");
    test_cond("yk__sdscmp(bar,bar)", yk__sdscmp(x, y) < 0) yk__sdsfree(y);
    yk__sdsfree(x);
    x = yk__sdsnewlen("\a\n\0foo\r", 7);
    y = yk__sdscatrepr(yk__sdsempty(), x, yk__sdslen(x));
    test_cond("yk__sdscatrepr(...data...)",
              memcmp(y, "\"\\a\\n\\x00foo\\r\"", 15) == 0) {
      unsigned int oldfree;
      char *p;
      int step = 10, j, i;
      yk__sdsfree(x);
      yk__sdsfree(y);
      x = yk__sdsnew("0");
      test_cond("yk__sdsnew() free/len buffers",
                yk__sdslen(x) == 1 && yk__sdsavail(x) == 0);
      for (i = 0; i < 10; i++) {
        int oldlen = yk__sdslen(x);
        x = yk__sdsMakeRoomFor(x, step);
        int type = x[-1] & YK__SDS_TYPE_MASK;
        test_cond("yk__sdsMakeRoomFor() len", yk__sdslen(x) == oldlen);
        if (type != YK__SDS_TYPE_5) {
          test_cond("yk__sdsMakeRoomFor() free", yk__sdsavail(x) >= step);
          oldfree = yk__sdsavail(x);
        }
        p = x + oldlen;
        for (j = 0; j < step; j++) { p[j] = 'A' + j; }
        yk__sdsIncrLen(x, step);
      }
      test_cond("yk__sdsMakeRoomFor() content",
                memcmp("0ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCD"
                       "EFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ",
                       x, 101) == 0);
      test_cond("yk__sdsMakeRoomFor() final length", yk__sdslen(x) == 101);
      yk__sdsfree(x);
    }
  }
  test_report() return 0;
}
#endif
#ifdef SDS_TEST_MAIN
int main(void) { return yk__sdsTest(); }
#endif
#endif /* YK__SDS_IMPLEMENTATION */
/*
*/
