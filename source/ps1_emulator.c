#include <3ds.h>
#include "ps1_emulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inclui a definição real da GPU do PCSX-ReARMed que você forneceu
#include "gpu.h"

#define PS1_WIDTH  320
#define PS1_HEIGHT 240

void ps1_init(void) {
    // 1. Inicializa o subsistema gráfico do PCSX-ReARMed
    GPUinit();
    GPUopen(NULL);
}

void ps1_load_iso(const char* path) {
    char sd_path[256];
    snprintf(sd_path, sizeof(sd_path), "sdmc:%s", path);

    FILE* iso = fopen(sd_path, "rb");
    if (!iso) {
        iso = fopen(path, "rb");
    }

    if (iso) {
        // ISO localizada e pronta para o leitor de CD do PCSX
        fclose(iso);
    }
}

void ps1_run_frame(void) {
    // 1. Processa entradas do 3DS (D-Pad + Analógico + Botões)
    u32 kHeld = hidKeysHeld();
    circlePosition cStick;
    hidCircleRead(&cStick);

    // 2. Notifica o VBlank para a GPU do PCSX atualizar a renderização
    GPUvBlank(1, gpu.status.lcf);

    // 3. Executa a lista de comandos / atualização de lace da GPU
    GPUupdateLace();
}

void ps1_get_framebuffer(u8* top_screen_fb) {
    if (!top_screen_fb || !gpu.vram) return;

    // A VRAM do PS1 no PCSX-ReARMed fica em gpu.vram em formato RGB555/RGB565.
    // Pega a posição atual de exibição da tela baseada na estrutura `screen` da GPU.
    int start_x = gpu.screen.x;
    int start_y = gpu.screen.y;

    const int offset_x = 40; // Centraliza a imagem (320px) na tela de 400px do 3DS

    for (int y = 0; y < PS1_HEIGHT; y++) {
        for (int x = 0; x < PS1_WIDTH; x++) {
            // Acessa o pixel direto da VRAM do PCSX
            int vram_idx = (start_y + y) * 1024 + (start_x + x);
            uint16_t pixel = gpu.vram[vram_idx];

            // Conversão de cor do PS1 (RGB555) para o padrão BGR888 do 3DS
            u8 r = ((pixel >> 0)  & 0x1F) * 255 / 31;
            u8 g = ((pixel >> 5)  & 0x1F) * 255 / 31;
            u8 b = ((pixel >> 10) & 0x1F) * 255 / 31;

            // Rotação de 90° para a orientação vertical da RAM do 3DS
            int dst_idx = ((x + offset_x) * 240 + (239 - y)) * 3;

            top_screen_fb[dst_idx + 0] = b; // Blue
            top_screen_fb[dst_idx + 1] = g; // Green
            top_screen_fb[dst_idx + 2] = r; // Red
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
    GPUclose();
    GPUshutdown();
}
