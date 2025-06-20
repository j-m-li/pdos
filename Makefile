
all: pdos.vhd edit 
	echo done

edit: d/edit.c d/main.c d/std.h
	cc -ansi -Wall -I d/ -o edit.exe d/main.c d/edit.c \
		d/tk.c d/tk_inline.c d/tk_style.c d/tk_block.c d/tk_text.c \
		d/tk_range.c

mount:
	mkdir -p mnt
	sudo mount -o loop,umask=000,offset=$$((128*512)) -t vfat pdos.vhd mnt

run:
	qemu-system-i386 -drive file=pdos.vhd,index=0,media=disk,format=raw \
	-drive file=fat:rw:./d,id=fat32,format=raw,index=1



pdos.vhd: pdos.zip
	unzip pdos.zip
	mv uc386.vhd pdos.vhd
	touch pdos.vhd

pdos.zip:
#	curl --output pdos.zip http://www.pdos.org/pdos.zip
	curl --output pdos.zip http://www.pdos.org/uc386.zip

clean:
	rm -f edit
	rm -f *.zip *.vhd

