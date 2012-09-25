#include "tty.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>

struct termios stdin_saved_attributes;
struct termios tty_saved_attributes;

int tty_open(const char *tty_dev)
{
	struct termios new_attributes;

	int tty_fd = open(tty_dev,O_RDWR| O_NOCTTY | O_NONBLOCK);

	if (tty_fd<0) {
		return -1;
	} else {
		tcgetattr(tty_fd,&tty_saved_attributes);
		tcgetattr(tty_fd,&new_attributes);

		// Set the new attributes for the serial port
		// http://linux.about.com/library/cmd/blcmdl3_termios.htm
		// http://www.gnu.org/software/libc/manual/html_node/Low_002dLevel-I_002fO.html#Low_002dLevel-I_002fO

		// c_cflag
		new_attributes.c_cflag |= CREAD;		 	// Enable receiver
		new_attributes.c_cflag |= CS8;			 	// 8 data bit

		// c_iflag
		new_attributes.c_iflag |= IGNPAR;		 	// Ignore framing errors and parity errors.

		// c_lflag
		new_attributes.c_lflag &= ~(ICANON); 	// DISABLE canonical mode.
		// Disables the special characters EOF, EOL, EOL2,
		// ERASE, KILL, LNEXT, REPRINT, STATUS, and WERASE, and buffers by lines.
		new_attributes.c_lflag &= ~(ECHO);		// DISABLE this: Echo input characters.
		new_attributes.c_lflag &= ~(ECHOE);		// DISABLE this: If ICANON is also set, the ERASE character erases the preceding input
		// character, and WERASE erases the preceding word.
		new_attributes.c_lflag &= ~(ISIG);		// DISABLE this: When any of the characters INTR, QUIT, SUSP,
		// or DSUSP are received, generate the corresponding signal.

		new_attributes.c_cc[VMIN]=1;					// Minimum number of characters for non-canonical read.
		new_attributes.c_cc[VTIME]=0;					// Timeout in deciseconds for non-canonical read.

		cfsetospeed(&new_attributes,B9600);		// Set the baud rate
		cfsetispeed(&new_attributes,B9600);


		tcsetattr(tty_fd, TCSANOW, &new_attributes);
	}
	return tty_fd;
}

void tty_write(int fd, const uint8_t *buf, size_t len)
{
	size_t to_write = len;
	while (to_write > 0)
		to_write -= write(fd, buf + len - to_write, to_write);
	tcdrain(fd);
}
