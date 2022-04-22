#include <criterion/criterion.h>
#include <errno.h>
#include "ssq/error.h"

Test(error, set, .description = "Set error") {
    SSQ_ERROR err;
    ssq_error_clear(&err);

    const SSQ_ERROR_CODE code      = SSQ_ERR_BADRES;
    const char           message[] = "Bad response";

    ssq_error_set(&err, code, message);

    cr_expect_eq(err.code, SSQ_ERR_BADRES);
    cr_expect_str_eq(err.message, message);
}

Test(error, set_from_errno, .description = "Set from errno") {
    SSQ_ERROR err;
    ssq_error_clear(&err);

    errno = EINVAL;
    ssq_error_set_from_errno(&err);

    cr_expect_eq(err.code, SSQ_ERR_SYS);
    cr_expect_str_not_empty(err.message);
}

Test(error, clear, .description = "Clear error") {
    SSQ_ERROR err;
    ssq_error_set(&err, SSQ_ERR_BADRES, "Bad response");

    ssq_error_clear(&err);

    cr_expect_eq(err.code, SSQ_OK);
    cr_expect_str_empty(err.message);
}
