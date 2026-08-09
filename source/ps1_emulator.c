#include "ps1_emulator.h"
#include <string.h>

// Estrutura interna fictícia para simular o que o controle do PS1 está recebendo
typedef struct {
    bool up, down, left, right;
    bool button_cross; // Pular
    bool button_square; // Soco do Rayman
} PS1_Controller;

PS1_Controller rayman_input;

void ps1_init(void) {
    memset(&rayman_input, 0, sizeof(PS1_Controller));
}

void ps1_run_frame(void) {
    // 1. Captura quais botões digitais estão pressionados no momento
    u32 kHeld = hidKeysHeld();
    
    // 2. Captura a posição exata do Analógico (Circle Pad)
    circlePosition cStick;
    hidCircleRead(&cStick);

    // 3. Reseta os estados de movimento antes de checar as novas entradas
    rayman_input.left  = false;
    rayman_input.right = false;
    rayman_input.up    = false;
    rayman_input.down  = false;

    // 4. Mapeia o Movimento para a Esquerda (D-Pad Esquerdo OU Analógico empurrado para a esquerda)
    if ((kHeld & KEY_DLEFT) || (cStick.dx < -40)) {
        rayman_input.left = true;
    }
    // Mapeia o Movimento para a Direita (D-Pad Direito OU Analógico empurrado para a direita)
    if ((kHeld & KEY_DRIGHT) || (cStick.dx > 40)) {
        rayman_input.right = true;
    }
    // Mapeia para Cima (Olhar para cima / Subir cordas)
    if ((kHeld & KEY_DUP) || (cStick.dy > 40)) {
        rayman_input.up = true;
    }
    // Mapeia para Baixo (Agachar)
    if ((kHeld & KEY_DDOWN) || (cStick.dy < -40)) {
        rayman_input.down = true;
    }

    // 5. Mapeia os Botões de Ação (A do 3DS = X do PS1 para Pular | Y do 3DS = Quadrado do PS1 para Atirar Soco)
    rayman_input.button_cross  = (kHeld & KEY_A);
    rayman_input.button_square = (kHeld & KEY_Y);

    // [AQUI ENTRA A LÓGICA DO SEU MOTOR]
    // Use as variáveis rayman_input.left, rayman_input.button_cross, etc., para mover o boneco.
}

// (Mantenha as funções ps1_load_iso, ps1_get_framebuffer, etc. iguais às anteriores)
