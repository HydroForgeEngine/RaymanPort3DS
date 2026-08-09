#include <3ds.h>
#include <fat.h>
#include <stdio.h>
#include "ps1_emulator.h"

int main(int argc, char* argv[])
{
    // 1. Inicializa o hardware de vídeo e gráfico do 3DS
    gfxInitDefault();

    // 2. Inicializa o sistema de arquivos do Cartão SD (necessário para ler o Rayman.iso)
    bool fat_ok = fatInitDefault();

    // 3. Detecção de New 3DS para ativação de Overclock (804MHz + cache L2)
    bool isNew3DS = false;
    APT_CheckNew3DS(&isNew3DS);
    if (isNew3DS) {
        osSetSpeedupEnable(true); 
    }

    // 4. Inicializa os subsistemas do emulador e carrega a ISO do Rayman
    ps1_init();

    if (fat_ok) {
        // Tenta carregar a ISO a partir da raiz do cartão SD
        ps1_load_iso("/Rayman.iso");
    }

    // 5. Loop principal de execução do console
    while (aptMainLoop())
    {
        // Atualiza e lê os botões e analógico no frame atual
        hidScanInput();
        u32 kDown = hidKeysDown();

        // Se o usuário pressionar START, encerra o loop com segurança
        if (kDown & KEY_START) {
            break;
        }

        // Processa a lógica de jogo e entradas do controle do PS1
        ps1_run_frame();

        // Obtém o ponteiro de memória do framebuffer da tela superior (Top Screen)
        u16 width, height;
        u8* top_screen = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &width, &height);

        // Copia e rotaciona o buffer do PS1 para a tela superior do 3DS
        ps1_get_framebuffer(top_screen);

        // Desenha a interface ou elementos extras na tela inferior (Bottom Screen)
        draw_bottom_map();

        // Envia os buffers para a GPU do 3DS e aguarda o sincronismo vertical (VBlank)
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    // 6. Encerramento limpo ao fechar o aplicativo
    ps1_shutdown();
    gfxExit();

    return 0;
}
