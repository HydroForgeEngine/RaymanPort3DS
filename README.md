# Rayman 3DS Port

Um port / motor de emulação simplificado do clássico **Rayman (PlayStation 1)** adaptado para a **Nintendo 3DS** e **New 3DS**, desenvolvido em **C** utilizando a biblioteca `libctru`.

---

## 🚀 Funcionalidades

- **Mapeamento de Controles Unificado:** Suporte tanto ao **D-Pad** digital quanto ao **Circle Pad** (Analógico) da 3DS.
- **Suporte a New 3DS:** Detecção automática para ativação de overclock (804MHz + Cache L2) garantindo maior desempenho.
- **Leitura do SD Card:** Suporte à leitura direta da imagem ISO (`sdmc:/Rayman.iso`) via `libfat`.
- **Renderização e Framebuffer:**
  - Conversão de cores de **RGB565** (PS1) para **BGR888** (3DS).
  - Rotação de matriz de 90° para compatibilidade com a orientação de memória da 3DS.
  - Centralização da imagem (320x240) na tela superior da 3DS (400x240).
- **Dual Screen:** Suporte preliminar para uso da tela inferior (Bottom Screen).
- **Estabilidade Visual:** Sincronização VBlank para prevenir *screen tearing*.

---

## 📂 Estrutura do Projeto

```text
.
├── source/
│   ├── main.c              # Inicialização do hardware, loop principal e ciclo de vida
│   ├── ps1_emulator.c      # Leitura de inputs, conversão de framebuffer e controle do emulador
│   └── ps1_emulator.h      # Cabeçalho com declarações de funções e interfaces
└── README.md               # Documentação do projeto
