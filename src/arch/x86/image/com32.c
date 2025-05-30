/*
 * Copyright (C) 2008 Daniel Verkamp <daniel@drv.nu>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/**
 * @file
 *
 * SYSLINUX COM32 image format
 *
 */

FILE_LICENCE ( GPL2_OR_LATER );

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>
#include <realmode.h>
#include <basemem.h>
#include <comboot.h>
#include <ipxe/uaccess.h>
#include <ipxe/image.h>
#include <ipxe/segment.h>
#include <ipxe/init.h>
#include <ipxe/memmap.h>
#include <ipxe/console.h>

/**
 * Execute COMBOOT image
 *
 * @v image		COM32 image
 * @ret rc		Return status code
 */
static int com32_exec_loop ( struct image *image ) {
	struct memmap_region region;
	int state;
	uint32_t avail_mem_top;

	state = rmsetjmp ( comboot_return );

	switch ( state ) {
	case 0: /* First time through; invoke COM32 program */

		/* Find end of block covering COM32 image loading area */
		memmap_describe ( COM32_START_PHYS, 1, &region );
		assert ( memmap_is_usable ( &region ) );
		avail_mem_top = ( COM32_START_PHYS + memmap_size ( &region ) );
		DBGC ( image, "COM32 %s: available memory top = 0x%x\n",
		       image->name, avail_mem_top );
		assert ( avail_mem_top != 0 );

		/* Hook COMBOOT API interrupts */
		hook_comboot_interrupts();

		/* Unregister image, so that a "boot" command doesn't
		 * throw us into an execution loop.  We never
		 * reregister ourselves; COMBOOT images expect to be
		 * removed on exit.
		 */
		unregister_image ( image );

		__asm__ __volatile__ ( PHYS_CODE (
			/* Preserve registers */
			"pushal\n\t"
			/* Preserve stack pointer */
			"subl $4, %k0\n\t"
			"movl %%esp, (%k0)\n\t"
			/* Switch to COM32 stack */
			"movl %k0, %%esp\n\t"
			/* Enable interrupts */
			"sti\n\t"
			/* Construct stack frame */
			"pushl %k1\n\t"
			"pushl %k2\n\t"
			"pushl %k3\n\t"
			"pushl %k4\n\t"
			"pushl %k5\n\t"
			"pushl %k6\n\t"
			"pushl $6\n\t"
			/* Call COM32 entry point */
			"movl %k7, %k0\n\t"
			"call *%k0\n\t"
			/* Disable interrupts */
			"cli\n\t"
			/* Restore stack pointer */
			"movl 28(%%esp), %%esp\n\t"
			/* Restore registers */
			"popal\n\t" )
			:
			: "R" ( avail_mem_top ),
			  "R" ( virt_to_phys ( com32_cfarcall_wrapper ) ),
			  "R" ( virt_to_phys ( com32_farcall_wrapper ) ),
			  "R" ( get_fbms() * 1024 - ( COM32_BOUNCE_SEG << 4 ) ),
			  "i" ( COM32_BOUNCE_SEG << 4 ),
			  "R" ( virt_to_phys ( com32_intcall_wrapper ) ),
			  "R" ( virt_to_phys ( image->cmdline ?
					       image->cmdline : "" ) ),
			  "i" ( COM32_START_PHYS )
			: "memory" );
		DBGC ( image, "COM32 %s: returned\n", image->name );
		break;

	case COMBOOT_EXIT:
		DBGC ( image, "COM32 %s: exited\n", image->name );
		break;

	case COMBOOT_EXIT_RUN_KERNEL:
		assert ( image->replacement );
		DBGC ( image, "COM32 %s: exited to run kernel %s\n",
		       image->name, image->replacement->name );
		break;

	case COMBOOT_EXIT_COMMAND:
		DBGC ( image, "COM32 %s: exited after executing command\n",
		       image->name );
		break;

	default:
		assert ( 0 );
		break;
	}

	unhook_comboot_interrupts();
	comboot_force_text_mode();

	return 0;
}

/**
 * Check image name extension
 *
 * @v image		COM32 image
 * @ret rc		Return status code
 */
static int com32_identify ( struct image *image ) {
	const char *ext;
	static const uint8_t magic[] = { 0xB8, 0xFF, 0x4C, 0xCD, 0x21 };

	if ( image->len >= sizeof ( magic ) ) {
		/* Check for magic number
		 * mov eax,21cd4cffh
		 * B8 FF 4C CD 21
		 */
		if ( memcmp ( image->data, magic, sizeof ( magic) ) == 0 ) {
			DBGC ( image, "COM32 %s: found magic number\n",
			       image->name );
			return 0;
		}
	}

	/* Magic number not found; check filename extension */

	ext = strrchr( image->name, '.' );

	if ( ! ext ) {
		DBGC ( image, "COM32 %s: no extension\n",
		       image->name );
		return -ENOEXEC;
	}

	++ext;

	if ( strcasecmp( ext, "c32" ) ) {
		DBGC ( image, "COM32 %s: unrecognized extension %s\n",
		       image->name, ext );
		return -ENOEXEC;
	}

	return 0;
}


/**
 * Load COM32 image into memory
 * @v image		COM32 image
 * @ret rc		Return status code
 */
static int com32_load_image ( struct image *image ) {
	size_t filesz, memsz;
	void *buffer;
	int rc;

	filesz = image->len;
	memsz = filesz;
	buffer = phys_to_virt ( COM32_START_PHYS );
	if ( ( rc = prep_segment ( buffer, filesz, memsz ) ) != 0 ) {
		DBGC ( image, "COM32 %s: could not prepare segment: %s\n",
		       image->name, strerror ( rc ) );
		return rc;
	}

	/* Copy image to segment */
	memcpy ( buffer, image->data, filesz );

	return 0;
}

/**
 * Prepare COM32 low memory bounce buffer
 * @v image		COM32 image
 * @ret rc		Return status code
 */
static int com32_prepare_bounce_buffer ( struct image * image ) {
	void *seg;
	size_t filesz, memsz;
	int rc;

	seg = real_to_virt ( COM32_BOUNCE_SEG, 0 );

	/* Ensure the entire 64k segment is free */
	memsz = 0xFFFF;
	filesz = 0;

	/* Prepare, verify, and load the real-mode segment */
	if ( ( rc = prep_segment ( seg, filesz, memsz ) ) != 0 ) {
		DBGC ( image, "COM32 %s: could not prepare bounce buffer segment: %s\n",
		       image->name, strerror ( rc ) );
		return rc;
	}

	return 0;
}

/**
 * Probe COM32 image
 *
 * @v image		COM32 image
 * @ret rc		Return status code
 */
static int com32_probe ( struct image *image ) {
	int rc;

	/* Check if this is a COMBOOT image */
	if ( ( rc = com32_identify ( image ) ) != 0 ) {
		return rc;
	}

	return 0;
}

/**
 * Execute COMBOOT image
 *
 * @v image		COM32 image
 * @ret rc		Return status code
 */
static int com32_exec ( struct image *image ) {
	int rc;

	/* Load image */
	if ( ( rc = com32_load_image ( image ) ) != 0 ) {
		return rc;
	}

	/* Prepare bounce buffer segment */
	if ( ( rc = com32_prepare_bounce_buffer ( image ) ) != 0 ) {
		return rc;
	}

	/* Reset console */
	console_reset();

	return com32_exec_loop ( image );
}

/** SYSLINUX COM32 image type */
struct image_type com32_image_type __image_type ( PROBE_NORMAL ) = {
	.name = "COM32",
	.probe = com32_probe,
	.exec = com32_exec,
};
