build_os:
	arm-none-eabi-g++ -mcpu=cortex-a7 -fpic -ffreestanding -c ./src/bootloader.s -o ./build/bootloader.o
	arm-none-eabi-g++ -mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu++98 -c ./src/kernel.c -o ./build/kernel.o -nostdlib -lgcc -fno-rtti -fno-exceptions -O2 -Wall -Wextra -Werror
	arm-none-eabi-g++ -T ./linker.ld -o ./bin/lucidpi.elf -ffreestanding -O2 -nostdlib ./build/bootloader.o ./build/kernel.o -lgcc
	arm-none-eabi-objcopy ./bin/lucidpi.elf -O binary ./bin/lucidpi.img