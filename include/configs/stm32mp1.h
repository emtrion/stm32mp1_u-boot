/* SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause */
/*
 * Copyright (C) 2018, STMicroelectronics - All Rights Reserved
 *
 * Configuration settings for the STM32MP15x CPU
 */

#ifndef __CONFIG_H
#define __CONFIG_H
#include <linux/sizes.h>
#include <asm/arch/stm32.h>

#ifndef CONFIG_STM32MP1_TRUSTED
/* PSCI support */
#define CONFIG_ARMV7_PSCI_1_0
#define CONFIG_ARMV7_SECURE_BASE		STM32_SYSRAM_BASE
#define CONFIG_ARMV7_SECURE_MAX_SIZE		STM32_SYSRAM_SIZE
#endif

/*
 * Configuration of the external SRAM memory used by U-Boot
 */
#define CONFIG_SYS_SDRAM_BASE			STM32_DDR_BASE
#define CONFIG_SYS_INIT_SP_ADDR			CONFIG_SYS_TEXT_BASE

/*
 * Console I/O buffer size
 */
#define CONFIG_SYS_CBSIZE			SZ_1K

/*
 * default load address used for command tftp,  bootm , loadb, ...
 */
#define CONFIG_LOADADDR			0xc2000000
#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR

/* ATAGs */
#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/*
 * For booting Linux, use the first 256 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ		SZ_256M

/* Extend size of kernel image for uncompression */
#define CONFIG_SYS_BOOTM_LEN		SZ_32M

/* SPL support */
#ifdef CONFIG_SPL
/* SPL use DDR */
#define CONFIG_SPL_BSS_START_ADDR	0xC0200000
#define CONFIG_SPL_BSS_MAX_SIZE		0x00100000
#define CONFIG_SYS_SPL_MALLOC_START	0xC0300000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x00100000

/* limit SYSRAM usage to first 128 KB */
#define CONFIG_SPL_MAX_SIZE		0x00020000
#define CONFIG_SPL_STACK		(STM32_SYSRAM_BASE + \
					 STM32_SYSRAM_SIZE)
#endif /* #ifdef CONFIG_SPL */

#define CONFIG_SYS_MEMTEST_START	STM32_DDR_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + SZ_64M)
#define CONFIG_SYS_MEMTEST_SCRATCH	(CONFIG_SYS_MEMTEST_END + 4)

/*MMC SD*/
#define CONFIG_SYS_MMC_MAX_DEVICE	3

/* NAND support */
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_SYS_MAX_NAND_DEVICE	1

/* SPI FLASH support */
#if defined(CONFIG_SPL_BUILD)
#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x80000
#endif

/* Ethernet need */
#ifdef CONFIG_DWC_ETH_QOS
#define CONFIG_SYS_NONCACHED_MEMORY	(1 * SZ_1M)	/* 1M */
#define CONFIG_SERVERIP                 192.168.1.1
#define CONFIG_BOOTP_SERVERIP
#define CONFIG_SYS_AUTOLOAD		"no"
#endif

#ifdef CONFIG_DM_VIDEO
#define CONFIG_VIDEO_BMP_RLE8
#define CONFIG_BMP_16BPP
#define CONFIG_BMP_24BPP
#define CONFIG_BMP_32BPP
#endif

/*****************************************************************************/
#ifdef CONFIG_DISTRO_DEFAULTS
/*****************************************************************************/

#if !defined(CONFIG_SPL_BUILD)

#ifdef CONFIG_CMD_MMC
#define BOOT_TARGET_MMC0(func)	func(MMC, mmc, 0)
#define BOOT_TARGET_MMC1(func)	func(MMC, mmc, 1)
#define BOOT_TARGET_MMC2(func)	func(MMC, mmc, 2)
#else
#define BOOT_TARGET_MMC0(func)
#define BOOT_TARGET_MMC1(func)
#define BOOT_TARGET_MMC2(func)
#endif

#ifdef CONFIG_NET
#define BOOT_TARGET_PXE(func)	func(PXE, pxe, na)
#else
#define BOOT_TARGET_PXE(func)
#endif

#ifdef CONFIG_CMD_UBIFS
#define BOOT_TARGET_UBIFS(func)	func(UBIFS, ubifs, 0)
#else
#define BOOT_TARGET_UBIFS(func)
#endif

#define BOOT_TARGET_DEVICES(func)	\
	BOOT_TARGET_MMC1(func)		\
	BOOT_TARGET_UBIFS(func)		\
	BOOT_TARGET_MMC0(func)		\
	BOOT_TARGET_MMC2(func)		\
	BOOT_TARGET_PXE(func)

/*
 * bootcmd for stm32mp1:
 *    CONFIG_BOOTCOMMAND="run bootcmd_stm32mp"
 * for serial/usb: execute the stm32prog command
 * for mmc boot (eMMC, SD card), boot only on the same device
 * for nand or spi-nand boot, boot with on ubifs partition on UBI partition
 * for nor boot, use SD card = mmc0
 */

#define STM32MP_BOOTCMD "bootcmd_stm32mp=" \
	"if test ${boot_device} = serial || test ${boot_device} = usb; then " \
		"stm32prog ${boot_device} ${boot_instance}; " \
	"else " \
		"if test ${boot_device} = mmc; then run emmc_boot; fi;" \
		"if test ${boot_device} = nor; then run emmc_boot; fi;" \
	"fi;\0"

/* DTIMG command added only for Android distribution */
#ifdef CONFIG_CMD_DTIMG
/*
 * bootcmd for android on MMC:
 *    CONFIG_BOOTCOMMAND="run bootcmd_android"
 * overidde DISTRO script "mmc_boot" to boot android on mmc
 * using system_${suffix} partition (with "_a") by default
 * - display splash screen
 * - load device tree form dtimg
 * - load kernel and set bootargs
 * - start kernel
 */

#define STM32MP_ANDROID \
	"suffix=a\0" \
	"dtimg_addr=0xc44FFF80\0" \
	"android_mmc_splash="\
		"if part start mmc ${devnum} splash splash_start && " \
		   "part size mmc ${devnum} splash splash_size;"\
		"then " \
		   "mmc read ${splashimage} ${splash_start} ${splash_size};" \
		   "cls; bmp display ${splashimage} m m;" \
		"fi\0" \
	"android_mmc_fdt="\
		"if part start mmc ${devnum} dt_${suffix} dt_start &&" \
		   "part size mmc ${devnum} dt_${suffix} dt_size;"\
		"then " \
		   "mmc read ${dtimg_addr} ${dt_start} ${dt_size};" \
		   "dtimg getindex ${dtimg_addr} ${board_id} ${board_rev}" \
		     " dt_index;" \
		   "dtimg start ${dtimg_addr} ${dt_index} fdt_addr_r;"\
		"fi\0" \
	"android_mmc_kernel="\
		"if part start mmc ${devnum} boot_${suffix} boot_start && " \
		   "part size mmc ${devnum} boot_${suffix} boot_size;"\
		"then " \
		   "mmc read ${kernel_addr_r} ${boot_start} ${boot_size};" \
		   "env set bootargs ${android_bootargs} " \
		     "androidboot.serialno=${serial#} " \
		     "androidboot.slot_suffix=_${suffix};" \
		"fi\0" \
	"android_mmc_boot="\
		"mmc dev ${devnum};"\
		"run android_mmc_splash;" \
		"run android_mmc_fdt;" \
		"run android_mmc_kernel;" \
		"bootm ${kernel_addr_r} ${kernel_addr_r} ${fdt_addr_r};\0" \
	"bootcmd_android=" \
		"env set mmc_boot run android_mmc_boot;" \
		"run bootcmd_stm32mp\0"

#else
#define STM32MP_ANDROID
#endif/* CONFIG_CMD_DTIMG */

#ifdef CONFIG_FASTBOOT_CMD_OEM_FORMAT
/* eMMC default partitions for fastboot command: oem format */
#define PARTS_DEFAULT \
	"partitions=" \
	"name=ssbl,size=2M;" \
	"name=bootfs,size=64MB,bootable;" \
	"name=vendorfs,size=16M;" \
	"name=rootfs,size=746M;" \
	"name=userfs,size=-\0"
#else
#define PARTS_DEFAULT
#endif

#include <config_distro_bootcmd.h>

#define SD_BOOT \
	"sd_boot=ext4load mmc 0:4 ${kernel_addr_r} /boot/${image.linux};" \
	"setenv bootargs \"console=${console} root=/dev/mmcblk0p4 rw rootwait mtdparts=256k(fsbl1),256k(fsbl2),1m(ssbl)\";" \
	"bootm ${kernel_addr_r};\0"
#define EMMC_BOOT \
	"emmc_boot=ext4load mmc 1:3 ${kernel_addr_r} /boot/${image.linux};" \
	"setenv bootargs \"console=${console} root=/dev/mmcblk1p3 rw rootwait mtdparts=256k(fsbl1),256k(fsbl2),1m(ssbl)\";" \
	"bootm ${kernel_addr_r};\0"
#define CONFIGURE_IP \
	"configure-ip=if test -n \"${ip-method}\"; \
	then if test \"${ip-method}\" = dhcp; \
	then setenv ip dhcp && setenv autoload no && dhcp ; \
	elif test \"${ip-method}\" = static; \
	then if test -n \"${ipaddr}\" && test -n \"${serverip}\" && test -n \"${netmask}\"; \
	then setenv ip ${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}:eth0:off; \
	else echo You have to set ipaddr, netmask and serverip when using ip-method static. ;  false; fi; \
	else echo ip-method has to be either dhcp or static. ; false ; fi; else echo ip-method has to be either dhcp or static. ; false ; fi\0"
#define NFS_TEST \
	"test-nfsroot=if test -n \"${nfsroot}\"; then true ; else echo Please set nfsroot variable. ; false ; fi\0"
#define NET_BOOT \
	"net_boot=run configure-ip && run test-nfsroot && nfs ${nfsroot}/boot/uboot_script && env import -t ${fileaddr} ${filesize} && if test -n \"${uboot_script_net_boot}\"; then run uboot_script_net_boot; else echo Bootscript does not define uboot_script_net_boot, aborting. ; fi\0"
#define UPDATE_UBOOT \
	"update_uboot=if test -n \"${serverip}\"; then run configure-ip && sf probe && sf erase fsbl1 0x40000 && sf erase fsbl2 0x40000 && nfs ${kernel_addr_r} ${nfsroot}/boot/u-boot-spl.stm32-stm32mp157c-emsbc-argon-basic && sf write $fileaddr fsbl1 $filesize && sf write $fileaddr fsbl2 $filesize && sf erase ssbl 0x100000 && nfs ${nfsroot}/boot/u-boot-stm32mp157c-emsbc-argon-basic.img && sf write $fileaddr ssbl $filesize && echo Update U-Boot successful; else echo Please set serverip variable. ; fi\0"
#define UPDATE_KERNEL \
	"update_kernel=if test -n \"${serverip}\"; then run configure-ip && run test-nfsroot && nfs ${nfsroot}/boot/uboot_script && env import -t ${fileaddr} ${filesize} && if test -n \"${uboot_script_update_kernel}\"; then run uboot_script_update_kernel; else echo Bootscript does not define uboot_script_update kernel, aborting ; fi ; else echo Please set serverip variable. ; fi\0"
#define UPDATE_ROOTFS \
	"update_rootfs=if test -n \"${serverip}\"; then run configure-ip && run test-nfsroot && nfs ${nfsroot}/boot/uboot_script && env import -t ${fileaddr} ${filesize} && if test -n \"${uboot_script_update_rootfs}\" ; then run uboot_script_update_rootfs; else echo Bootscript does not define update_rootfs, aborting ; fi ; else echo Please set serverip variable. ; fi\0"
#define RES_SYS \
	"restore_sys=if test -n \"${serverip}\"; then run configure-ip && run test-nfsroot && nfs ${nfsroot}/boot/uboot_script && env import -t ${fileaddr} ${filesize} && if test -n \"${uboot_script_restore_sys}\"; then run uboot_script_restore_sys; else echo Bootscript does not define uboot_script_restore_sys, aborting. ; fi ; else echo Please set serverip variable. ; fi\0"

/*
 * memory layout for 32M uncompressed/compressed kernel,
 * 1M fdt, 1M script, 1M pxe and 1M for splashimage
 * and the ramdisk at the end.
 */
#define CONFIG_EXTRA_ENV_SETTINGS \
	"stdin=serial\0" \
	"stdout=serial\0" \
	"stderr=serial\0" \
	"bootdelay=1\0" \
	"console=ttySTM0,115200\0" \
	"kernel_addr_r=0xc2000000\0" \
	"image.linux=linux\0" \
	"fdt_addr_r=0xc4000000\0" \
	"scriptaddr=0xc4100000\0" \
	"ramdisk_addr_r=0xc4400000\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"bootlimit=0\0" \
	STM32MP_BOOTCMD \
	STM32MP_ANDROID \
	PARTS_DEFAULT \
	BOOTENV \
	SD_BOOT \
	EMMC_BOOT \
	CONFIGURE_IP \
	NFS_TEST \
	NET_BOOT \
	UPDATE_UBOOT \
	UPDATE_KERNEL \
	UPDATE_ROOTFS \
	RES_SYS

#endif /* ifndef CONFIG_SPL_BUILD */
#endif /* ifdef CONFIG_DISTRO_DEFAULTS*/

#endif /* __CONFIG_H */
