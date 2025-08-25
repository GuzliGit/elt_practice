# Задание 3

### Для начала была произведена компиляция модуля

![Make](Screenshots/make.png "")

### После этого модуль был загружен с помощью insmod, также был проверен список активных модулей и системный журнал

![Insmod](Screenshots/insmod.png "")

### Также было проверено создание соответствующего файла в sysfs

![Sysfs](Screenshots/sysfs.png "")

### Ниже представлен пример использования модуля

![Test](Screenshots/test.png "")

### После этого модуль был выгружен из ядра, а в системном журнале появилось соответствующее сообщение

![Rmmod](Screenshots/rmmod.png "")

## Примеры работы
 
### Значение: 1 (001) = ScrollLock

![ScrollLock](Screenshots/ScrollLock.gif "")

### Значение: 5 (101) = ScrollLock + CapsLock

![Caps+ScrollLock](Screenshots/Caps+ScrollLock.gif "")
