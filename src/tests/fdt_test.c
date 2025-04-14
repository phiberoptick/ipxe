/*
 * Copyright (C) 2025 Michael Brown <mbrown@fensystems.co.uk>.
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
 *
 * You can also choose to distribute this program under the terms of
 * the Unmodified Binary Distribution Licence (as given in the file
 * COPYING.UBDL), provided that you have satisfied its requirements.
 */

FILE_LICENCE ( GPL2_OR_LATER_OR_UBDL );

/** @file
 *
 * Flattened device tree self-tests
 *
 */

/* Forcibly enable assertions */
#undef NDEBUG

#include <string.h>
#include <ipxe/fdt.h>
#include <ipxe/test.h>

/** Simplified QEMU sifive_u device tree blob */
static const uint8_t sifive_u[] = {
	0xd0, 0x0d, 0xfe, 0xed, 0x00, 0x00, 0x05, 0x43, 0x00, 0x00, 0x00, 0x38,
	0x00, 0x00, 0x04, 0x68, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x11,
	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdb,
	0x00, 0x00, 0x04, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x1b,
	0x73, 0x69, 0x66, 0x69, 0x76, 0x65, 0x2c, 0x68, 0x69, 0x66, 0x69, 0x76,
	0x65, 0x2d, 0x75, 0x6e, 0x6c, 0x65, 0x61, 0x73, 0x68, 0x65, 0x64, 0x2d,
	0x61, 0x30, 0x30, 0x00, 0x73, 0x69, 0x66, 0x69, 0x76, 0x65, 0x2c, 0x68,
	0x69, 0x66, 0x69, 0x76, 0x65, 0x2d, 0x75, 0x6e, 0x6c, 0x65, 0x61, 0x73,
	0x68, 0x65, 0x64, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x1c,
	0x00, 0x00, 0x00, 0x26, 0x53, 0x69, 0x46, 0x69, 0x76, 0x65, 0x20, 0x48,
	0x69, 0x46, 0x69, 0x76, 0x65, 0x20, 0x55, 0x6e, 0x6c, 0x65, 0x61, 0x73,
	0x68, 0x65, 0x64, 0x20, 0x41, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x63, 0x68, 0x6f, 0x73, 0x65, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x2c, 0x2f, 0x73, 0x6f, 0x63,
	0x2f, 0x73, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x40, 0x31, 0x30, 0x30, 0x31,
	0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x01, 0x61, 0x6c, 0x69, 0x61, 0x73, 0x65, 0x73, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x38,
	0x2f, 0x73, 0x6f, 0x63, 0x2f, 0x73, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x40,
	0x31, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x40,
	0x2f, 0x73, 0x6f, 0x63, 0x2f, 0x65, 0x74, 0x68, 0x65, 0x72, 0x6e, 0x65,
	0x74, 0x40, 0x31, 0x30, 0x30, 0x39, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x63, 0x70, 0x75, 0x73,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x4a,
	0x00, 0x0f, 0x42, 0x40, 0x00, 0x00, 0x00, 0x01, 0x63, 0x70, 0x75, 0x40,
	0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x5d, 0x63, 0x70, 0x75, 0x00, 0x00, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x69, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x6d,
	0x6f, 0x6b, 0x61, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x1b, 0x72, 0x69, 0x73, 0x63,
	0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x25,
	0x00, 0x00, 0x00, 0x74, 0x72, 0x76, 0x36, 0x34, 0x69, 0x6d, 0x61, 0x63,
	0x5f, 0x7a, 0x69, 0x63, 0x6e, 0x74, 0x72, 0x5f, 0x7a, 0x69, 0x63, 0x73,
	0x72, 0x5f, 0x7a, 0x69, 0x66, 0x65, 0x6e, 0x63, 0x65, 0x69, 0x5f, 0x7a,
	0x69, 0x68, 0x70, 0x6d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x69, 0x6e, 0x74, 0x65, 0x72, 0x72, 0x75, 0x70, 0x74, 0x2d, 0x63, 0x6f,
	0x6e, 0x74, 0x72, 0x6f, 0x6c, 0x6c, 0x65, 0x72, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x7e,
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x8f, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0f,
	0x00, 0x00, 0x00, 0x1b, 0x72, 0x69, 0x73, 0x63, 0x76, 0x2c, 0x63, 0x70,
	0x75, 0x2d, 0x69, 0x6e, 0x74, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01,
	0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x40, 0x38, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07,
	0x00, 0x00, 0x00, 0x5d, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x69,
	0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01,
	0x73, 0x6f, 0x63, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x1b,
	0x73, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x2d, 0x62, 0x75, 0x73, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa4,
	0x00, 0x00, 0x00, 0x01, 0x73, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x40, 0x31,
	0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x69, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x1b,
	0x73, 0x69, 0x66, 0x69, 0x76, 0x65, 0x2c, 0x75, 0x61, 0x72, 0x74, 0x30,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01,
	0x65, 0x74, 0x68, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x40, 0x31, 0x30, 0x30,
	0x39, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06,
	0x00, 0x00, 0x00, 0xab, 0x52, 0x54, 0x00, 0x12, 0x34, 0x56, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xbd,
	0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05,
	0x00, 0x00, 0x00, 0xc8, 0x67, 0x6d, 0x69, 0x69, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0xd1,
	0x63, 0x6f, 0x6e, 0x74, 0x72, 0x6f, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x69, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x16,
	0x00, 0x00, 0x00, 0x1b, 0x73, 0x69, 0x66, 0x69, 0x76, 0x65, 0x2c, 0x66,
	0x75, 0x35, 0x34, 0x30, 0x2d, 0x63, 0x30, 0x30, 0x30, 0x2d, 0x67, 0x65,
	0x6d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x65, 0x74, 0x68, 0x65,
	0x72, 0x6e, 0x65, 0x74, 0x2d, 0x70, 0x68, 0x79, 0x40, 0x30, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x69,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x09,
	0x23, 0x61, 0x64, 0x64, 0x72, 0x65, 0x73, 0x73, 0x2d, 0x63, 0x65, 0x6c,
	0x6c, 0x73, 0x00, 0x23, 0x73, 0x69, 0x7a, 0x65, 0x2d, 0x63, 0x65, 0x6c,
	0x6c, 0x73, 0x00, 0x63, 0x6f, 0x6d, 0x70, 0x61, 0x74, 0x69, 0x62, 0x6c,
	0x65, 0x00, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x00, 0x73, 0x74, 0x64, 0x6f,
	0x75, 0x74, 0x2d, 0x70, 0x61, 0x74, 0x68, 0x00, 0x73, 0x65, 0x72, 0x69,
	0x61, 0x6c, 0x30, 0x00, 0x65, 0x74, 0x68, 0x65, 0x72, 0x6e, 0x65, 0x74,
	0x30, 0x00, 0x74, 0x69, 0x6d, 0x65, 0x62, 0x61, 0x73, 0x65, 0x2d, 0x66,
	0x72, 0x65, 0x71, 0x75, 0x65, 0x6e, 0x63, 0x79, 0x00, 0x64, 0x65, 0x76,
	0x69, 0x63, 0x65, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x00, 0x72, 0x65, 0x67,
	0x00, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x00, 0x72, 0x69, 0x73, 0x63,
	0x76, 0x2c, 0x69, 0x73, 0x61, 0x00, 0x23, 0x69, 0x6e, 0x74, 0x65, 0x72,
	0x72, 0x75, 0x70, 0x74, 0x2d, 0x63, 0x65, 0x6c, 0x6c, 0x73, 0x00, 0x69,
	0x6e, 0x74, 0x65, 0x72, 0x72, 0x75, 0x70, 0x74, 0x2d, 0x63, 0x6f, 0x6e,
	0x74, 0x72, 0x6f, 0x6c, 0x6c, 0x65, 0x72, 0x00, 0x72, 0x61, 0x6e, 0x67,
	0x65, 0x73, 0x00, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x2d, 0x6d, 0x61, 0x63,
	0x2d, 0x61, 0x64, 0x64, 0x72, 0x65, 0x73, 0x73, 0x00, 0x70, 0x68, 0x79,
	0x2d, 0x68, 0x61, 0x6e, 0x64, 0x6c, 0x65, 0x00, 0x70, 0x68, 0x79, 0x2d,
	0x6d, 0x6f, 0x64, 0x65, 0x00, 0x72, 0x65, 0x67, 0x2d, 0x6e, 0x61, 0x6d,
	0x65, 0x73, 0x00
};

/**
 * Perform FDT self-test
 *
 */
static void fdt_test_exec ( void ) {
	struct fdt_header *hdr;
	struct fdt fdt;
	const char *string;
	uint64_t u64;
	unsigned int count;
	unsigned int offset;

	/* Verify parsing */
	hdr = ( ( struct fdt_header * ) sifive_u );
	ok ( fdt_parse ( &fdt, hdr, 0 ) != 0 );
	ok ( fdt_parse ( &fdt, hdr, 1 ) != 0 );
	ok ( fdt_parse ( &fdt, hdr, ( sizeof ( sifive_u ) - 1 ) ) != 0 );
	ok ( fdt_parse ( &fdt, hdr, -1UL ) == 0 );
	ok ( fdt.len == sizeof ( sifive_u ) );
	ok ( fdt_parse ( &fdt, hdr, sizeof ( sifive_u ) ) == 0 );
	ok ( fdt.len == sizeof ( sifive_u ) );

	/* Verify string properties */
	ok ( ( string = fdt_string ( &fdt, 0, "model" ) ) != NULL );
	ok ( strcmp ( string, "SiFive HiFive Unleashed A00" ) == 0 );
	ok ( ( string = fdt_string ( &fdt, 0, "nonexistent" ) ) == NULL );

	/* Verify string list properties */
	ok ( ( string = fdt_strings ( &fdt, 0, "model", &count ) ) != NULL );
	ok ( count == 1 );
	ok ( memcmp ( string, "SiFive HiFive Unleashed A00", 28 ) == 0 );
	ok ( ( string = fdt_strings ( &fdt, 0, "compatible",
				      &count ) ) != NULL );
	ok ( count == 2 );
	ok ( memcmp ( string, "sifive,hifive-unleashed-a00\0"
		      "sifive,hifive-unleashed", 52 ) == 0 );
	ok ( ( string = fdt_strings ( &fdt, 0, "nonexistent",
				      &count ) ) == NULL );
	ok ( count == 0 );

	/* Verify integer properties */
	ok ( fdt_u64 ( &fdt, 0, "#address-cells", &u64 ) == 0 );
	ok ( u64 == 2 );
	ok ( fdt_u64 ( &fdt, 0, "#nonexistent", &u64 ) != 0 );

	/* Verify path lookup */
	ok ( fdt_path ( &fdt, "", &offset ) == 0 );
	ok ( offset == 0 );
	ok ( fdt_path ( &fdt, "/", &offset ) == 0 );
	ok ( offset == 0 );
	ok ( fdt_path ( &fdt, "/cpus/cpu@0/interrupt-controller",
			&offset ) == 0 );
	ok ( ( string = fdt_string ( &fdt, offset, "compatible" ) ) != NULL );
	ok ( strcmp ( string, "riscv,cpu-intc" ) == 0 );
	ok ( fdt_path ( &fdt, "//soc/serial@10010000//", &offset ) == 0 );
	ok ( ( string = fdt_string ( &fdt, offset, "compatible" ) ) != NULL );
	ok ( strcmp ( string, "sifive,uart0" ) == 0 );
	ok ( fdt_path ( &fdt, "/nonexistent", &offset ) != 0 );
	ok ( fdt_path ( &fdt, "/cpus/nonexistent", &offset ) != 0 );

	/* Verify alias lookup */
	ok ( fdt_alias ( &fdt, "serial0", &offset ) == 0 );
	ok ( ( string = fdt_string ( &fdt, offset, "compatible" ) ) != NULL );
	ok ( strcmp ( string, "sifive,uart0" ) == 0 );
	ok ( fdt_alias ( &fdt, "nonexistent0", &offset ) != 0 );
}

/** FDT self-test */
struct self_test fdt_test __self_test = {
	.name = "fdt",
	.exec = fdt_test_exec,
};
