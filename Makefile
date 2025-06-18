
all: pdos.vhd
	echo done

run:
	qemu-system-i386 -drive file=pdos.vhd,index=0,media=disk,format=raw \
	-drive file=fat:rw:./d,id=fat32,format=raw,index=1



pdos.vhd: pdos.zip
	unzip pdos.zip
	touch pdos.vhd

pdos.zip:
	curl --output pdos.zip http://www.pdos.org/pdos.zip
