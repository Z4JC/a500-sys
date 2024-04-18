# include <errno.h>
# include <stdint.h>
# include <stdlib.h>

# include <proto/dos.h>

char *getcwd(char *buf, size_t size) {
  if (!size) return errno = EINVAL, NULL;

  char *mybuf = NULL;
  if (!buf && !(mybuf = buf = (char *) malloc(size)))
    return errno = ENOMEM, NULL;

  BPTR dirlock = Lock("", ACCESS_READ);
  if (!dirlock) { errno = ENOENT; goto fail; }

  char *out = buf;
  *out = 0;

  size_t out_size = 0;
  do {
    struct FileInfoBlock fib;
    if (!Examine(dirlock, &fib)) { errno = EIO; goto fail; }

    BPTR new_dirlock = ParentDir(dirlock);
    UnLock(dirlock);

    if (out_size && new_dirlock) *out++ = '/';

    char *name = fib.fib_FileName, *endname = name;
    while (*endname++) {}
    uint16_t name_len = --endname - name;
    const int needs_colon = !new_dirlock && (!name_len || endname[-1] != ':');

    out_size += name_len + needs_colon;
    if (out_size >= size) { errno = ERANGE; goto fail; }

    if (needs_colon)
      *out++ = ':';

    while (name_len--)
      *out++ = *--endname;

    dirlock = new_dirlock;
  } while (dirlock);

  *out = 0;

  for (char *rev = buf; rev < out;) {
    char t = *--out;
    *out = *rev;
    *rev++ = t;
  }

  return buf;
 fail:
  free(mybuf);
  UnLock(dirlock);
  return NULL;
}
