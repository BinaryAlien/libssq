#include <criterion/criterion.h>
#include "ssq/ssq.h"

#ifdef _WIN32
static void helper_expect_timeouts_eq(const DWORD actual, const time_t expected_timeout_im_ms) {
    cr_expect_eq(actual, expected_timeout_in_ms);
}
#else /* not _WIN32 */
static void helper_expect_timeouts_eq(const struct timeval actual, const time_t expected_timeout_in_ms) {
    cr_expect_eq(actual.tv_sec, expected_timeout_in_ms / 1000);
    cr_expect_eq(actual.tv_usec, expected_timeout_in_ms % 1000 * 1000);
}
#endif /* _WIN32 */

Test(ssq, init, .description = "Initialization") {
    SSQ_QUERIER *const querier = ssq_init();

    cr_assert_neq(querier, NULL);

    cr_expect_eq(querier->addr_list, NULL);
    cr_expect(ssq_ok(querier));
    cr_expect_str_empty(ssq_errm(querier));
    helper_expect_timeouts_eq(querier->timeout_recv, SSQ_TIMEOUT_RECV_DEFAULT_VALUE);
    helper_expect_timeouts_eq(querier->timeout_send, SSQ_TIMEOUT_SEND_DEFAULT_VALUE);

    ssq_free(querier);
}

Test(querier, set_timeout_recv, .description = "Setting the recvfrom timeout") {
    SSQ_QUERIER *const querier = ssq_init();

    cr_assert_neq(querier, NULL);

    const time_t timeout_value_in_ms = 1234;

    ssq_set_timeout(querier, SSQ_TIMEOUT_RECV, timeout_value_in_ms);

    helper_expect_timeouts_eq(querier->timeout_recv, timeout_value_in_ms);
    helper_expect_timeouts_eq(querier->timeout_send, SSQ_TIMEOUT_SEND_DEFAULT_VALUE);

    ssq_free(querier);
}

Test(ssq, set_timeout_send, .description = "Setting the sendto timeout") {
    SSQ_QUERIER *const querier = ssq_init();

    cr_assert_neq(querier, NULL);

    const time_t timeout_value_in_ms = 1234;

    ssq_set_timeout(querier, SSQ_TIMEOUT_SEND, timeout_value_in_ms);

    helper_expect_timeouts_eq(querier->timeout_recv, SSQ_TIMEOUT_RECV_DEFAULT_VALUE);
    helper_expect_timeouts_eq(querier->timeout_send, timeout_value_in_ms);

    ssq_free(querier);
}

Test(ssq, set_timeout_both, .description = "Setting both timeouts") {
    SSQ_QUERIER *const querier = ssq_init();

    cr_assert_neq(querier, NULL);

    const time_t timeout_value_in_ms = 1234;

    ssq_set_timeout(querier, SSQ_TIMEOUT_RECV | SSQ_TIMEOUT_SEND, timeout_value_in_ms);

    helper_expect_timeouts_eq(querier->timeout_recv, timeout_value_in_ms);
    helper_expect_timeouts_eq(querier->timeout_send, timeout_value_in_ms);

    ssq_free(querier);
}

Test(ssq, errclr, .description = "Clear error") {
    SSQ_QUERIER *const querier = ssq_init();

    cr_assert_neq(querier, NULL);

    ssq_error_set(&(querier->err), SSQ_ERR_BADRES, "Bad response");
    ssq_errclr(querier);
    cr_expect(ssq_ok(querier));
    cr_expect_str_empty(ssq_errm(querier));

    ssq_free(querier);
}
