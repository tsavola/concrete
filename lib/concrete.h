#ifndef CONCRETE_H_INCLUDED
#define CONCRETE_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>

#ifndef CONCRETE_API
#define CONCRETE_API
#endif

#define CONCRETE_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

struct ConcreteContext;

enum ConcreteErrorType {
	CONCRETE_ERROR_NONE = 0,
	CONCRETE_ERROR_SYSTEM,
	CONCRETE_ERROR_INTEGRITY,
	CONCRETE_ERROR_ALLOCATION,
	CONCRETE_ERROR_CODE,
};

struct ConcreteError {
	enum ConcreteErrorType  type;
	const char             *message;
};

typedef struct ConcreteContext ConcreteContext;
typedef struct ConcreteError   ConcreteError;

CONCRETE_API ConcreteContext *concrete_create(ConcreteError *error);

CONCRETE_API ConcreteContext *concrete_resume(void          *data,
                                              size_t         size,
                                              ConcreteError *error);

CONCRETE_API void concrete_load(ConcreteContext *context,
                                void            *code,
                                size_t           size,
                                ConcreteError   *error);

CONCRETE_API bool concrete_execute(ConcreteContext *context,
                                   ConcreteError   *error);

CONCRETE_API void concrete_snapshot(ConcreteContext  *context,
                                    const void      **base_ptr,
                                    size_t           *size_ptr);

CONCRETE_API void concrete_destroy(ConcreteContext *context);

#ifdef __cplusplus
}
#endif

#endif
