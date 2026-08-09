#include "ps1_emulator.h"
#include <string.h>

// Buffer interno simulando a memória de vídeo gerada pelo emulador de PS1 (320x240 em RGB)
// Nota: Em um port real, esse buffer vem direto do plugin gráfico (como o Unai)
u8 ps1_virtual_fb[320 * 240 * 3]; 

void ps1_init(void) {
    // Zera o buffer virtual inicial do PS1
    memset(ps1_virtual_fb, 0, sizeof(ps1_virtual_fb));
}

void ps1_load_iso(const char* path) {
    // Lógica para abrir o arquivo Rayman.iso via fatfs do 3DS
}

void ps1_run_frame(void) {
    // Executa a CPU e atualiza o buffer 'ps1_virtual_fb' com o frame atual do Rayman
}

// ALGORÍTMO DE REDIMENSIONAMENTO E COPIA DE BUFFER (320x240 PS1 -> 400x240 3DS)
void ps1_get_framebuffer(u8* 3ds_top_fb) {
    if (!3ds_top_fb) return;

    // Limpa a tela do 3DS com preto para apagar frames anteriores (evita ghosting nas bordas)
    // 400 * 240 * 3 bytes por pixel = 288000 bytes
    memset(3ds_top_fb, 0, 400 * 240 * 3);

    // O PS1 gera 320x240. O 3DS tem 400x240. 
    // Para centralizar (4:3), pulamos os primeiros 40 pixels da lateral.
    int offset_x = 40; 

    for (int y = 0; y < 240; y++) {
        for (int x = 0; x < 320; x++) {
            
            // 1. Encontra a posição do pixel no buffer de origem do PS1
            int ps1_index = (y * 320 + x) * 3;
            u8 r = ps1_virtual_fb[ps1_index];
            u8 g = ps1_virtual_fb[ps1_index + 1];
            u8 b = ps1_virtual_fb[ps1_index + 2];

            // 2. Mapeamento de rotação de tela do hardware do 3DS (Crucial para homebrew)
            // A memória de vídeo do 3DS organiza os pixels de cima para baixo, da esquerda para a direita.
            int screen_x = x + offset_x;
            int screen_y = 239 - y; // Inverte o eixo Y pois a tela do 3DS lê de baixo para cima fisicamente

            int _3ds_index = (screen_x * 240 + screen_y) * 3;

            // 3. Injeta o pixel convertendo para a ordem BGR888 do 3DS
            3ds_top_fb[_3ds_index]     = b; // Azul
            3ds_top_fb[_3ds_index + 1] = g; // Verde
            3ds_top_fb[_3ds_index + 2] = r; // Vermelho
        }
    }
}

void draw_bottom_map(void) {
    // Usando o console de debug nativo ou citro2d para desenhar informações na tela inferior
}

void ps1_shutdown(void) {
    // Libera ponteiros ou arquivos abertos
}
