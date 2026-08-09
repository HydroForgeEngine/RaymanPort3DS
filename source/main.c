#include <3ds.h>
#include "ps1_emulator.h" // Inclui o seu novo módulo de emulação

int main(void)
{
    // Inicializa o hardware de vídeo padrão do 3DS
    gfxInitDefault();
    
    // Detecção dinâmica para overclock automático caso rode em um New 3DS
    bool isNew3DS = false;
    APT_CheckNew3DS(&isNew3DS);
    if (isNew3DS) {
        osSetSpeedupEnable(true); 
    }

    // Inicializa o subsistema do Rayman
    ps1_init();
    ps1_load_iso("/Rayman.iso");

    // Loop de renderização e lógica do jogo
    while (aptMainLoop())
    {
        hidScanInput();
        u32 kDown = hidKeysDown();

        // Se o usuário apertar START no Old ou New 3DS, fecha o jogo com segurança
        if (kDown & KEY_START) {
            break;
        }

        // Calcula o próximo passo do jogo
        ps1_run_frame();

        // Captura o endereço real de vídeo da tela de cima (Top Screen)
        u16 width, height;
        u8* top_screen = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &width, &height);

        // Processa o upscale/centralização e desenha os pixels do PS1 no 3DS
        ps1_get_framebuffer(top_screen);

        // Executa desenhos na tela secundária
        draw_bottom_map();

        // Empurra os buffers para a tela e aguarda o VBlank (Garante estabilidade e evita screen tearing)
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    // Finaliza tudo de forma limpa antes de retornar ao menu do console
    ps1_shutdown();
    gfxExit();

    return 0;
}
