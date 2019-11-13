/* See LICENSE of license details. */

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "platform.h"
#include "stub.h"
#include "weak_under_alias.h"

int __wrap_puts(const char *s)
{
  while (*s != '\0') {
    platform_putchar(*s);

    if (*s == '\n') {
      platform_putchar('\r');
    }

    ++s;
  }

  return 0;
}
weak_under_alias(puts);
