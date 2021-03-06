#!/bin/bash
#
# Description	: Android Build Script.
# Authors	: lqm www.9tripod.com
# Version	: 1.0
# Notes		: None
#

#
# JAVA PATH
#
export PATH=/usr/lib/jvm/jdk1.6.0_45/bin:$PATH

#
# Some Directories
#
BS_DIR_TOP=$(cd `dirname $0` ; pwd)
BS_DIR_TOOLS=${BS_DIR_TOP}/tools
BS_DIR_RELEASE=${BS_DIR_TOP}/out/release
BS_DIR_UBOOT=${BS_DIR_TOP}/uboot
BS_DIR_KERNEL=${BS_DIR_TOP}/kernel
BS_DIR_BUILDROOT=${BS_DIR_TOP}/buildroot
OUT=${BS_DIR_TOP}/out/target/product/rk3288

#
# Target Config
#
BS_CONFIG_BOOTLOADER_UBOOT=x3288_config
BS_CONFIG_KERNEL=x3288_defconfig
BS_CONFIT_BUILDROOT=x3288_defconfig

setup_environment()
{
	LANG=C
	cd ${BS_DIR_TOP};

	PATH=${BS_DIR_TOP}/out/host/linux-x86/bin:$PATH;

	mkdir -p ${BS_DIR_RELEASE} || return 1
	[ -f "$BS_DIR_RELEASE/upgrade_tool" ] || { cp -v $BS_DIR_TOOLS/upgrade_tool $BS_DIR_RELEASE;}
	[ -f "$BS_DIR_RELEASE/config.ini" ] || { cp -v $BS_DIR_TOOLS/config.ini $BS_DIR_RELEASE;}
}

build_bootloader_uboot()
{
	# Compiler uboot
	cd ${BS_DIR_UBOOT} || return 1
	make distclean || return 1
	make ${BS_CONFIG_BOOTLOADER_UBOOT} || return 1
	make -j${threads} || return 1

	# Copy bootloader to release directory
	cp -v ${BS_DIR_UBOOT}/RK3288UbootLoader_V2.19.09.bin ${BS_DIR_RELEASE}/uboot.bin

	echo "^_^ uboot path: ${BS_DIR_RELEASE}/uboot.bin"
	return 0
}

build_kernel()
{
	#export PATH=${BS_DIR_UBOOT}/tools:$PATH 
	# Compiler kernel
	cd ${BS_DIR_KERNEL} || return 1
	make ${BS_CONFIG_KERNEL} || return 1
	make x3288.img -j${threads} || return 1

	# Copy kernel.img & resource.img to release directory
	cp -v ${BS_DIR_KERNEL}/kernel.img ${BS_DIR_RELEASE}
	cp -v ${BS_DIR_KERNEL}/resource.img ${BS_DIR_RELEASE}

	return 0
}

build_system()
{
	# Compiler buildroot
	cd ${BS_DIR_BUILDROOT} || return 1
	make ${BS_CONFIT_BUILDROOT} || return 1
	make || return 1

	# Copy image to release directory
	cp -v ${BS_DIR_BUILDROOT}/output/images/rootfs.ext2 ${BS_DIR_RELEASE}/qt_rootfs.img

	chmod a+r -R $BS_DIR_RELEASE/

	# generate update.img
	cd ${BS_DIR_RELEASE} || return 1
	# Copy package-file and parameter.txt
	cp -av ${BS_DIR_TOOLS}/package-file ${BS_DIR_RELEASE} || return 1;
	cp -av ${BS_DIR_TOOLS}/parameter.txt ${BS_DIR_RELEASE} || return 1;
	# Firmware pack
	${BS_DIR_TOOLS}/afptool -pack ${BS_DIR_RELEASE}/ ${BS_DIR_RELEASE}/temp.img || return 1;
	# Generating image
	${BS_DIR_TOOLS}/rkImageMaker -RK32 ${BS_DIR_RELEASE}/uboot.bin ${BS_DIR_RELEASE}/temp.img ${BS_DIR_RELEASE}/update.img -os_type:androidos || return 1;
	rm -fr ${BS_DIR_RELEASE}/temp.img || return 1;

	echo "update.img is generated now!"

	return 0
}

threads=$(grep processor /proc/cpuinfo | awk '{field=$NF};END{print field+1}')
uboot=no
kernel=no
system=no

if [ -z $1 ]; then
	uboot=yes
	kernel=yes
	system=yes
fi

while [ "$1" ]; do
    case "$1" in
	-j=*)
		x=$1
		threads=${x#-j=}
		;;
	-u|--uboot)
		uboot=yes
	    ;;
	-k|--kernel)
	    	kernel=yes
	    ;;
	-s|--system)
		system=yes
	    ;;
	-a|--all)
		uboot=yes
		kernel=yes
		system=yes
	    ;;
	-h|--help)
	    cat >&2 <<EOF
Usage: build.sh [OPTION]
Build script for compile the source of telechips project.

  -j=n                 using n threads when building source project (example: -j=16)
  -u, --uboot          build bootloader uboot from source
  -k, --kernel         build kernel from source
  -s, --system         build android file system from source
  -a, --all            build all, include anything
  -h, --help           display this help and exit
EOF
	    exit 0
	    ;;
	*)
	    echo "build.sh: Unrecognised option $1" >&2
	    exit 1
	    ;;
    esac
    shift
done

setup_environment || exit 1

if [ "${uboot}" = yes ]; then
	build_bootloader_uboot || exit 1
fi

if [ "${kernel}" = yes ]; then
	build_kernel || exit 1
fi

if [ "${system}" = yes ]; then
	build_system || exit 1
fi

exit 0
