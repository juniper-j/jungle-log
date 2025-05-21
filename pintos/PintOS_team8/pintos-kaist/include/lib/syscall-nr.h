#ifndef __LIB_SYSCALL_NR_H
#define __LIB_SYSCALL_NR_H

/* System call numbers. */
enum {
	/* Projects 2 and later. */
	SYS_HALT,                   /* Halt the operating system. == 0 */
	SYS_EXIT,                   /* Terminate this process. == 1 */
	SYS_FORK,                   /* Clone current process. == 2 */
	SYS_EXEC,                   /* Switch current process. == 3 */
	SYS_WAIT,                   /* Wait for a child process to die. == 4 */
	SYS_CREATE,                 /* Create a file. == 5 */
	SYS_REMOVE,                 /* Delete a file. == 6 */
	SYS_OPEN,                   /* Open a file. == 7 */
	SYS_FILESIZE,               /* Obtain a file's size. == 8 */
	SYS_READ,                   /* Read from a file. == 9 */
	SYS_WRITE,                  /* Write to a file. == 10 */
	SYS_SEEK,                   /* Change position in a file. == 11 */
	SYS_TELL,                   /* Report current position in a file. == 12 */
	SYS_CLOSE,                  /* Close a file. == 13 */

	/* Project 3 and optionally project 4. */
	SYS_MMAP,                   /* Map a file into memory. */
	SYS_MUNMAP,                 /* Remove a memory mapping. */

	/* Project 4 only. */
	SYS_CHDIR,                  /* Change the current directory. */
	SYS_MKDIR,                  /* Create a directory. */
	SYS_READDIR,                /* Reads a directory entry. */
	SYS_ISDIR,                  /* Tests if a fd represents a directory. */
	SYS_INUMBER,                /* Returns the inode number for a fd. */
	SYS_SYMLINK,                /* Returns the inode number for a fd. */

	/* Extra for Project 2 */
	SYS_DUP2,                   /* Duplicate the file descriptor */

	SYS_MOUNT,
	SYS_UMOUNT,
};

#endif /* lib/syscall-nr.h */
