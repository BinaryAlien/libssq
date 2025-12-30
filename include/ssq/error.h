#ifndef SSQ_ERROR_H
#define SSQ_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum ssq_error_code {
  SSQE_OK = 0,
  SSQE_SYSTEM,
  SSQE_GAI,
  SSQE_MUTE,
  SSQE_BAD,
  SSQE_UNSUPPORTED,
};

struct ssq_error;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_ERROR_H */
