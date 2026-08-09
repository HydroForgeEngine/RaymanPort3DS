/*
 * Rayman3DS - CD-ROM Reader + Dual Screen
 *
 * Nintendo 3DS
 * devkitPro + libctru
 *
 * Lê setores RAW de CD-ROM PS1:
 * 2352 bytes por setor.
 *
 * Controles:
 *   Circle Pad = mover marcador no mapa
 *   A          = próximo setor
 *   B          = setor anterior
 *   L          = -10 setores
 *   R          = +10 setores
 *   START      = sair
 *
 * ATENÇÃO:
 * Este programa NÃO executa Rayman.
 * Ele apenas lê a imagem do CD e demonstra
 * a interface de duas telas.
 */

#include <3ds.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define ROM_PATH "/Rayman_rom.psx"

#define SECTOR_SIZE 2352

#define TOP_WIDTH  400
#define TOP_HEIGHT 240

#define BOT_WIDTH  320
#define BOT_HEIGHT 240

static FILE *rom = NULL;

static uint8_t sectorBuffer[SECTOR_SIZE];

static uint64_t totalSectors = 0;
static uint64_t currentSector = 0;

static int playerX = 160;
static int playerY = 120;


/* =========================================================
   Framebuffer
   ========================================================= */

static inline void putPixel(
    u32 *fb,
    int width,
    int height,
    int x,
    int y,
    u32 color
)
{
    if (x < 0 || x >= width)
        return;

    if (y < 0 || y >= height)
        return;

    /*
     * Framebuffer do 3DS:
     * X primeiro e Y invertido.
     */
    int index = x * height + (height - 1 - y);

    fb[index] = color;
}


/* =========================================================
   Retângulo
   ========================================================= */

static void rectangle(
    u32 *fb,
    int width,
    int height,
    int x,
    int y,
    int w,
    int h,
    u32 color
)
{
    for (int yy = y; yy < y + h; yy++)
    {
        for (int xx = x; xx < x + w; xx++)
        {
            putPixel(
                fb,
                width,
                height,
                xx,
                yy,
                color
            );
        }
    }
}


/* =========================================================
   Limpar tela
   ========================================================= */

static void clearFramebuffer(
    u32 *fb,
    int width,
    int height,
    u32 color
)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            putPixel(
                fb,
                width,
                height,
                x,
                y,
                color
            );
        }
    }
}


/* =========================================================
   Abre imagem PS1
   ========================================================= */

static bool openPSXImage(void)
{
    rom = fopen(ROM_PATH, "rb");

    if (rom == NULL)
    {
        return false;
    }

    if (fseek(rom, 0, SEEK_END) != 0)
    {
        fclose(rom);
        rom = NULL;

        return false;
    }

    long fileSize = ftell(rom);

    if (fileSize <= 0)
    {
        fclose(rom);
        rom = NULL;

        return false;
    }

    rewind(rom);

    /*
     * CD-ROM RAW:
     *
     * 2352 bytes = 1 setor.
     */

    totalSectors =
        (uint64_t)fileSize / SECTOR_SIZE;

    if (totalSectors == 0)
    {
        fclose(rom);
        rom = NULL;

        return false;
    }

    return true;
}


/* =========================================================
   Fecha imagem
   ========================================================= */

static void closePSXImage(void)
{
    if (rom != NULL)
    {
        fclose(rom);
        rom = NULL;
    }
}


/* =========================================================
   Lê setor RAW de 2352 bytes
   ========================================================= */

static bool readSector(uint64_t sector)
{
    if (rom == NULL)
        return false;

    if (sector >= totalSectors)
        return false;

    uint64_t offset =
        sector * SECTOR_SIZE;

    /*
     * Posiciona no começo do setor.
     */

    if (fseek(
            rom,
            (long)offset,
            SEEK_SET
        ) != 0)
    {
        return false;
    }

    /*
     * Lê exatamente 2352 bytes.
     */

    size_t read =
        fread(
            sectorBuffer,
            1,
            SECTOR_SIZE,
            rom
        );

    if (read != SECTOR_SIZE)
    {
        return false;
    }

    currentSector = sector;

    return true;
}


/* =========================================================
   Lê setor seguinte
   ========================================================= */

static void nextSector(void)
{
    if (currentSector + 1 < totalSectors)
    {
        readSector(currentSector + 1);
    }
}


/* =========================================================
   Lê setor anterior
   ========================================================= */

static void previousSector(void)
{
    if (currentSector > 0)
    {
        readSector(currentSector - 1);
    }
}


/* =========================================================
   Avança vários setores
   ========================================================= */

static void advanceSector(uint64_t amount)
{
    uint64_t newSector =
        currentSector + amount;

    if (newSector >= totalSectors)
    {
        newSector =
            totalSectors - 1;
    }

    readSector(newSector);
}


/* =========================================================
   Volta vários setores
   ========================================================= */

static void rewindSector(uint64_t amount)
{
    if (currentSector < amount)
    {
        readSector(0);
        return;
    }

    readSector(currentSector - amount);
}


/* =========================================================
   Desenha indicador do jogador
   ========================================================= */

static void drawPlayer(u32 *fb)
{
    /*
     * Corpo
     */

    rectangle(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        playerX - 5,
        playerY - 7,
        10,
        14,
        0xFFFFFFFF
    );

    /*
     * Cabeça
     */

    rectangle(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        playerX - 4,
        playerY - 12,
        8,
        5,
        0xFFFFFFFF
    );
}


/* =========================================================
   Desenha mapa da tela inferior
   ========================================================= */

static void drawBottomMap(u32 *fb)
{
    clearFramebuffer(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        0x181818FF
    );

    /*
     * Área do mapa
     */

    rectangle(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        10,
        10,
        300,
        180,
        0x293329FF
    );

    /*
     * Caminho horizontal
     */

    rectangle(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        30,
        100,
        250,
        20,
        0x596059FF
    );

    /*
     * Caminho vertical
     */

    rectangle(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        140,
        40,
        20,
        120,
        0x596059FF
    );

    /*
     * Área da esquerda
     */

    rectangle(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        30,
        50,
        60,
        50,
        0x3F4F3FFF
    );

    /*
     * Área da direita
     */

    rectangle(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        220,
        130,
        60,
        50,
        0x3F4F3FFF
    );

    /*
     * Pequenos pontos representando
     * salas/áreas.
     */

    rectangle(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        45,
        65,
        10,
        10,
        0xFFFFFFFF
    );

    rectangle(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        245,
        145,
        10,
        10,
        0xFFFFFFFF
    );

    /*
     * Jogador
     */

    drawPlayer(fb);

    /*
     * Barra inferior
     */

    rectangle(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        0,
        200,
        320,
        40,
        0x101010FF
    );

    /*
     * Indicadores gráficos simples.
     *
     * Como não usamos uma biblioteca de fontes,
     * usamos barras para representar estado.
     */

    /*
     * Progresso do setor
     */

    if (totalSectors > 0)
    {
        int progress =
            (int)(
                ((double)currentSector /
                 (double)totalSectors) *
                300.0
            );

        if (progress < 1)
            progress = 1;

        rectangle(
            fb,
            BOT_WIDTH,
            BOT_HEIGHT,
            10,
            210,
            progress,
            5,
            0xFFFFFFFF
        );
    }

    /*
     * Indicador de leitura.
     */

    rectangle(
        fb,
        BOT_WIDTH,
        BOT_HEIGHT,
        10,
        225,
        40,
        5,
        0xFFFFFFFF
    );
}


/* =========================================================
   Tela superior
   ========================================================= */

static void drawTopScreen(u32 *fb)
{
    clearFramebuffer(
        fb,
        TOP_WIDTH,
        TOP_HEIGHT,
        0x101820FF
    );

    /*
     * Área de gameplay provisória.
     */

    rectangle(
        fb,
        TOP_WIDTH,
        TOP_HEIGHT,
        10,
        10,
        380,
        220,
        0x426642FF
    );

    /*
     * Céu/área superior.
     */

    rectangle(
        fb,
        TOP_WIDTH,
        TOP_HEIGHT,
        10,
        10,
        380,
        80,
        0x304F70FF
    );

    /*
     * Chão.
     */

    rectangle(
        fb,
        TOP_WIDTH,
        TOP_HEIGHT,
        10,
        190,
        380,
        40,
        0x60452FFF
    );

    /*
     * Plataforma.
     */

    rectangle(
        fb,
        TOP_WIDTH,
        TOP_HEIGHT,
        80,
        150,
        100,
        15,
        0x806040FF
    );

    /*
     * Outra plataforma.
     */

    rectangle(
        fb,
        TOP_WIDTH,
        TOP_HEIGHT,
        260,
        120,
        70,
        15,
        0x806040FF
    );

    /*
     * Personagem provisório.
     */

    rectangle(
        fb,
        TOP_WIDTH,
        TOP_HEIGHT,
        185,
        160,
        20,
        30,
        0xFFFFFFFF
    );
}


/* =========================================================
   Move jogador
   ========================================================= */

static void updatePlayer(void)
{
    circlePosition pos;

    hidCircleRead(&pos);

    const int speed = 2;

    if (pos.dx > 100)
        playerX += speed;

    if (pos.dx < -100)
        playerX -= speed;

    if (pos.dy > 100)
        playerY -= speed;

    if (pos.dy < -100)
        playerY += speed;

    /*
     * Limites.
     */

    if (playerX < 20)
        playerX = 20;

    if (playerX > 300)
        playerX = 300;

    if (playerY < 25)
        playerY = 25;

    if (playerY > 190)
        playerY = 190;
}


/* =========================================================
   Main
   ========================================================= */

int main(int argc, char **argv)
{
    /*
     * Inicializa 3DS.
     */

    gfxInitDefault();

    /*
     * Inicializa SD/FAT.
     *
     * fsInit() é usado para acesso ao filesystem
     * do console.
     */

    Result rc = fsInit();

    if (R_FAILED(rc))
    {
        gfxExit();

        return 1;
    }

    /*
     * Abre a imagem.
     */

    bool imageLoaded =
        openPSXImage();

    /*
     * Se conseguiu abrir,
     * carrega o primeiro setor.
     */

    if (imageLoaded)
    {
        readSector(0);
    }

    /*
     * Loop principal.
     */

    while (aptMainLoop())
    {
        hidScanInput();

        u32 keysDown =
            hidKeysDown();

        /*
         * START = sair
         */

        if (keysDown & KEY_START)
        {
            break;
        }

        /*
         * A = próximo setor
         */

        if (keysDown & KEY_A)
        {
            if (imageLoaded)
            {
                nextSector();
            }
        }

        /*
         * B = setor anterior
         */

        if (keysDown & KEY_B)
        {
            if (imageLoaded)
            {
                previousSector();
            }
        }

        /*
         * R = +10 setores
         */

        if (keysDown & KEY_R)
        {
            if (imageLoaded)
            {
                advanceSector(10);
            }
        }

        /*
         * L = -10 setores
         */

        if (keysDown & KEY_L)
        {
            if (imageLoaded)
            {
                rewindSector(10);
            }
        }

        /*
         * Circle Pad
         */

        updatePlayer();

        /*
         * Pega framebuffers.
         */

        u32 *top =
            (u32 *)gfxGetFramebuffer(
                GFX_TOP,
                GFX_LEFT,
                NULL,
                NULL
            );

        u32 *bottom =
            (u32 *)gfxGetFramebuffer(
                GFX_BOTTOM,
                GFX_LEFT,
                NULL,
                NULL
            );

        /*
         * Desenha.
         */

        drawTopScreen(top);

        drawBottomMap(bottom);

        /*
         * Atualiza telas.
         */

        gfxFlushBuffers();

        gfxSwapBuffers();

        gspWaitForVBlank();
    }

    /*
     * Fecha arquivo.
     */

    closePSXImage();

    /*
     * Libera filesystem.
     */

    fsExit();

    /*
     * Libera gráficos.
     */

    gfxExit();

    return 0;
}
