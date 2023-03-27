#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

enum pos { START, CENTER, END };

struct config {

  uint32_t font_size;
  uint32_t char_width;
  uint32_t char_height;
  enum pos horizontal_pos;
  enum pos vertical_pos;

};

//void config_apply(struct config *restricted conf, struct bread bread);


#endif /* CONFIG_H */
