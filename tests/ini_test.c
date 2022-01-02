#define YK__INI_IMPLEMENTATION
#include "yk__ini.h"
#include <stdio.h>
#include <stdlib.h>
int main() {
  yk__ini_t *yk__ini = yk__ini_create(NULL);
  yk__ini_property_add(yk__ini, YK__INI_GLOBAL_SECTION, "FirstSetting", 12,
                       "Test", 4);
  yk__ini_property_add(yk__ini, YK__INI_GLOBAL_SECTION, "SecondSetting", 13,
                       "2", 1);
  int section = yk__ini_section_add(yk__ini, "MySection", 9);
  yk__ini_property_add(yk__ini, section, "ThirdSetting", 12, "Three", 5);
  int size = yk__ini_save(yk__ini, NULL, 0);// Find the size needed
  char *data = (char *) malloc(size);
  size = yk__ini_save(yk__ini, data, size);// Actually save the file
  yk__ini_destroy(yk__ini);
  FILE *fp = fopen("test.ini", "w");
  fwrite(data, 1, size - 1, fp);
  fclose(fp);
  free(data);
  return 0;
}