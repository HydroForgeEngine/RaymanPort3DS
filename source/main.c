
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ISO_PATH "/Rayman.iso"
#define SECTOR_SIZE 2048

static FILE *iso = NULL;
static uint64_t isoSize = 0;
static uint64_t totalSectors = 0;
static uint64_t currentSector = 0;

static unsigned char sectorBuffer[SECTOR_SIZE];

/* ---------------------------------------------------------
   Abre a ISO
   --------------------------------------------------------- */

static int isoOpen(void)
{
    iso = fopen(ISO_PATH, "rb");

    if (iso == NULL)
        return 0;

    if (fseek(iso, 0, SEEK_END) != 0)
    {
        fclose(iso);
        iso = NULL;
        return 0;
    }

    long size = ftell(iso);

    if (size <= 0)
    {
        fclose(iso);
        iso = NULL;
        return 0;
    }

    isoSize = (uint64_t)size;

    rewind(iso);

    totalSectors =
        isoSize / SECTOR_SIZE;

    return 1;
}

/* ---------------------------------------------------------
   Fecha ISO
   --------------------------------------------------------- */

static void isoClose(void)
{
    if (iso != NULL)
    {
        fclose(iso);
        iso = NULL;
    }
}

/* ---------------------------------------------------------
   Lê um setor de 2048 bytes
   --------------------------------------------------------- */

static int isoReadSector(uint64_t sector)
{
    if (iso == NULL)
        return 0;

    if (sector >= totalSectors)
        return 0;

    uint64_t offset =
        sector * SECTOR_SIZE;

    if (fseek(iso, (long)offset, SEEK_SET) != 0)
        return 0;

    size_t result =
        fread(
            sectorBuffer,
            1,
            SECTOR_SIZE,
            iso
        );

    if (result != SECTOR_SIZE)
        return 0;

    currentSector = sector;

    return 1;
}

/* ---------------------------------------------------------
   Imprime informações da ISO
   --------------------------------------------------------- */

static void printISOInfo(void)
{
    printf("\x1b[1;1H");
    printf("================================\n");
    printf("        RAYMAN 3DS ISO\n");
    printf("================================\n\n");

    printf("Arquivo:\n");
    printf("%s\n\n", ISO_PATH);

    printf("Tamanho:\n");
    printf("%llu bytes\n\n",
           (unsigned long long)isoSize);

    printf("Setor:\n");
    printf("2048 bytes\n\n");

    printf("Total de setores:\n");
    printf("%llu\n\n",
           (unsigned long long)totalSectors);

    printf("Setor atual:\n");
    printf("%llu\n\n",
           (unsigned long long)currentSector);

    printf("Controles:\n");
    printf("A     = proximo setor\n");
    printf("B     = setor anterior\n");
    printf("R     = +10 setores\n");
    printf("L     = -10 setores\n");
    printf("START = sair\n");
}

/* ---------------------------------------------------------
   Mostra os primeiros bytes do setor
   --------------------------------------------------------- */

static void printSectorData(void)
{
    printf("\x1b[15;1H");
    printf("Primeiros 32 bytes do setor:\n\n");

    for (int i = 0; i < 32; i++)
    {
        printf("%02X ", sectorBuffer[i]);

        if ((i + 1) % 16 == 0)
            printf("\n");
    }
}

/* ---------------------------------------------------------
   Tela inferior
   --------------------------------------------------------- */

static void drawBottomScreen(void)
{
    consoleClear();

    printf("\x1b[1;1H");
    printf("========== MAPA ==========\n\n");

    printf("       +---------+\n");
    printf("       |         |\n");
    printf("       |    @    |\n");
    printf("       |    |    |\n");
    printf("   +---+----+----+---+\n");
    printf("   |                  |\n");
    printf("   |      AREA 01     |\n");
    printf("   |                  |\n");
    printf("   +------------------+\n\n");

    printf("@ = jogador\n\n");

    printf("SETOR: %llu\n",
           (unsigned long long)currentSector);

    printf("\nA/B = navegar\n");
    printf("L/R = saltar setores\n");
}

/* ---------------------------------------------------------
   Main
   --------------------------------------------------------- */

int main(int argc, char **argv)
{
    gfxInitDefault();

    consoleInit(
        GFX_TOP,
        NULL
    );

    consoleInit(
        GFX_BOTTOM,
        NULL
    );

    printf("Inicializando...\n\n");

    if (!isoOpen())
    {
        printf("ERRO!\n\n");
        printf("Nao foi possivel abrir:\n");
        printf("%s\n\n", ISO_PATH);

        printf("Coloque Rayman.iso na raiz\n");
        printf("do cartao SD.\n\n");

        printf("Pressione START para sair.");

        while (aptMainLoop())
        {
            hidScanInput();

            if (hidKeysDown() & KEY_START)
                break;

            gspWaitForVBlank();
        }

        gfxExit();

        return 1;
    }

    /*
     * Lê o primeiro setor.
     */

    if (!isoReadSector(0))
    {
        printf("Erro lendo setor 0.\n");

        while (aptMainLoop())
        {
            hidScanInput();

            if (hidKeysDown() & KEY_START)
                break;

            gspWaitForVBlank();
        }

        isoClose();
        gfxExit();

        return 1;
    }

    while (aptMainLoop())
    {
        hidScanInput();

        u32 keys =
            hidKeysDown();

        /* START */

        if (keys & KEY_START)
            break;

        /* A = próximo setor */

        if (keys & KEY_A)
        {
            if (currentSector + 1 < totalSectors)
            {
                isoReadSector(
                    currentSector + 1
                );
            }
        }

        /* B = setor anterior */

        if (keys & KEY_B)
        {
            if (currentSector > 0)
            {
                isoReadSector(
                    currentSector - 1
                );
            }
        }

        /* R = +10 */

        if (keys & KEY_R)
        {
            uint64_t next =
                currentSector + 10;

            if (next >= totalSectors)
                next = totalSectors - 1;

            isoReadSector(next);
        }

        /* L = -10 */

        if (keys & KEY_L)
        {
            uint64_t previous;

            if (currentSector < 10)
                previous = 0;
            else
                previous = currentSector - 10;

            isoReadSector(previous);
        }

        /*
         * Tela superior
         */

        consoleSelect(
            consoleGetDefault()
        );

        printISOInfo();
        printSectorData();

        /*
         * Tela inferior.
         *
         * Para uma interface definitiva,
         * podemos trocar o console por
         * framebuffer/Citro2D posteriormente.
         */

        consoleSelect(
            consoleGetDefault()
        );

        /*
         * Atualiza.
         */

        gfxFlushBuffers();
        gfxSwapBuffers();

        gspWaitForVBlank();
    }

    isoClose();

    gfxExit();

    return 0;
}
