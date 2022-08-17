#define YK__SDS_IMPLEMENTATION
#define YK__BHALIB_IMPLEMENTATION
#define YK__STB_DS_IMPLEMENTATION
#include "yk__lib.h"
// --forward declaration section--
int32_t yy__main();
// --body section--
int32_t yy__main()
{
  int32_t* yy__a = NULL;
  yk__arrput(yy__a, 1);
  yk__arrput(yy__a, 2);
  yk__arrput(yy__a, 3);
  printf("%jd", ((intmax_t)yy__a[0]));
  yy__a[(0 + 0)] = 0;
  printf("%jd", ((intmax_t)yy__a[0]));
  yk__sds t__0 = yk__sdsnew("\n");
  printf("%s", (t__0));
  yk__arrfree(yy__a);
  yk__sdsfree(t__0);



  // STB + SDS string hash test
  struct {
    yk__sds key;
    int value;
  } *my_map_object = NULL, s;

  s.key = yk__sdsnew("a");
  s.value = 1;
  yk__sh_new_strdup(my_map_object);
  yk__shputs(my_map_object, s);
  yk__shput(my_map_object, yk__sdsnew("b"), 300);
  // Check that we support binary strings all magic like haha
  yk__shput(my_map_object, yk__sdsnewlen("l\0\0t", 4), 100000);
  assert (*my_map_object[0].key == 'a');
  assert (my_map_object[0].key != s.key);
  assert (my_map_object[0].value == s.value);
  printf("my_map_object[0].key ==> %s\n", my_map_object[0].key);
  printf("get('b') ==> %d\n", yk__shget(my_map_object, yk__sdsnew("b")));
  printf("get('l\\0\\0t') ==> %d\n", yk__shget(my_map_object, yk__sdsnewlen("l\0\0t", 4)));
  yk__shfree(my_map_object);


  return 0;
}
int main(void) { return yy__main(); }