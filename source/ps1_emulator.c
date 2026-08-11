#include <3ds.h>
#include "ps1_emulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inclui os cabeçalhos do núcleo do PCSX-ReARMed
#include "pcsx/libpcsx.h"

// Buffer de vídeo gerado pelo PCSX (320x240 RGB565)
static u16* pcsx_gpu_buffer = NULL;

void ps1_init(void) {
    // 1. Inicializa o núcleo do PCSX-ReARMed
    pcsx_core_init();
    
    // 2. Configura a GPU do PCSX para renderizar em um buffer interno 320x240
    pcsx_gpu_buffer = (u16*)malloc(320 * 240 * sizeof(u16));
    pcsx_set_video_buffer(pcsx_gpu_buffer);
}

void ps1_load_iso(const char* path) {
    char sd_path[256];
    snprintf(sd_path, sizeof(sd_path), "sdmc:%s", path);

    // Carrega a ISO no leitor de CD interno do PCSX
    if (pcsx_load_cd(sd_path) != 0) {
        // Fallback caso o caminho sem sdmc: seja necessário
        pcsx_load_cd(path);
    }
}

void ps1_run_frame(void) {
    // 1. Lê as entradas do 3DS (D-Pad, Analógico e Botões)
    u32 kHeld = hidKeysHeld();
    circlePosition cStick;
    hidCircleRead(&cStick);

    // 2. Converte os botões do 3DS para o formato de pad do PCSX (Pad do PS1)
    u32 ps1_pad_state = 0;
    if ((kHeld & KEY_DLEFT)  || (cStick.dx < -40)) ps1_pad_state |= PS1_PAD_LEFT;
    if ((kHeld & KEY_DRIGHT) || (cStick.dx > 40))  ps1_pad_state |= PS1_PAD_RIGHT;
    if ((kHeld & KEY_DUP)    || (cStick.dy > 40))  ps1_pad_state |= PS1_PAD_UP;
    if ((kHeld & KEY_DDOWN)  || (cStick.dy < -40)) ps1_pad_state |= PS1_PAD_DOWN;
    if (kHeld & KEY_A) ps1_pad_state |= PS1_PAD_CROSS;  // Pular
    if (kHeld & KEY_Y) ps1_pad_state |= PS1_PAD_SQUARE; // Soco

    pcsx_set_pad_state(0, ps1_pad_state);

    // 3. EXECUTA EXATAMENTE 1 FRAME DO JOGO NO PCSX (Executa CPU MIPS, GPU e Som)
    pcsx_run_frame();
}

void ps1_get_framebuffer(u8* top_screen_fb) {
    if (!top_screen_fb || !pcsx_gpu_buffer) return;

    // Pega na imagem gerada pelo PCSX (320x240) e desenha no ecrã do 3DS
    const int offset_x = 40; // Centraliza na tela de 400px

    for (int y = 0; y < 240; y++) {
        for (int x = 0; x < 320; x++) {
            u16 pixel = pcsx_gpu_buffer[y * 320 + x];

            // Conversão de cor RGB565 para BGR888 do 3DS
            u8 r = ((pixel >> 11) & 0x1F) * 255 / 31;
            u8 g = ((pixel >> 5)  & 0x3F) * 255 / 63;
            u8 b = ((pixel)       & 0x1F) * 255 / 31;

            // Rotação de 90° para a RAM do 3DS
            int dst_idx = ((x + offset_x) * 240 + (239 - y)) * 3;

            top_screen_fb[dst_idx + 0] = b;
            top_screen_fb[dst_idx + 1] = g;
            top_screen_fb[dst_idx + 2] = r;
        }
    }
}

void draw_bottom_map(void) {
    u16 width, height;
    u8* bottom_fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, &width, &height);
    if (bottom_fb) {
        memset(bottom_fb, 0, width * height * 3);
    }
}

void ps1_shutdown(void) {
    pcsx_core_shutdown();
    if (pcsx_gpu_buffer) {
        free(pcsx_gpu_buffer);
        pcsx_gpu_buffer = NULL;
    }
}
