obj-m =
obj-m += MioSoC.o


ifeq ($(KERNEL_PATH),)
$(error please set KERNEL_PATH for kernel compile)
endif

all:MioSoC.ko sigsoc

MioSoC.ko:MioSoC.c MioSoC.h
	make -C ${KERNEL_PATH} M=${PWD} modules

sigsoc:main.o
	gcc -Wall -o $@ $^

main.o:main.c
	gcc -Wall -c $< -o $@

clean:
	make -C ${KERNEL_PATH} M=${PWD} clean
	rm -f main.o sigsoc
	# to remove the backup files
	rm -f *~ *.gbk.c *.gbk.h