#include "ssq/timeout.h"

#include <stdint.h>

#ifdef _WIN32
# include <windows.h>
#else /* !_WIN32 */
# include <sys/time.h>
#endif /* _WIN32 */

#include "timeout.h"

void ssq_timeout_set(
    struct ssq_timeout       *const timeout,
    const enum ssq_timeout_selector which,
    const ssq_milliseconds          duration
) {
#ifdef _WIN32
    if (which & SSQ_TIMEOUT_RECV)
        timeout->recv = duration;
    if (which & SSQ_TIMEOUT_SEND)
        timeout->send = duration;
#else /* !_WIN32 */
    const struct timeval duration_tv = {
        .tv_sec  = duration / 1000,
        .tv_usec = duration % 1000 * 1000,
    };
    if (which & SSQ_TIMEOUT_RECV)
        timeout->recv = duration_tv;
    if (which & SSQ_TIMEOUT_SEND)
        timeout->send = duration_tv;
#endif /* _WIN32 */
}
