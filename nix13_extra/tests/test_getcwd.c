# include <errno.h>

# include "getcwd.h"
# include "utest.h"

UTEST(getcwd_tests, einval_size_0) {
  char mybuf[128];

  EXPECT_EQ(getcwd(mybuf, 0), NULL);
  EXPECT_EQ(errno, EINVAL);
}
