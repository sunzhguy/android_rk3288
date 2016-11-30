/*
 * (C) Copyright 2008-2014 Rockchip Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <command.h>
#include <linux/sizes.h>

#include <fastboot.h>
#include <malloc.h>
#include <../board/rockchip/common/config.h>
#include <generated/timestamp_autogenerated.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_RK_HDMI)
extern short g_hdmi_vic;
#endif

#if defined(CONFIG_UBOOT_CHARGE) && defined(CONFIG_POWER_FG_ADC)
extern u8 g_increment;
#endif

#ifdef CONFIG_RK3036_TVE
extern int g_tve_pos;
#endif

#ifdef CONFIG_BOOTM_LINUX
extern int do_bootm_linux(int flag, int argc, char *argv[],
		        bootm_headers_t *images);
#endif

extern int rkimage_load_image(rk_boot_img_hdr *hdr,
		const disk_partition_t *boot_ptn, const disk_partition_t *kernel_ptn);

/* Section for Android bootimage format support
 * Refer:
 * http://android.git.kernel.org/?p=platform/system/core.git;a=blob;f=mkbootimg/bootimg.h
 */
static void bootimg_print_image_hdr(rk_boot_img_hdr *hdr)
{
	int i;

	debug("   Image magic:   %s\n", hdr->magic);

	debug("   kernel_size:   0x%x\n", hdr->kernel_size);
	debug("   kernel_addr:   0x%x\n", hdr->kernel_addr);

	debug("   rdisk_size:   0x%x\n", hdr->ramdisk_size);
	debug("   rdisk_addr:   0x%x\n", hdr->ramdisk_addr);

	debug("   second_size:   0x%x\n", hdr->second_size);
	debug("   second_addr:   0x%x\n", hdr->second_addr);

	debug("   tags_addr:   0x%x\n", hdr->tags_addr);
	debug("   page_size:   0x%x\n", hdr->page_size);

	debug("   name:      %s\n", hdr->name);
	debug("   cmdline:   %s\n", hdr->cmdline);

	for (i = 0; i < 8; i++)
		debug("   id[%d]:   0x%x\n", i, hdr->id[i]);
}


#ifdef CONFIG_LMB
static void boot_start_lmb(bootm_headers_t *images)
{
	ulong		mem_start;
	phys_size_t	mem_size;

	lmb_init(&images->lmb);

	mem_start = getenv_bootm_low();
	mem_size = getenv_bootm_size();

	lmb_add(&images->lmb, (phys_addr_t)mem_start, mem_size);

	arch_lmb_reserve(&images->lmb);
	debug("\n");
	board_lmb_reserve(&images->lmb);
	debug("\n");
}
#else
#define lmb_reserve(lmb, base, size)
static inline void boot_start_lmb(bootm_headers_t *images) { }
#endif

static int rk_bootrk_start(bootm_headers_t *images)
{
	boot_start_lmb(images);
	images->state = BOOTM_STATE_OS_GO;

	return 0;
}


static rk_boot_img_hdr * rk_load_image_from_ram(char *ram_addr,
		bootm_headers_t *pimage)
{
	rk_boot_img_hdr *hdr = NULL;
	unsigned addr;
	char *ep;
	void *kaddr, *raddr, *secaddr;

#ifdef CONFIG_KERNEL_RUNNING_ADDR
	kaddr = (void*)CONFIG_KERNEL_RUNNING_ADDR;
#else
	kaddr = (void*)CONFIG_KERNEL_LOAD_ADDR;
#endif
	raddr = (void*)(gd->arch.rk_boot_buf_addr);

	addr = simple_strtoul(ram_addr, &ep, 16);
	if (ep == ram_addr || *ep != '\0') {
		printf("'%s' does not seem to be a partition nor "
				"an address\n", ram_addr);
		/* this is most likely due to having no
		 * partition table in factory case, or could
		 * be argument is wrong.  in either case, start
		 * fastboot mode.
		 */
		return NULL;
	}

	hdr = malloc(sizeof(rk_boot_img_hdr));
	if (hdr == NULL) {
		printf("error allocating buffer\n");
		return NULL;
	}
	/* set this aside somewhere safe */
	memcpy(hdr, (void *) addr, sizeof(rk_boot_img_hdr));

	if (memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		printf("bootrk: bad boot image magic\n");
		free(hdr);
		return NULL;
	}

	hdr->ramdisk_addr = (int)raddr;
	hdr->kernel_addr = (int)kaddr;
	kaddr = (void *)(addr + hdr->page_size);
	raddr = (void *)(kaddr + ALIGN(hdr->kernel_size,
				hdr->page_size));
	secaddr = (void *)(raddr + ALIGN(hdr->ramdisk_size,
				hdr->page_size));

	memmove((void *)hdr->kernel_addr, kaddr, hdr->kernel_size);
	memmove((void *)hdr->ramdisk_addr, raddr, hdr->ramdisk_size);

	char* fastboot_unlocked_env = getenv(FASTBOOT_UNLOCKED_ENV_NAME);
	unsigned long unlocked = 0;
	if (fastboot_unlocked_env) {
		if (!strict_strtoul(fastboot_unlocked_env, 10, &unlocked)) {
			unlocked = unlocked? 1 : 0;
		}
	}

	/* check image */
	if (SecureBootImageCheck(hdr, unlocked) == false) {
#ifdef CONFIG_SECUREBOOT_CRYPTO
		if ((SecureMode != SBOOT_MODE_NS) && (SecureBootCheckOK == 0)) {
			puts("Not allow to boot no secure sign image!");
			while(1);
		}
#endif /* CONFIG_SECUREBOOT_CRYPTO */

		/* if image check error, boot fail */
		board_fbt_boot_failed("ramdisk");
	}

	/* loader fdt */
#ifdef CONFIG_OF_LIBFDT
	resource_content content =
		rkimage_load_fdt_ram(secaddr, hdr->second_size);
	if (!content.load_addr) {
		printf("failed to load fdt from %p!\n", ram_addr);
#ifdef CONFIG_OF_FROM_RESOURCE
		content = rkimage_load_fdt(get_disk_partition(RESOURCE_NAME));
#endif
	}
	if (!content.load_addr) {
		printf("failed to load fdt!\n");
		goto fail;
	} else {
		pimage->ft_addr = content.load_addr;
		pimage->ft_len = content.content_size;
	}
#endif /* CONFIG_OF_LIBFDT */

	return hdr;

fail:
	/* if booti fails, always start fastboot */
	free(hdr); /* hdr may be NULL, but that's ok. */
	return NULL;
}


static rk_boot_img_hdr * rk_load_image_from_storage(const disk_partition_t* ptn, bootm_headers_t *pimage)
{
	rk_boot_img_hdr *hdr = NULL;
	unsigned long blksz = ptn->blksz;
	unsigned sector;
	unsigned blocks;
	void *kaddr, *raddr;
#ifdef CONFIG_OF_LIBFDT
	resource_content content;

	/* init set content.addr = NULL */
	content.load_addr = NULL;
#endif

#ifdef CONFIG_KERNEL_RUNNING_ADDR
	kaddr = (void*)CONFIG_KERNEL_RUNNING_ADDR;
#else
	kaddr = (void*)CONFIG_KERNEL_LOAD_ADDR;
#endif
	raddr = (void*)(gd->arch.rk_boot_buf_addr);

	hdr = memalign(ARCH_DMA_MINALIGN, blksz << 2);
	if (hdr == NULL) {
		FBTERR("error allocating blksz(%lu) buffer\n", blksz);
		return NULL;
	}

	if (StorageReadLba(ptn->start, (void *) hdr, 1 << 2) != 0) {
		FBTERR("bootrk: failed to read bootimg header\n");
		goto fail;
	}

	if (memcmp(hdr->magic, BOOT_MAGIC,
				BOOT_MAGIC_SIZE)) {
		memset(hdr, 0, blksz);
		hdr->kernel_addr = (uint32)kaddr;
		hdr->ramdisk_addr = (uint32)raddr;

		snprintf((char*)hdr->magic, BOOT_MAGIC_SIZE, "%s\n", "RKIMAGE!");
		if (rkimage_load_image(hdr, ptn, get_disk_partition(KERNEL_NAME)) != 0) {
			FBTERR("bootrk: bad boot or kernel image\n");
			goto fail;
		}
	} else {
		hdr->kernel_addr = (uint32)kaddr;
		hdr->ramdisk_addr = (uint32)raddr;

		sector = ptn->start + (hdr->page_size / blksz);
		blocks = DIV_ROUND_UP(hdr->kernel_size, blksz);
		if (StorageReadLba(sector, (void *) hdr->kernel_addr, \
					blocks) != 0) {
			FBTERR("bootrk: failed to read kernel\n");
			goto fail;
		}

		sector += ALIGN(hdr->kernel_size, hdr->page_size) / blksz;
		blocks = DIV_ROUND_UP(hdr->ramdisk_size, blksz);
		if (StorageReadLba(sector, (void *) hdr->ramdisk_addr, \
					blocks) != 0) {
			FBTERR("bootrk: failed to read ramdisk\n");
			goto fail;
		}
#ifdef CONFIG_SECUREBOOT_CRYPTO
		if (hdr->second_size != 0) {
			hdr->second_addr = hdr->ramdisk_addr + blksz * blocks;

			sector += ALIGN(hdr->ramdisk_size, hdr->page_size) / blksz;
			blocks = DIV_ROUND_UP(hdr->second_size, blksz);
			if (StorageReadLba(sector, (void *) hdr->second_addr, \
						blocks) != 0) {
				FBTERR("bootrk: failed to read second\n");
				goto fail;
			}

			/* load fdt from boot image sencode address */
			#ifdef CONFIG_OF_LIBFDT
			debug("Try to load fdt from second address.\n");
			content = rkimage_load_fdt_ram(hdr->second_addr, hdr->second_size);
			if (!content.load_addr) {
				printf("failed to load fdt from second address %p!\n", hdr->second_addr);
			}
			#endif /* CONFIG_OF_LIBFDT */
		}
#else
		#ifdef CONFIG_OF_LIBFDT
		if (hdr->second_size != 0) {
			/* load fdt from boot image */
			debug("Try to load fdt from %s.\n", ptn->name);
			content = rkimage_load_fdt(ptn);
			if (!content.load_addr) {
				printf("failed to load fdt from %s!\n", ptn->name);
			}
		}
		#endif /* CONFIG_OF_LIBFDT */
#endif /* CONFIG_SECUREBOOT_CRYPTO */
	}

	char* fastboot_unlocked_env = getenv(FASTBOOT_UNLOCKED_ENV_NAME);
	unsigned long unlocked = 0;
	if (fastboot_unlocked_env) {
		if (!strict_strtoul(fastboot_unlocked_env, 10, &unlocked)) {
			unlocked = unlocked? 1 : 0;
		}
	}

	/* check image */
	if (SecureBootImageCheck(hdr, unlocked) == false) {
#ifdef CONFIG_SECUREBOOT_CRYPTO
		if ((SecureMode != SBOOT_MODE_NS) && (SecureBootCheckOK == 0)) {
			puts("Not allow to boot no secure sign image!\n");
			while(1);
		}
#endif /* CONFIG_SECUREBOOT_CRYPTO */

		/* if image check error, boot fail */
		board_fbt_boot_failed(ptn->name);
	}

	/* loader fdt from resource if content.load_addr == NULL */
#ifdef CONFIG_OF_LIBFDT
	if (!content.load_addr) {
		#ifdef CONFIG_OF_FROM_RESOURCE
		puts("load fdt from resouce.\n");
		content = rkimage_load_fdt(get_disk_partition(RESOURCE_NAME));
		#endif
	}

	if (!content.load_addr) {
		puts("failed to load fdt!\n");
		goto fail;
	} else {
		pimage->ft_addr = content.load_addr;
		pimage->ft_len = content.content_size;
	}
#endif /* CONFIG_OF_LIBFDT */

	return hdr;

fail:
	/* if booti fails, always start fastboot */
	free(hdr); /* hdr may be NULL, but that's ok. */
	return NULL;
}

#if defined(CONFIG_LCD) && defined(CONFIG_KERNEL_LOGO)
static int g_rk_fb_size = -1;
static int rk_load_kernel_logo(void)
{
	const char* file_path = "logo_kernel.bmp";
	resource_content content;
	int blocks;
	int offset = CONFIG_RK_FB_SIZE;

	debug("loader kernel logo from resource.\n");
	g_rk_fb_size = -1;
	memset(&content, 0, sizeof(content));

	snprintf(content.path, sizeof(content.path), "%s", file_path);

	/* kernel logo default load from resource */
	if (!get_content(0, &content))
		return -1;
	content.load_addr = (void *)gd->fb_base + offset;

	blocks = (content.content_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
	if (content.content_size > CONFIG_RK_LCD_SIZE - offset) {
		FBTERR("Failed to load bmp image, too large, %d\n",
		       content.content_size);
		return -1;
	}
	if (!load_content_data(&content, 0, content.load_addr, blocks)) {
		return -1;
	}

	g_rk_fb_size = CONFIG_RK_FB_SIZE;

	return offset;
}
#endif

static int bootrk_no_ramdisk = 0;
static void rk_commandline_setenv(const char *boot_name, rk_boot_img_hdr *hdr, bool charge)
{
#ifdef CONFIG_CMDLINE_TAG
	/* static just to be safe when it comes to the stack */
	static char command_line[1024];
	/* Use the cmdline from board_fbt_finalize_bootargs instead of
	 * any hardcoded into u-boot.  Also, Android wants the
	 * serial number on the command line instead of via
	 * tags so append the serial number to the bootimg header
	 * value and set the bootargs environment variable.
	 * do_bootm_linux() will use the bootargs environment variable
	 * to pass it to the kernel.  Add the bootloader
	 * version too.
	 */

	board_fbt_finalize_bootargs(command_line, sizeof(command_line),
			hdr->ramdisk_addr, hdr->ramdisk_size,
			!strcmp(boot_name, RECOVERY_NAME));

	// Storage Media Name
	uint32 media = StorageGetBootMedia();
	char *medianame = NULL;
	if (media == BOOT_FROM_FLASH) {
		medianame = "nand";
	} else if (media == BOOT_FROM_EMMC) {
		medianame = "emmc";
	} else if (media == BOOT_FROM_SD0) {
		medianame = "sd";
	} else if (media == BOOT_FROM_UMS) {
		medianame = "ums";
	}

	if (medianame != NULL) {
		snprintf(command_line, sizeof(command_line),
				"%s storagemedia=%s", command_line, medianame);
	}

#ifdef CONFIG_RK_SDCARD_BOOT_EN
	if (StorageSDCardUpdateMode() != 0) { // sd \BF\A8\C9\FD\BC\B6\A3\AC\BD\F8\C8\EBrecovery
		snprintf(command_line, sizeof(command_line),
				"%s %s", command_line, "sdfwupdate");
	}
#endif

#ifdef CONFIG_RK_UMS_BOOT_EN
	if (StorageUMSUpdateMode() != 0) { // ums update
		snprintf(command_line, sizeof(command_line),
				"%s %s", command_line, "usbfwupdate");
	}
#endif

	if (charge) {
		snprintf(command_line, sizeof(command_line),
				"%s %s", command_line, "androidboot.mode=charger");
	}

#if defined(CONFIG_LCD) && defined(CONFIG_RK_FB_DDREND)
	/*
	 * uboot fb commandline: uboot_logo=<size>@<address>[:<offset>]
	 * size - fb size, address - fb address, offset - kernel bmp logo offset.
	 * offset is optional, depend on resource image has kernel_logo.bmp.
	 */
	snprintf(command_line, sizeof(command_line),
			"%s uboot_logo=0x%08lx@0x%08lx", command_line, CONFIG_RK_LCD_SIZE, gd->fb_base);
#if defined(CONFIG_KERNEL_LOGO)
	if (g_rk_fb_size != -1)
		snprintf(command_line, sizeof(command_line),
				"%s:0x%08lx", command_line, g_rk_fb_size);
#endif /* CONFIG_KERNEL_LOGO */
#endif /* CONFIG_RK_FB_DDREND */

	snprintf(command_line, sizeof(command_line),
			"%s loader.timestamp=%s", command_line, U_BOOT_TIMESTAMP);

#if defined(CONFIG_RK_HDMI)
	snprintf(command_line, sizeof(command_line),
			 "%s hdmi.vic=%d", command_line, g_hdmi_vic);
#endif


#ifdef CONFIG_RK3036_TVE
	snprintf(command_line, sizeof(command_line),
			"%s tve.format=%d", command_line, g_tve_pos);
#endif

#if defined(CONFIG_UBOOT_CHARGE) && defined(CONFIG_POWER_FG_ADC)
	snprintf(command_line, sizeof(command_line),
			 "%s adc.incre=%d", command_line, g_increment);
#endif

	char *sn = getenv("fbt_sn#");
	if (sn != NULL) {
		/* append serial number if it wasn't in device_info already */
		if (!strstr(command_line, FASTBOOT_SERIALNO_BOOTARG)) {
			snprintf(command_line, sizeof(command_line),
					"%s %s=%s", command_line, FASTBOOT_SERIALNO_BOOTARG, sn);
		}
	}

	command_line[sizeof(command_line) - 1] = 0;
	if(strstr(command_line, "root=") != NULL)
		bootrk_no_ramdisk = 1;

	setenv("bootargs", command_line);
#endif /* CONFIG_CMDLINE_TAG */
}


/* bootrk [ <addr> | <partition> ] */
int do_bootrk(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *boot_source = "boot";
	rk_boot_img_hdr *hdr = NULL;
	const disk_partition_t* ptn = NULL;
	bootm_headers_t images;
	bool charge = false;

	if (argc >= 2) {
		if (!strcmp(argv[1], "charge")) {
			charge = true;
		} else {
			boot_source = argv[1];
		}
	}

	memset(&images, 0, sizeof(images));
	if (rk_bootrk_start(&images)) { /*it returns 1 when failed.*/
		puts("bootrk: failed to setup lmb!\n");
		goto fail;
	}

	ptn = get_disk_partition(boot_source);
	if (ptn) {
		hdr = rk_load_image_from_storage(ptn, &images);
		if (hdr == NULL) {
			goto fail;
		}
	} else {
		hdr = rk_load_image_from_ram(boot_source, &images);
		if (hdr == NULL) {
			goto fail;
		}
	}
#if defined(CONFIG_LCD) && defined(CONFIG_KERNEL_LOGO)
	rk_load_kernel_logo();
#endif

#if defined(CONFIG_UBOOT_CHARGE) && defined(CONFIG_POWER_FG_ADC)
	if (fg_adc_storage_flag_load() == 1) {
		g_increment = g_increment + fg_adc_storage_load();
	}

	fg_adc_storage_flag_store(0);
	fg_adc_storage_store(0);
#endif

	rk_commandline_setenv(boot_source, hdr, charge);

	rk_module_deinit();

	/* Secure boot state will set drm, sn and others information in the nanc ram,
	 * so, after set, PLS notice do not read/write nand flash.
	 */
	SecureBootSecureState2Kernel(SecureBootCheckOK);

	/* after here, make sure no read/write storate */
	if(bootrk_no_ramdisk)
	{
		hdr->ramdisk_addr = 0;
		hdr->ramdisk_size = 0;
	}
	bootimg_print_image_hdr(hdr);
	printf("kernel   @ 0x%08x (0x%08x)\n", hdr->kernel_addr, hdr->kernel_size);
	printf("ramdisk  @ 0x%08x (0x%08x)\n", hdr->ramdisk_addr, hdr->ramdisk_size);

	images.ep = hdr->kernel_addr;
	images.rd_start = hdr->ramdisk_addr;
	images.rd_end = hdr->ramdisk_addr + hdr->ramdisk_size;

#ifdef CONFIG_IMPRECISE_ABORTS_CHECK
	puts("enable imprecise aborts check.");
	enable_imprecise_aborts();
#endif

#ifdef CONFIG_BOOTM_LINUX
	puts("bootrk: do_bootm_linux...\n");
	do_bootm_linux(0, 0, NULL, &images);
#endif /* CONFIG_BOOTM_LINUX */

fail:
	board_fbt_boot_failed(boot_source);

	puts("bootrk: Control returned to monitor - resetting...\n");
	do_reset(cmdtp, flag, argc, argv);
	return 1;
}

U_BOOT_CMD(
		bootrk,	2,	1,	do_bootrk,
		"boot rockchip android bootimg",
#ifdef DEBUG
		"[ <addr> | <partition> ]\n    - boot application image\n"
		"\t'addr' should be the address of the boot image which is\n"
		"\tzImage+ramdisk.img if in memory.  'partition' is the name\n"
		"\tof the partition to boot from.  The default is to boot\n"
		"\tfrom the 'boot' partition.\n"
#else
		"\n"
#endif
		);

