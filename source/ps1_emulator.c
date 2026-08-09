#include <3ds.h>
#include "ps1_emulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Estruturas de Dados
// -----------------------------------------------------------------------------

// Estrutura interna para mapear os controles do PS1
typedef struct {
    bool up, down, left, right;
    bool button_cross;  // Pular (Mapeado para A do 3DS)
    bool button_square; // Soco (Mapeado para Y do 3DS)
} PS1_Controller;

// -----------------------------------------------------------------------------
// Variáveis Globais / Estáticas do Módulo
// -----------------------------------------------------------------------------

static PS1_Controller rayman_input;
static FILE* iso_file = NULL;

// Buffer de renderização interno (Resolução clássica de PS1: 320x240 em RGB565)
#define PS1_WIDTH  320
#define PS1_HEIGHT 240
static u16 ps1_framebuffer[PS1_WIDTH * PS1_HEIGHT];

// -----------------------------------------------------------------------------
// Funções do Emulador
// -----------------------------------------------------------------------------

void ps1_init(void) {
    // Reseta o estado dos controles
    memset(&rayman_input, 0, sizeof(PS1_Controller));

    // Limpa o buffer de vídeo interno com a cor preta
    memset(ps1_framebuffer, 0, sizeof(ps1_framebuffer));
}

void ps1_load_iso(const char* path) {
    char sd_path[256];
    
    // Tenta montar o caminho relativo ao cartão SD do 3DS
    snprintf(sd_path, sizeof(sd_path), "sdmc:%s", path);
    iso_file = fopen(sd_path, "rb");

    // Fallback para tentar abrir o caminho direto fornecido
    if (!iso_file) {
        iso_file = fopen(path, "rb");
    }

    if (iso_file) {
        // Exemplo: Ler cabeçalho ou preparar índice de setores da ISO aqui
    }
}

void ps1_run_frame(void) {
    // 1. Captura botões digitais pressionados no momento
    u32 kHeld = hidKeysHeld();
    
    // 2. Captura a posição do Analógico (Circle Pad)
    circlePosition cStick;
    hidCircleRead(&cStick);

    // 3. Reseta os estados de movimento antes de processar os novos inputs
    rayman_input.left  = false;
    rayman_input.right = false;
    rayman_input.up    = false;
    rayman_input.down  = false;

    // 4. Mapeia movimentação (D-Pad OU Circle Pad)
    if ((kHeld & KEY_DLEFT) || (cStick.dx < -40)) {
        rayman_input.left = true;
    }
    if ((kHeld & KEY_DRIGHT) || (cStick.dx > 40)) {
        rayman_input.right = true;
    }
    if ((kHeld & KEY_DUP) || (cStick.dy > 40)) {
        rayman_input.up = true;
    }
    if ((kHeld & KEY_DDOWN) || (cStick.dy < -40)) {
        rayman_input.down = true;
    }

    // 5. Mapeia botões de ação (A do 3DS = X do PS1 | Y do 3DS = Quadrado do PS1)
    rayman_input.button_cross  = (kHeld & KEY_A);
    rayman_input.button_square = (kHeld & KEY_Y);

    // -------------------------------------------------------------------------
    // LÓGICA DO MOTOR DO JOGO / EMULADOR
    // -------------------------------------------------------------------------
    // Aqui entra a execução do frame do jogo baseando-se no rayman_input 
    // e atualizando a matriz `ps1_framebuffer`.
}

void ps1_get_framebuffer(u8* top_screen_fb) {
    if (!top_screen_fb) return;

    // A tela superior do 3DS possui resolução 400x240 na orientação vertical RAM (240x400).
    // O framebuffer do PS1 tem 320x240.
    // Calculamos uma margem horizontal de 40px de cada lado para centralizar ( (400 - 320) / 2 ).
    const int offset_x = 40; 

    for (int y = 0; y < PS1_HEIGHT; y++) {
        for (int x = 0; x < PS1_WIDTH; x++) {
            // Pega o pixel atual no formato RGB565 do PS1
            u16 src_pixel = ps1_framebuffer[y * PS1_WIDTH + x];

            // Converte RGB565 para componentes BGR888 (padrão do 3DS)
            u8 r = (src_pixel >> 11) & 0x1F;
            u8 g = (src_pixel >> 5)  & 0x3F;
            u8 b = (src_pixel)       & 0x1F;

            // Escala os valores de 5/6 bits para 8 bits (0-255)
            r = (r * 255) / 31;
            g = (g * 255) / 63;
            b = (b * 255) / 31;

            // Mapeia as coordenadas (x, y) horizontais para a orientação vertical do 3DS
            int screen_x = x + offset_x;
            int screen_y = y;

            // Índice no buffer do 3DS (formato BGR888 de 3 bytes por pixel)
            int dst_idx = (screen_x * 240 + (239 - screen_y)) * 3;

            top_screen_fb[dst_idx + 0] = b; // Blue
            top_screen_fb[dst_idx + 1] = g; // Green
            top_screen_fb[dst_idx + 2] = r; // Red
        }
    }
}

void draw_bottom_map(void) {
    u16 width, height;
    u8* bottom_fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, &width, &height);

    if (!bottom_fb) return;

    // Preenche a tela inferior (320x240) limpa com zeros (preto) ou desenha o HUD
    memset(bottom_fb, 0, width * height * 3);
}

void ps1_shutdown(void) {
    // Fecha a ISO com segurança se estiver aberta
    if (iso_file) {
        fclose(iso_file);
        iso_file = NULL;
    }
}
