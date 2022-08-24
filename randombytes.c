#include "randombytes.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "crypto_classify.h"

#ifdef GETRANDOM
#include <sys/random.h>

void randombytes(void *x, size_t l)
{
  ssize_t n;
  for (size_t i = 0; i < l; i += n)
    if (0 >= (n = getrandom((char *) x + i, l - i, 0)))
      exit(2);
  crypto_classify(x,l);
}

#else

void randombytes(void *x, size_t l)
{
    static int fd = -1;
    ssize_t n;
    if (fd < 0 && 0 > (fd = open("/dev/urandom", O_RDONLY)))
        exit(1);
    for (size_t i = 0; i < l; i += n)
        if (0 >= (n = read(fd, (char *) x + i, l - i)))
            exit(2);
    crypto_classify(x,l);
}
#endif
