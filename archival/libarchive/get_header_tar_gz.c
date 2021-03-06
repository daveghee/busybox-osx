/* vi: set sw=4 ts=4: */
/*
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

#include "libbb.h"
#include "bb_archive.h"

char FAST_FUNC get_header_tar_gz(archive_handle_t *archive_handle)
{
#if BB_MMU
	uint16_t magic;
#endif

	/* Can't lseek over pipes */
	archive_handle->seek = seek_by_read;

	/* Check gzip magic only if open_transformer will invoke unpack_gz_stream (MMU case).
	 * Otherwise, it will invoke an external helper "gunzip -cf" (NOMMU case) which will
	 * need the header. */
#if BB_MMU
	xread(archive_handle->src_fd, &magic, 2);
	/* Can skip this check, but error message will be less clear */
	if (magic != GZIP_MAGIC) {
		bb_error_msg_and_die("invalid gzip magic");
	}
#endif

	open_transformer(archive_handle->src_fd, unpack_gz_stream, "gunzip");
	archive_handle->offset = 0;
	while (get_header_tar(archive_handle) == EXIT_SUCCESS)
		continue;

	/* Can only do one file at a time */
	return EXIT_FAILURE;
}
