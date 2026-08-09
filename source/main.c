int main(void)
{
    gfxInitDefault();
    hidInit();
    audioInit();

    ps1_init();

    ps1_load_iso("/Rayman.iso");

    while (aptMainLoop())
    {
        hidScanInput();

        ps1_run_frame();

        ps1_get_framebuffer(
            top_screen
        );

        draw_bottom_map();

        gfxFlushBuffers();
        gfxSwapBuffers();

        gspWaitForVBlank();
    }

    ps1_shutdown();

    gfxExit();

    return 0;
}
