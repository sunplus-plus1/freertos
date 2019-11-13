/* See LICENSE of license details. */

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "platform.h"
#include "stub.h"
#include "weak_under_alias.h"

ssize_t __wrap_write(int fd, const void* ptr, size_t len)
{
  const uint8_t * current = (const uint8_t *)ptr;

  if (isatty(fd)) {
    for (size_t jj = 0; jj < len; jj++) {
      platform_putchar(current[jj]);

      if (current[jj] == '\n') {
        platform_putchar('\r');
      }
    }
    return len;
  }

  return _stub(EBADF);
}
weak_under_alias(write);
