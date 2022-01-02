/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

yk__ini.h - v1.2 - Simple yk__ini-file reader for C/C++.

Do this:
    #define YK__INI_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/
#ifndef yk__ini_h
#define yk__ini_h
#define YK__INI_GLOBAL_SECTION (0)
#define YK__INI_NOT_FOUND (-1)
typedef struct yk__ini_t yk__ini_t;
yk__ini_t *yk__ini_create(void *memctx);
yk__ini_t *yk__ini_load(char const *data, void *memctx);
int yk__ini_save(yk__ini_t const *yk__ini, char *data, int size);
void yk__ini_destroy(yk__ini_t *yk__ini);
int yk__ini_section_count(yk__ini_t const *yk__ini);
char const *yk__ini_section_name(yk__ini_t const *yk__ini, int section);
int yk__ini_property_count(yk__ini_t const *yk__ini, int section);
char const *yk__ini_property_name(yk__ini_t const *yk__ini, int section,
                                  int property);
char const *yk__ini_property_value(yk__ini_t const *yk__ini, int section,
                                   int property);
int yk__ini_find_section(yk__ini_t const *yk__ini, char const *name,
                         int name_length);
int yk__ini_find_property(yk__ini_t const *yk__ini, int section,
                          char const *name, int name_length);
int yk__ini_section_add(yk__ini_t *yk__ini, char const *name, int length);
void yk__ini_property_add(yk__ini_t *yk__ini, int section, char const *name,
                          int name_length, char const *value, int value_length);
void yk__ini_section_remove(yk__ini_t *yk__ini, int section);
void yk__ini_property_remove(yk__ini_t *yk__ini, int section, int property);
void yk__ini_section_name_set(yk__ini_t *yk__ini, int section, char const *name,
                              int length);
void yk__ini_property_name_set(yk__ini_t *yk__ini, int section, int property,
                               char const *name, int length);
void yk__ini_property_value_set(yk__ini_t *yk__ini, int section, int property,
                                char const *value, int length);
#endif /* yk__ini_h */
/**

yk__ini.h 
=====

Simple yk__ini-file reader for C/C++.


Examples
--------

#### Loading an yk__ini file and retrieving values

    #define YK__INI_IMPLEMENTATION
    #include "yk__ini.h"

    #include <stdio.h>
    #include <stdlib.h>

    int main()
        {
        FILE* fp = fopen( "test.yk__ini", "r" );
        fseek( fp, 0, SEEK_END );
        int size = ftell( fp );
        fseek( fp, 0, SEEK_SET );
        char* data = (char*) malloc( size + 1 );
        fread( data, 1, size, fp );
        data[ size ] = '\0';
        fclose( fp );

        yk__ini_t* yk__ini = yk__ini_load( data );
        free( data );
        int second_index = yk__ini_find_property( yk__ini, YK__INI_GLOBAL_SECTION, "SecondSetting" );
        char const* second = yk__ini_property_value( yk__ini, YK__INI_GLOBAL_SECTION, second_index );
        printf( "%s=%s\n", "SecondSetting", second );
        int section = yk__ini_find_section( yk__ini, "MySection" );
        int third_index = yk__ini_find_property( yk__ini, section, "ThirdSetting" );
        char const* third = yk__ini_property_value( yk__ini, section, third_index );
        printf( "%s=%s\n", "ThirdSetting", third );
        yk__ini_destroy( yk__ini );

        return 0;
        }

-----------------------------------------------------------------------------------------------

#### Creating a new yk__ini file

    #define YK__INI_IMPLEMENTATION
    #include "yk__ini.h"

    #include <stdio.h>
    #include <stdlib.h>

    int main()
        {       
        yk__ini_t* yk__ini = yk__ini_create();
        yk__ini_property_add( yk__ini, YK__INI_GLOBAL_SECTION, "FirstSetting", "Test" );
        yk__ini_property_add( yk__ini, YK__INI_GLOBAL_SECTION, "SecondSetting", "2" );
        int section = yk__ini_section_add( yk__ini, "MySection" );
        yk__ini_property_add( yk__ini, section, "ThirdSetting", "Three" );

        int size = yk__ini_save( yk__ini, NULL, 0 ); // Find the size needed
        char* data = (char*) malloc( size );
        size = yk__ini_save( yk__ini, data, size ); // Actually save the file
        yk__ini_destroy( yk__ini );

        FILE* fp = fopen( "test.yk__ini", "w" );
        fwrite( data, 1, size, fp );
        fclose( fp );
        free( data );

        return 0;
        }



API Documentation
-----------------

yk__ini.h is a small library for reading classic .yk__ini files. It is a single-header library, and does not need any .lib files 
or other binaries, or any build scripts. To use it, you just include yk__ini.h to get the API declarations. To get the 
definitions, you must include yk__ini.h from *one* single C or C++ file, and #define the symbol `YK__INI_IMPLEMENTATION` before 
you do. 


### Customization

There are a few different things in yk__ini.h which are configurable by #defines. The customizations only affect the 
implementation, so will only need to be defined in the file where you have the #define YK__INI_IMPLEMENTATION.

Note that if all customizations are utilized, yk__ini.h will include no external files whatsoever, which might be useful
if you need full control over what code is being built.


#### Custom memory allocators

To store the internal data structures, yk__ini.h needs to do dynamic allocation by calling `malloc`. Programs might want to 
keep track of allocations done, or use custom defined pools to allocate memory from. yk__ini.h allows for specifying custom 
memory allocation functions for `malloc` and `free`.
This is done with the following code:

    #define YK__INI_IMPLEMENTATION
    #define YK__INI_MALLOC( ctx, size ) ( my_custom_malloc( ctx, size ) )
    #define YK__INI_FREE( ctx, ptr ) ( my_custom_free( ctx, ptr ) )
    #include "yk__ini.h"

where `my_custom_malloc` and `my_custom_free` are your own memory allocation/deallocation functions. The `ctx` parameter
is an optional parameter of type `void*`. When `yk__ini_create` or `yk__ini_load` is called, you can pass in a `memctx` 
parameter, which can be a pointer to anything you like, and which will be passed through as the `ctx` parameter to every 
`YK__INI_MALLOC`/`YK__INI_FREE` call. For example, if you are doing memory tracking, you can pass a pointer to your tracking 
data as `memctx`, and in your custom allocation/deallocation function, you can cast the `ctx` param back to the 
right type, and access the tracking data.

If no custom allocator is defined, yk__ini.h will default to `malloc` and `free` from the C runtime library.


#### Custom C runtime function

The library makes use of three additional functions from the C runtime library, and for full flexibility, it allows you 
to substitute them for your own. Here's an example:

    #define YK__INI_IMPLEMENTATION
    #define YK__INI_MEMCPY( dst, src, cnt ) ( my_memcpy_func( dst, src, cnt ) )
    #define YK__INI_STRLEN( s ) ( my_strlen_func( s ) )
    #define YK__INI_STRNICMP( s1, s2, cnt ) ( my_strnicmp_func( s1, s2, cnt ) )
    #include "yk__ini.h"

If no custom function is defined, yk__ini.h will default to the C runtime library equivalent.


yk__ini_create
----------
    
    yk__ini_t* yk__ini_create( void* memctx )

Instantiates a new, empty yk__ini structure, which can be manipulated with other API calls, to fill it with data. To save it
out to an yk__ini-file string, use `yk__ini_save`. When no longer needed, it can be destroyed by calling `yk__ini_destroy`.
`memctx` is a pointer to user defined data which will be passed through to the custom YK__INI_MALLOC/YK__INI_FREE calls. It can 
be NULL if no user defined data is needed.


yk__ini_load
--------

    yk__ini_t* yk__ini_load( char const* data, void* memctx )

Parse the zero-terminated string `data` containing an yk__ini-file, and create a new yk__ini_t instance containing the data. 
The instance can be manipulated with other API calls to enumerate sections/properties and retrieve values. When no 
longer needed, it can be destroyed by calling `yk__ini_destroy`. `memctx` is a pointer to user defined data which will be 
passed through to the custom YK__INI_MALLOC/YK__INI_FREE calls. It can be NULL if no user defined data is needed.


yk__ini_save
--------
    
    int yk__ini_save( yk__ini_t const* yk__ini, char* data, int size )

Saves an yk__ini structure as a zero-terminated yk__ini-file string, into the specified buffer. Returns the number of bytes 
written, including the zero terminator. If `data` is NULL, nothing is written, but `yk__ini_save` still returns the number
of bytes it would have written. If the size of `data`, as specified in the `size` parameter, is smaller than that 
required, only part of the yk__ini-file string will be written. `yk__ini_save` still returns the number of bytes it would have
written had the buffer been large enough.


yk__ini_destroy
-----------

    void yk__ini_destroy( yk__ini_t* yk__ini )

Destroy an `yk__ini_t` instance created by calling `yk__ini_load` or `yk__ini_create`, releasing the memory allocated by it. No
further API calls are valid on an `yk__ini_t` instance after calling `yk__ini_destroy` on it.


yk__ini_section_count
-----------------

    int yk__ini_section_count( yk__ini_t const* yk__ini )

Returns the number of sections in an yk__ini file. There's at least one section in an yk__ini file (the global section), but 
there can be many more, each specified in the file by the section name wrapped in square brackets [ ].


yk__ini_section_name
----------------

    char const* yk__ini_section_name( yk__ini_t const* yk__ini, int section )

Returns the name of the section with the specified index. `section` must be non-negative and less than the value 
returned by `yk__ini_section_count`, or `yk__ini_section_name` will return NULL. The defined constant `YK__INI_GLOBAL_SECTION` can
be used to indicate the global section.


yk__ini_property_count
------------------

    int yk__ini_property_count( yk__ini_t const* yk__ini, int section )

Returns the number of properties belonging to the section with the specified index. `section` must be non-negative and 
less than the value returned by `yk__ini_section_count`, or `yk__ini_section_name` will return 0. The defined constant 
`YK__INI_GLOBAL_SECTION` can be used to indicate the global section. Properties are declared in the yk__ini-file on he format
`name=value`.


yk__ini_property_name
-----------------

    char const* yk__ini_property_name( yk__ini_t const* yk__ini, int section, int property )

Returns the name of the property with the specified index `property` in the section with the specified index `section`.
`section` must be non-negative and less than the value returned by `yk__ini_section_count`, and `property` must be 
non-negative and less than the value returned by `yk__ini_property_count`, or `yk__ini_property_name` will return NULL. The 
defined constant `YK__INI_GLOBAL_SECTION` can be used to indicate the global section.


yk__ini_property_value
------------------

    char const* yk__ini_property_value( yk__ini_t const* yk__ini, int section, int property )

Returns the value of the property with the specified index `property` in the section with the specified index `section`.
`section` must be non-negative and less than the value returned by `yk__ini_section_count`, and `property` must be 
non-negative and less than the value returned by `yk__ini_property_count`, or `yk__ini_property_value` will return NULL. The 
defined constant `YK__INI_GLOBAL_SECTION` can be used to indicate the global section.


yk__ini_find_section
----------------

    int yk__ini_find_section( yk__ini_t const* yk__ini, char const* name, int name_length )

Finds the section with the specified name, and returns its index. `name_length` specifies the number of characters in
`name`, which does not have to be zero-terminated. If `name_length` is zero, the length is determined automatically, but
in this case `name` has to be zero-terminated. If no section with the specified name could be found, the value
`YK__INI_NOT_FOUND` is returned.


yk__ini_find_property
-----------------

    int yk__ini_find_property( yk__ini_t const* yk__ini, int section, char const* name, int name_length )

Finds the property with the specified name, within the section with the specified index, and returns the index of the 
property. `name_length` specifies the number of characters in `name`, which does not have to be zero-terminated. If 
`name_length` is zero, the length is determined automatically, but in this case `name` has to be zero-terminated. If no 
property with the specified name could be found within the specified section, the value `YK__INI_NOT_FOUND` is  returned.
`section` must be non-negative and less than the value returned by `yk__ini_section_count`, or `yk__ini_find_property` will 
return `YK__INI_NOT_FOUND`. The defined constant `YK__INI_GLOBAL_SECTION` can be used to indicate the global section.


yk__ini_section_add
---------------

    int yk__ini_section_add( yk__ini_t* yk__ini, char const* name, int length )

Adds a section with the specified name, and returns the index it was added at. There is no check done to see if a 
section with the specified name already exists - multiple sections of the same name are allowed. `length` specifies the 
number of characters in `name`, which does not have to be zero-terminated. If `length` is zero, the length is determined 
automatically, but in this case `name` has to be zero-terminated.


yk__ini_property_add
----------------
    
    void yk__ini_property_add( yk__ini_t* yk__ini, int section, char const* name, int name_length, char const* value, int value_length )

Adds a property with the specified name and value to the specified section, and returns the index it was added at. There 
is no check done to see if a property with the specified name already exists - multiple properties of the same name are 
allowed. `name_length` and `value_length` specifies the number of characters in `name` and `value`, which does not have 
to be zero-terminated. If `name_length` or `value_length` is zero, the length is determined automatically, but in this 
case `name`/`value` has to be zero-terminated. `section` must be non-negative and less than the value returned by
`yk__ini_section_count`, or the property will not be added. The defined constant `YK__INI_GLOBAL_SECTION` can be used to 
indicate the global section.


yk__ini_section_remove
------------------

    void yk__ini_section_remove( yk__ini_t* yk__ini, int section )

Removes the section with the specified index, and all properties within it. `section` must be non-negative and less than 
the value returned by `yk__ini_section_count`. The defined constant `YK__INI_GLOBAL_SECTION` can be used to indicate the global 
section. Note that removing a section will shuffle section indices, so that section indices you may have stored will no 
longer indicate the same section as it did before the remove. Use the find functions to update your indices.


yk__ini_property_remove
-------------------

    void yk__ini_property_remove( yk__ini_t* yk__ini, int section, int property )

Removes the property with the specified index from the specified section. `section` must be non-negative and less than 
the value returned by `yk__ini_section_count`, and `property` must be non-negative and less than the value returned by 
`yk__ini_property_count`. The defined constant `YK__INI_GLOBAL_SECTION` can be used to indicate the global section. Note that 
removing a property will shuffle property indices within the specified section, so that property indices you may have 
stored will no longer indicate the same property as it did before the remove. Use the find functions to update your 
indices.


yk__ini_section_name_set
--------------------

    void yk__ini_section_name_set( yk__ini_t* yk__ini, int section, char const* name, int length )

Change the name of the section with the specified index. `section` must be non-negative and less than the value returned 
by `yk__ini_section_count`. The defined constant `YK__INI_GLOBAL_SECTION` can be used to indicate the global section. `length` 
specifies the number of characters in `name`, which does not have to be zero-terminated. If `length` is zero, the length 
is determined automatically, but in this case `name` has to be zero-terminated.


yk__ini_property_name_set
---------------------

    void yk__ini_property_name_set( yk__ini_t* yk__ini, int section, int property, char const* name, int length )

Change the name of the property with the specified index in the specified section. `section` must be non-negative and 
less than the value returned by `yk__ini_section_count`, and `property` must be non-negative and less than the value 
returned by `yk__ini_property_count`. The defined constant `YK__INI_GLOBAL_SECTION` can be used to indicate the global section.
`length` specifies the number of characters in `name`, which does not have to be zero-terminated. If `length` is zero, 
the length is determined automatically, but in this case `name` has to be zero-terminated.


yk__ini_property_value_set
----------------------

    void yk__ini_property_value_set( yk__ini_t* yk__ini, int section, int property, char const* value, int length  )

Change the value of the property with the specified index in the specified section. `section` must be non-negative and 
less than the value returned by `yk__ini_section_count`, and `property` must be non-negative and less than the value 
returned by `yk__ini_property_count`. The defined constant `YK__INI_GLOBAL_SECTION` can be used to indicate the global section.
`length` specifies the number of characters in `value`, which does not have to be zero-terminated. If `length` is zero, 
the length is determined automatically, but in this case `value` has to be zero-terminated.

*/
/*
----------------------
    IMPLEMENTATION
----------------------
*/
#ifdef YK__INI_IMPLEMENTATION
#undef YK__INI_IMPLEMENTATION
#define YK__INITIAL_CAPACITY (256)
#undef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#undef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stddef.h>
#ifndef YK__INI_MALLOC
#include <stdlib.h>
#define YK__INI_MALLOC(ctx, size) (malloc(size))
#define YK__INI_FREE(ctx, ptr) (free(ptr))
#endif
#ifndef YK__INI_MEMCPY
#include <string.h>
#define YK__INI_MEMCPY(dst, src, cnt) (memcpy(dst, src, cnt))
#endif
#ifndef YK__INI_STRLEN
#include <string.h>
#define YK__INI_STRLEN(s) (strlen(s))
#endif
#ifndef YK__INI_STRNICMP
#ifdef _WIN32
#include <string.h>
#define YK__INI_STRNICMP(s1, s2, cnt) (strnicmp(s1, s2, cnt))
#else
#include <string.h>
#define YK__INI_STRNICMP(s1, s2, cnt) (strncasecmp(s1, s2, cnt))
#endif
#endif
struct yk__ini_internal_section_t {
  char name[32];
  char *name_large;
};
struct yk__ini_internal_property_t {
  int section;
  char name[32];
  char *name_large;
  char value[64];
  char *value_large;
};
struct yk__ini_t {
  struct yk__ini_internal_section_t *sections;
  int section_capacity;
  int section_count;
  struct yk__ini_internal_property_t *properties;
  int property_capacity;
  int property_count;
  void *memctx;
};
static int yk__ini_internal_property_index(yk__ini_t const *yk__ini,
                                           int section, int property) {
  int i;
  int p;
  if (yk__ini && section >= 0 && section < yk__ini->section_count) {
    p = 0;
    for (i = 0; i < yk__ini->property_count; ++i) {
      if (yk__ini->properties[i].section == section) {
        if (p == property) return i;
        ++p;
      }
    }
  }
  return YK__INI_NOT_FOUND;
}
yk__ini_t *yk__ini_create(void *memctx) {
  yk__ini_t *yk__ini;
  yk__ini = (yk__ini_t *) YK__INI_MALLOC(memctx, sizeof(yk__ini_t));
  yk__ini->memctx = memctx;
  yk__ini->sections = (struct yk__ini_internal_section_t *) YK__INI_MALLOC(
      yk__ini->memctx, YK__INITIAL_CAPACITY * sizeof(yk__ini->sections[0]));
  yk__ini->section_capacity = YK__INITIAL_CAPACITY;
  yk__ini->section_count = 1; /* global section */
  yk__ini->sections[0].name[0] = '\0';
  yk__ini->sections[0].name_large = 0;
  yk__ini->properties = (struct yk__ini_internal_property_t *) YK__INI_MALLOC(
      yk__ini->memctx, YK__INITIAL_CAPACITY * sizeof(yk__ini->properties[0]));
  yk__ini->property_capacity = YK__INITIAL_CAPACITY;
  yk__ini->property_count = 0;
  return yk__ini;
}
yk__ini_t *yk__ini_load(char const *data, void *memctx) {
  yk__ini_t *yk__ini;
  char const *ptr;
  int s;
  char const *start;
  char const *start2;
  int l;
  yk__ini = yk__ini_create(memctx);
  ptr = data;
  if (ptr) {
    s = 0;
    while (*ptr) {
      /* trim leading whitespace */
      while (*ptr && *ptr <= ' ') ++ptr;
      /* done? */
      if (!*ptr) break;
      /* comment */
      else if (*ptr == ';') {
        while (*ptr && *ptr != '\n') ++ptr;
      }
      /* section */
      else if (*ptr == '[') {
        ++ptr;
        start = ptr;
        while (*ptr && *ptr != ']' && *ptr != '\n') ++ptr;
        if (*ptr == ']') {
          s = yk__ini_section_add(yk__ini, start, (int) (ptr - start));
          ++ptr;
        }
      }
      /* property */
      else {
        start = ptr;
        while (*ptr && *ptr != '=' && *ptr != '\n') ++ptr;
        if (*ptr == '=') {
          l = (int) (ptr - start);
          ++ptr;
          while (*ptr && *ptr <= ' ' && *ptr != '\n') ptr++;
          start2 = ptr;
          while (*ptr && *ptr != '\n') ++ptr;
          while (*(--ptr) <= ' ') (void) ptr;
          ptr++;
          yk__ini_property_add(yk__ini, s, start, l, start2,
                               (int) (ptr - start2));
        }
      }
    }
  }
  return yk__ini;
}
int yk__ini_save(yk__ini_t const *yk__ini, char *data, int size) {
  int s;
  int p;
  int i;
  int l;
  char *n;
  int pos;
  if (yk__ini) {
    pos = 0;
    for (s = 0; s < yk__ini->section_count; ++s) {
      n = yk__ini->sections[s].name_large ? yk__ini->sections[s].name_large
                                          : yk__ini->sections[s].name;
      l = (int) YK__INI_STRLEN(n);
      if (l > 0) {
        if (data && pos < size) data[pos] = '[';
        ++pos;
        for (i = 0; i < l; ++i) {
          if (data && pos < size) data[pos] = n[i];
          ++pos;
        }
        if (data && pos < size) data[pos] = ']';
        ++pos;
        if (data && pos < size) data[pos] = '\n';
        ++pos;
      }
      for (p = 0; p < yk__ini->property_count; ++p) {
        if (yk__ini->properties[p].section == s) {
          n = yk__ini->properties[p].name_large
                  ? yk__ini->properties[p].name_large
                  : yk__ini->properties[p].name;
          l = (int) YK__INI_STRLEN(n);
          for (i = 0; i < l; ++i) {
            if (data && pos < size) data[pos] = n[i];
            ++pos;
          }
          if (data && pos < size) data[pos] = '=';
          ++pos;
          n = yk__ini->properties[p].value_large
                  ? yk__ini->properties[p].value_large
                  : yk__ini->properties[p].value;
          l = (int) YK__INI_STRLEN(n);
          for (i = 0; i < l; ++i) {
            if (data && pos < size) data[pos] = n[i];
            ++pos;
          }
          if (data && pos < size) data[pos] = '\n';
          ++pos;
        }
      }
      if (pos > 0) {
        if (data && pos < size) data[pos] = '\n';
        ++pos;
      }
    }
    if (data && pos < size) data[pos] = '\0';
    ++pos;
    return pos;
  }
  return 0;
}
void yk__ini_destroy(yk__ini_t *yk__ini) {
  int i;
  if (yk__ini) {
    for (i = 0; i < yk__ini->property_count; ++i) {
      if (yk__ini->properties[i].value_large)
        YK__INI_FREE(yk__ini->memctx, yk__ini->properties[i].value_large);
      if (yk__ini->properties[i].name_large)
        YK__INI_FREE(yk__ini->memctx, yk__ini->properties[i].name_large);
    }
    for (i = 0; i < yk__ini->section_count; ++i)
      if (yk__ini->sections[i].name_large)
        YK__INI_FREE(yk__ini->memctx, yk__ini->sections[i].name_large);
    YK__INI_FREE(yk__ini->memctx, yk__ini->properties);
    YK__INI_FREE(yk__ini->memctx, yk__ini->sections);
    YK__INI_FREE(yk__ini->memctx, yk__ini);
  }
}
int yk__ini_section_count(yk__ini_t const *yk__ini) {
  if (yk__ini) return yk__ini->section_count;
  return 0;
}
char const *yk__ini_section_name(yk__ini_t const *yk__ini, int section) {
  if (yk__ini && section >= 0 && section < yk__ini->section_count)
    return yk__ini->sections[section].name_large
               ? yk__ini->sections[section].name_large
               : yk__ini->sections[section].name;
  return NULL;
}
int yk__ini_property_count(yk__ini_t const *yk__ini, int section) {
  int i;
  int count;
  if (yk__ini) {
    count = 0;
    for (i = 0; i < yk__ini->property_count; ++i) {
      if (yk__ini->properties[i].section == section) ++count;
    }
    return count;
  }
  return 0;
}
char const *yk__ini_property_name(yk__ini_t const *yk__ini, int section,
                                  int property) {
  int p;
  if (yk__ini && section >= 0 && section < yk__ini->section_count) {
    p = yk__ini_internal_property_index(yk__ini, section, property);
    if (p != YK__INI_NOT_FOUND)
      return yk__ini->properties[p].name_large
                 ? yk__ini->properties[p].name_large
                 : yk__ini->properties[p].name;
  }
  return NULL;
}
char const *yk__ini_property_value(yk__ini_t const *yk__ini, int section,
                                   int property) {
  int p;
  if (yk__ini && section >= 0 && section < yk__ini->section_count) {
    p = yk__ini_internal_property_index(yk__ini, section, property);
    if (p != YK__INI_NOT_FOUND)
      return yk__ini->properties[p].value_large
                 ? yk__ini->properties[p].value_large
                 : yk__ini->properties[p].value;
  }
  return NULL;
}
int yk__ini_find_section(yk__ini_t const *yk__ini, char const *name,
                         int name_length) {
  int i;
  if (yk__ini && name) {
    if (name_length <= 0) name_length = (int) YK__INI_STRLEN(name);
    for (i = 0; i < yk__ini->section_count; ++i) {
      char const *const other = yk__ini->sections[i].name_large
                                    ? yk__ini->sections[i].name_large
                                    : yk__ini->sections[i].name;
      if (YK__INI_STRNICMP(name, other, name_length) == 0) return i;
    }
  }
  return YK__INI_NOT_FOUND;
}
int yk__ini_find_property(yk__ini_t const *yk__ini, int section,
                          char const *name, int name_length) {
  int i;
  int c;
  if (yk__ini && name && section >= 0 && section < yk__ini->section_count) {
    if (name_length <= 0) name_length = (int) YK__INI_STRLEN(name);
    c = 0;
    for (i = 0; i < yk__ini->property_capacity; ++i) {
      if (yk__ini->properties[i].section == section) {
        char const *const other = yk__ini->properties[i].name_large
                                      ? yk__ini->properties[i].name_large
                                      : yk__ini->properties[i].name;
        if (YK__INI_STRNICMP(name, other, name_length) == 0) return c;
        ++c;
      }
    }
  }
  return YK__INI_NOT_FOUND;
}
int yk__ini_section_add(yk__ini_t *yk__ini, char const *name, int length) {
  struct yk__ini_internal_section_t *new_sections;
  if (yk__ini && name) {
    if (length <= 0) length = (int) YK__INI_STRLEN(name);
    if (yk__ini->section_count >= yk__ini->section_capacity) {
      yk__ini->section_capacity *= 2;
      new_sections = (struct yk__ini_internal_section_t *) YK__INI_MALLOC(
          yk__ini->memctx,
          yk__ini->section_capacity * sizeof(yk__ini->sections[0]));
      YK__INI_MEMCPY(new_sections, yk__ini->sections,
                     yk__ini->section_count * sizeof(yk__ini->sections[0]));
      YK__INI_FREE(yk__ini->memctx, yk__ini->sections);
      yk__ini->sections = new_sections;
    }
    yk__ini->sections[yk__ini->section_count].name_large = 0;
    if (length + 1 >= sizeof(yk__ini->sections[0].name)) {
      yk__ini->sections[yk__ini->section_count].name_large =
          (char *) YK__INI_MALLOC(yk__ini->memctx, (size_t) length + 1);
      YK__INI_MEMCPY(yk__ini->sections[yk__ini->section_count].name_large, name,
                     (size_t) length);
      yk__ini->sections[yk__ini->section_count].name_large[length] = '\0';
    } else {
      YK__INI_MEMCPY(yk__ini->sections[yk__ini->section_count].name, name,
                     (size_t) length);
      yk__ini->sections[yk__ini->section_count].name[length] = '\0';
    }
    return yk__ini->section_count++;
  }
  return YK__INI_NOT_FOUND;
}
void yk__ini_property_add(yk__ini_t *yk__ini, int section, char const *name,
                          int name_length, char const *value,
                          int value_length) {
  struct yk__ini_internal_property_t *new_properties;
  if (yk__ini && name && section >= 0 && section < yk__ini->section_count) {
    if (name_length <= 0) name_length = (int) YK__INI_STRLEN(name);
    if (value_length <= 0) value_length = (int) YK__INI_STRLEN(value);
    if (yk__ini->property_count >= yk__ini->property_capacity) {
      yk__ini->property_capacity *= 2;
      new_properties = (struct yk__ini_internal_property_t *) YK__INI_MALLOC(
          yk__ini->memctx,
          yk__ini->property_capacity * sizeof(yk__ini->properties[0]));
      YK__INI_MEMCPY(new_properties, yk__ini->properties,
                     yk__ini->property_count * sizeof(yk__ini->properties[0]));
      YK__INI_FREE(yk__ini->memctx, yk__ini->properties);
      yk__ini->properties = new_properties;
    }
    yk__ini->properties[yk__ini->property_count].section = section;
    yk__ini->properties[yk__ini->property_count].name_large = 0;
    yk__ini->properties[yk__ini->property_count].value_large = 0;
    if (name_length + 1 >= sizeof(yk__ini->properties[0].name)) {
      yk__ini->properties[yk__ini->property_count].name_large =
          (char *) YK__INI_MALLOC(yk__ini->memctx, (size_t) name_length + 1);
      YK__INI_MEMCPY(yk__ini->properties[yk__ini->property_count].name_large,
                     name, (size_t) name_length);
      yk__ini->properties[yk__ini->property_count].name_large[name_length] =
          '\0';
    } else {
      YK__INI_MEMCPY(yk__ini->properties[yk__ini->property_count].name, name,
                     (size_t) name_length);
      yk__ini->properties[yk__ini->property_count].name[name_length] = '\0';
    }
    if (value_length + 1 >= sizeof(yk__ini->properties[0].value)) {
      yk__ini->properties[yk__ini->property_count].value_large =
          (char *) YK__INI_MALLOC(yk__ini->memctx, (size_t) value_length + 1);
      YK__INI_MEMCPY(yk__ini->properties[yk__ini->property_count].value_large,
                     value, (size_t) value_length);
      yk__ini->properties[yk__ini->property_count].value_large[value_length] =
          '\0';
    } else {
      YK__INI_MEMCPY(yk__ini->properties[yk__ini->property_count].value, value,
                     (size_t) value_length);
      yk__ini->properties[yk__ini->property_count].value[value_length] = '\0';
    }
    ++yk__ini->property_count;
  }
}
void yk__ini_section_remove(yk__ini_t *yk__ini, int section) {
  int p;
  if (yk__ini && section >= 0 && section < yk__ini->section_count) {
    if (yk__ini->sections[section].name_large)
      YK__INI_FREE(yk__ini->memctx, yk__ini->sections[section].name_large);
    for (p = yk__ini->property_count - 1; p >= 0; --p) {
      if (yk__ini->properties[p].section == section) {
        if (yk__ini->properties[p].value_large)
          YK__INI_FREE(yk__ini->memctx, yk__ini->properties[p].value_large);
        if (yk__ini->properties[p].name_large)
          YK__INI_FREE(yk__ini->memctx, yk__ini->properties[p].name_large);
        yk__ini->properties[p] = yk__ini->properties[--yk__ini->property_count];
      }
    }
    yk__ini->sections[section] = yk__ini->sections[--yk__ini->section_count];
    for (p = 0; p < yk__ini->property_count; ++p) {
      if (yk__ini->properties[p].section == yk__ini->section_count)
        yk__ini->properties[p].section = section;
    }
  }
}
void yk__ini_property_remove(yk__ini_t *yk__ini, int section, int property) {
  int p;
  if (yk__ini && section >= 0 && section < yk__ini->section_count) {
    p = yk__ini_internal_property_index(yk__ini, section, property);
    if (p != YK__INI_NOT_FOUND) {
      if (yk__ini->properties[p].value_large)
        YK__INI_FREE(yk__ini->memctx, yk__ini->properties[p].value_large);
      if (yk__ini->properties[p].name_large)
        YK__INI_FREE(yk__ini->memctx, yk__ini->properties[p].name_large);
      yk__ini->properties[p] = yk__ini->properties[--yk__ini->property_count];
      return;
    }
  }
}
void yk__ini_section_name_set(yk__ini_t *yk__ini, int section, char const *name,
                              int length) {
  if (yk__ini && name && section >= 0 && section < yk__ini->section_count) {
    if (length <= 0) length = (int) YK__INI_STRLEN(name);
    if (yk__ini->sections[section].name_large)
      YK__INI_FREE(yk__ini->memctx, yk__ini->sections[section].name_large);
    yk__ini->sections[section].name_large = 0;
    if (length + 1 >= sizeof(yk__ini->sections[0].name)) {
      yk__ini->sections[section].name_large =
          (char *) YK__INI_MALLOC(yk__ini->memctx, (size_t) length + 1);
      YK__INI_MEMCPY(yk__ini->sections[section].name_large, name,
                     (size_t) length);
      yk__ini->sections[section].name_large[length] = '\0';
    } else {
      YK__INI_MEMCPY(yk__ini->sections[section].name, name, (size_t) length);
      yk__ini->sections[section].name[length] = '\0';
    }
  }
}
void yk__ini_property_name_set(yk__ini_t *yk__ini, int section, int property,
                               char const *name, int length) {
  int p;
  if (yk__ini && name && section >= 0 && section < yk__ini->section_count) {
    if (length <= 0) length = (int) YK__INI_STRLEN(name);
    p = yk__ini_internal_property_index(yk__ini, section, property);
    if (p != YK__INI_NOT_FOUND) {
      if (yk__ini->properties[p].name_large)
        YK__INI_FREE(yk__ini->memctx, yk__ini->properties[p].name_large);
      yk__ini->properties[yk__ini->property_count].name_large = 0;
      if (length + 1 >= sizeof(yk__ini->properties[0].name)) {
        yk__ini->properties[p].name_large =
            (char *) YK__INI_MALLOC(yk__ini->memctx, (size_t) length + 1);
        YK__INI_MEMCPY(yk__ini->properties[p].name_large, name,
                       (size_t) length);
        yk__ini->properties[p].name_large[length] = '\0';
      } else {
        YK__INI_MEMCPY(yk__ini->properties[p].name, name, (size_t) length);
        yk__ini->properties[p].name[length] = '\0';
      }
    }
  }
}
void yk__ini_property_value_set(yk__ini_t *yk__ini, int section, int property,
                                char const *value, int length) {
  int p;
  if (yk__ini && value && section >= 0 && section < yk__ini->section_count) {
    if (length <= 0) length = (int) YK__INI_STRLEN(value);
    p = yk__ini_internal_property_index(yk__ini, section, property);
    if (p != YK__INI_NOT_FOUND) {
      if (yk__ini->properties[p].value_large)
        YK__INI_FREE(yk__ini->memctx, yk__ini->properties[p].value_large);
      yk__ini->properties[yk__ini->property_count].value_large = 0;
      if (length + 1 >= sizeof(yk__ini->properties[0].value)) {
        yk__ini->properties[p].value_large =
            (char *) YK__INI_MALLOC(yk__ini->memctx, (size_t) length + 1);
        YK__INI_MEMCPY(yk__ini->properties[p].value_large, value,
                       (size_t) length);
        yk__ini->properties[p].value_large[length] = '\0';
      } else {
        YK__INI_MEMCPY(yk__ini->properties[p].value, value, (size_t) length);
        yk__ini->properties[p].value[length] = '\0';
      }
    }
  }
}
#endif /* YK__INI_IMPLEMENTATION */
/*

contributors:
    Randy Gaul (copy-paste bug in yk__ini_property_value_set)
    Branimir Karadzic (YK__INI_STRNICMP bugfix)

revision history:
    1.2     using strnicmp for correct length compares, fixed copy-paste bug in yk__ini_property_value_set
    1.1     customization, added documentation, cleanup
    1.0     first publicly released version

*/
/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2015 Mattias Gustavsson

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

------------------------------------------------------------------------------

ALTERNATIVE B - Public Domain (www.unlicense.org)

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this 
software, either in source code form or as a compiled binary, for any purpose, 
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this 
software dedicate any and all copyright interest in the software to the public 
domain. We make this dedication for the benefit of the public at large and to 
the detriment of our heirs and successors. We intend this dedication to be an 
overt act of relinquishment in perpetuity of all present and future rights to 
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

------------------------------------------------------------------------------
*/
