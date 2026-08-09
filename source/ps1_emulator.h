#ifndef PS1_EMULATOR_H
#define PS1_EMULATOR_H

#include <3ds.h>

// Declarações das funções do seu motor
void ps1_init(void);
void ps1_load_iso(const char* path);
void ps1_run_frame(void);
void ps1_get_framebuffer(u8* 3ds_top_fb);
void draw_bottom_map(void);
void ps1_shutdown(void);

#endif
