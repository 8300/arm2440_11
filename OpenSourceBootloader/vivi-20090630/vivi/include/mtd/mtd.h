/*
 * vivi/include/mtd.h:
 *
 * Based on linux/include/linux/mtd/mtd.h
 *
 * $Id: mtd.h,v 1.1.1.1 2004/02/04 06:22:25 laputa Exp $ 
 */

#ifndef __VIVI_MTD_MTD_H__
#define __VIVI_MTD_MTD_H__


#include "config.h"
#include <types.h>


struct erase_info_user {
	u_int32_t start;
	u_int32_t length;
};

struct mtd_oob_buf {
	u_int32_t start;
	u_int32_t length;
	unsigned char *ptr;
};


#define MTD_CHAR_MAJOR 90
#define MTD_BLOCK_MAJOR 31
#define MAX_MTD_DEVICES 16


#define MTD_ABSENT		0
#define MTD_RAM			1
#define MTD_ROM			2
#define MTD_NORFLASH		3
#define MTD_NANDFLASH		4
#define MTD_PEROM		5
#define MTD_OTHER		14
#define MTD_UNKNOWN		15



#define MTD_CLEAR_BITS		1       // Bits can be cleared (flash)
#define MTD_SET_BITS		2       // Bits can be set
#define MTD_ERASEABLE		4       // Has an erase function
#define MTD_WRITEB_WRITEABLE	8       // Direct IO is possible
#define MTD_VOLATILE		16      // Set for RAMs
#define MTD_XIP			32	// eXecute-In-Place possible
#define MTD_OOB			64	// Out-of-band data (NAND flash)
#define MTD_ECC			128	// Device capable of automatic ECC

// Some common devices / combinations of capabilities
#define MTD_CAP_ROM		0
#define MTD_CAP_RAM		(MTD_CLEAR_BITS|MTD_SET_BITS|MTD_WRITEB_WRITEABLE)
#define MTD_CAP_NORFLASH        (MTD_CLEAR_BITS|MTD_ERASEABLE)
#define MTD_CAP_NANDFLASH       (MTD_CLEAR_BITS|MTD_ERASEABLE|MTD_OOB)
#define MTD_WRITEABLE		(MTD_CLEAR_BITS|MTD_SET_BITS)


// Types of automatic ECC/Checksum available
#define MTD_ECC_NONE		0 	// No automatic ECC available
#define MTD_ECC_RS_DiskOnChip	1	// Automatic ECC on DiskOnChip
#define MTD_ECC_SW		2	// SW ECC for Toshiba & Samsung devices

struct mtd_info_user {
	u_char type;
	u_int32_t flags;
	u_int32_t size;	 // Total size of the MTD
	u_int32_t erasesize;
	u_int32_t oobblock;  // Size of OOB blocks (e.g. 512)
	u_int32_t oobsize;   // Amount of OOB data per block (e.g. 16)
	u_int32_t ecctype;
	u_int32_t eccsize;
};

struct region_info_user {
	u_int32_t offset;		/* At which this region starts, 
					 * from the beginning of the MTD */
	u_int32_t erasesize;		/* For this region */
	u_int32_t numblocks;		/* Number of blocks in this region */
	u_int32_t regionindex;
};


#define MTD_ERASE_PENDING      	0x01
#define MTD_ERASING		0x02
#define MTD_ERASE_SUSPEND	0x04
#define MTD_ERASE_DONE          0x08
#define MTD_ERASE_FAILED        0x10

struct erase_info {
	struct mtd_info *mtd;
	u_int32_t addr;
	u_int32_t len;
	u_long time;
	u_long retries;
	u_int dev;
	u_int cell;
	void (*callback) (struct erase_info *self);
	u_long priv;
	u_char state;
	struct erase_info *next;
};

struct mtd_erase_region_info {
	u_int32_t offset;			/* At which this region starts, from the beginning of the MTD */
	u_int32_t erasesize;		/* For this region */
	u_int32_t numblocks;		/* Number of blocks of erasesize in this region */
};

struct mtd_info {
	u_char type;
	u_int32_t flags;
	u_int32_t size;	 // Total size of the MTD

	/* "Major" erase size for the device. Na?ve users may take this
	 * to be the only erase size available, or may use the more detailed
	 * information below if they desire
	 */
	u_int32_t erasesize;

	u_int32_t oobblock;  // Size of OOB blocks (e.g. 512)
	u_int32_t oobsize;   // Amount of OOB data per block (e.g. 16)
	u_int32_t ecctype;
	u_int32_t eccsize;

	// Kernel-only stuff starts here.
	char *name;
	int index;

	/* Data for variable erase regions. If numeraseregions is zero,
	 * it means that the whole device has erasesize as given above. 
	 */
	int numeraseregions;
	struct mtd_erase_region_info *eraseregions; 

	/* This really shouldn't be here. It can go away in 2.5 */
	u_int32_t bank_size;

	struct module *module;
	int (*erase) (struct mtd_info *mtd, struct erase_info *instr);

	/* This stuff for eXecute-In-Place */
	int (*point) (struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char **mtdbuf);

	/* We probably shouldn't allow XIP if the unpoint isn't a NULL */
	void (*unpoint) (struct mtd_info *mtd, u_char * addr);


	int (*read) (struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf);
	int (*write) (struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u_char *buf);

	int (*read_ecc) (struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf, u_char *eccbuf);
	int (*write_ecc) (struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u_char *buf, u_char *eccbuf);

	int (*read_oob) (struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf);
	int (*write_oob) (struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u_char *buf);

	/* 
	 * Methods to access the protection register area, present in some 
	 * flash devices. The user data is one time programmable but the
	 * factory data is read only. 
	 */
	int (*read_user_prot_reg) (struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf);

	int (*read_fact_prot_reg) (struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf);

	/* This function is not yet implemented */
	int (*write_user_prot_reg) (struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf);


	/* Chip-supported device locking */
	int (*lock) (struct mtd_info *mtd, loff_t ofs, size_t len);
	int (*unlock) (struct mtd_info *mtd, loff_t ofs, size_t len);

	void *priv;
};

/*
 * VIVI Interfaces
 */
#ifdef CONFIG_MTD
int write_to_flash(loff_t ofs, size_t len, const u_char *buf, int flag);
int mtd_dev_init(void);
#else
#define write_to_flash(a, b, c, d) (int)(1)
#define mtd_dev_init() (int)(1)
#endif

/*
 * Debugging macro and defines
 */
#define MTD_DEBUG_LEVEL0	(0)	/* Quiet   */
#define MTD_DEBUG_LEVEL1	(1)	/* Audible */
#define MTD_DEBUG_LEVEL2	(2)	/* Loud    */
#define MTD_DEBUG_LEVEL3	(3)	/* Noisy   */

#ifdef CONFIG_MTD_DEBUG
#define DEBUG(n, args...)			\
	if (n <=  CONFIG_MTD_DEBUG_VERBOSE) {	\
		printk( ##args );	\
	}
#else /* CONFIG_MTD_DEBUG */
#define DEBUG(n, args...)
#endif /* CONFIG_MTD_DEBUG */

#endif /* __VIVI_MTD_MTD_H__ */
