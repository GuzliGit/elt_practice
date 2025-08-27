obj-m += netfilter_mod.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

userspace: user.c
	gcc -o user user.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f user
