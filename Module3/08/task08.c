#include "p_manager.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc < 2 || (argc - 1) % 2 != 0)
    {
        printf("Корректный ввод: <имя программы> <файл> <кол-во потребителей на файл> <файл> ...\n");
        exit(EXIT_FAILURE);
    }

    start_processing(argc - 1, argv + 1);

    return EXIT_SUCCESS;
}