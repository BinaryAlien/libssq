#include <criterion/criterion.h>
#include <errno.h>
#include "ssq/error.h"

Test(error, set) {
    SSQ_ERROR err;
    ssq_error_clear(&err);

    ssq_error_set(&err, SSQ_ERR_BADRES, "Bad response");

    cr_expect_eq(err.code, SSQ_ERR_BADRES);
    cr_expect_str_eq(err.message, "Bad response");
}

Test(error, set_from_errno) {
    SSQ_ERROR err;
    ssq_error_clear(&err);

    errno = EINVAL;
    ssq_error_set_from_errno(&err);

    cr_expect_eq(err.code, SSQ_ERR_SYS);
    cr_expect_str_not_empty(err.message);
}

Test(error, clear) {
    SSQ_ERROR err;
    ssq_error_set(&err, SSQ_ERR_BADRES, "Bad response");

    ssq_error_clear(&err);

    cr_expect_eq(err.code, SSQ_OK);
    cr_expect_str_empty(err.message);
}
