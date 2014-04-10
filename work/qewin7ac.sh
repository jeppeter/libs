#! /bin/sh 


export  QEMU_AUDIO_DRV=pa
export QEMU_ALSA_VERBOSE=3
export QEMU_PA_SINK=front_out
export QEMU_PA_SOURCE=right2_in
export QEMU_PA_UNIX_SERVER=/run/user/0/pulse/cli
export QEMU_PA_SERVER=/run/user/0/pulse/native
export DISPLAY=192.168.1.20:0.0
./qemu-system-x86_64 -enable-kvm -M q35 -m 1024 -cpu host -smp 2 -bios /usr/share/qemu/bios.bin -device ioh3420,bus=pcie.0,addr=1c.0,multifunction=on,port=1,chassis=1,id=root.1 -device piix4-ide,bus=pcie.0 -drive file=/home/wrc/kvm/win764/win7_x64.img,id=disk,format=qcow2 -device ide-hd,bus=ide.0,drive=disk   -drive file=/home/wrc/work/hda.iso,if=none,media=cdrom,id=drive-ide1-1-0,readonly=on,format=raw -device ide-drive,drive=drive-ide1-1-0,id=ide1-1-0   -monitor stdio -soundhw ac97
