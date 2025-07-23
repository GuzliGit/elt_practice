#include "permissions.h"
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("Некорректный запуск!\nТребуемый формат: [исполняемый файл] [флаги(опционально)] [файл/каталог].\n");
        return -1;
    }

    exec_chmod_c(argv[argc - 1], argc - 1, argv);

    return 0;
}