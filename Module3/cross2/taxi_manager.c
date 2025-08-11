#include "drivers_manager.h"
#include <stdio.h>

void print_welcome()
{
    printf("                   [\\\n");
    printf("              .----' `-----. \n");
    printf("             //^^^^;;^^^^^^`\\\n");
    printf("     _______//_____||_____()_\\________\n");
    printf("    /666    :      : ___              `\\\n");
    printf("   |>   ____;      ;  |/\\><|   ____   _<)\n");
    printf("  {____/    \\_________________/    \\____}\n");
    printf("       \\ '' /                 \\ '' /\n");
    printf("        '--'                   '--'\n");
}

int main()
{
    print_welcome();
    printf("Для выхода нажмите Ctrl+C\n");

    run_cli();
}