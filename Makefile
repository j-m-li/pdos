
all: pdos.vhd edit run
	echo done

edit: d/edit.c
	cc -ansi -Wall -I d/ -o edit d/edit.c
	./edit

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

