#include "externals/json-maker.h"
#include "externals/tiny-json.h"
#include <stdbool.h>
#include <stddef.h> // For offsetof
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Type Definitions ---

typedef enum {
  RPC_INT8,
  RPC_UINT8,
  RPC_INT16,
  RPC_UINT16,
  RPC_INT32,
  RPC_UINT32,
  RPC_INT64,
  RPC_UINT64,
  RPC_DOUBLE,
  RPC_BOOLEAN,
  RPC_STRING,
  RPC_OBJECT,
  RPC_ARRAY
} RPCParamType_t;

typedef enum {
  RPC_OK = 0,
  RPC_ERR_PARAM_NULL = -1,
  RPC_ERR_JSON_NULL = -2,
  RPC_ERR_TYPE_MISMATCH = -3,
  RPC_ERR_UNKNOWN_TYPE = -4,
  RPC_ERR_BUFFER_OVERFLOW = -5
} RPCResult_t;

// Forward declaration
typedef struct RPCParameter RPCParameter_t;

struct RPCParameter {
  const char *name; // JSON Key. (NULL if it's an array element)
  RPCParamType_t type;
  size_t offset; // Offset from the Base Pointer
  size_t size;   // The size of this specific element (Stride)
  size_t count;  // For Arrays/Strings: Number of elements

  // Recursive Definitions
  union {
    const RPCParameter_t *obj_fields; // If OBJECT: Pointer to array of fields
    const RPCParameter_t *arr_elem;   // If ARRAY: Pointer to single element def
  } sub;
};

// --- Macros (FIXED) ---

// 1. Generic Field Helper
// This does the heavy lifting of calculating offsets
#define _FIELD(_name, _type, _struct, _member, _count, _fields, _elem)         \
  {.name = _name,                                                              \
   .type = _type,                                                              \
   .offset = offsetof(_struct, _member),                                       \
   .size = sizeof(((_struct *)0)->_member),                                    \
   .count = _count,                                                            \
   .sub.obj_fields = _fields}

// 2. Primitive Macros
#define R_INT8(n, st, m) _FIELD(n, RPC_INT8, st, m, 0, NULL, NULL)
#define R_INT16(n, st, m) _FIELD(n, RPC_INT16, st, m, 0, NULL, NULL)
#define R_INT32(n, st, m) _FIELD(n, RPC_INT32, st, m, 0, NULL, NULL)
#define R_INT64(n, st, m) _FIELD(n, RPC_INT64, st, m, 0, NULL, NULL)

#define R_UINT8(n, st, m) _FIELD(n, RPC_UINT8, st, m, 0, NULL, NULL)
#define R_UINT16(n, st, m) _FIELD(n, RPC_UINT16, st, m, 0, NULL, NULL)
#define R_UINT32(n, st, m) _FIELD(n, RPC_UINT32, st, m, 0, NULL, NULL)
#define R_UINT64(n, st, m) _FIELD(n, RPC_UINT64, st, m, 0, NULL, NULL)

#define R_DOUBLE(n, st, m) _FIELD(n, RPC_DOUBLE, st, m, 0, NULL, NULL)
#define R_BOOL(n, st, m) _FIELD(n, RPC_BOOLEAN, st, m, 0, NULL, NULL)

// FIX: Used 'st' and 'm' correctly here instead of '_struct' and '_member'
#define R_STR(n, st, m)                                                        \
  _FIELD(n, RPC_STRING, st, m, sizeof(((st *)0)->m), NULL, NULL)

// 3. Object Macro
#define R_OBJECT(n, st, m, schema) _FIELD(n, RPC_OBJECT, st, m, 0, schema, NULL)

// 4. Array Macros
// Helper to calculate array capacity: sizeof(arr) / sizeof(arr[0])
#define _ARR_COUNT(st, m) (sizeof(((st *)0)->m) / sizeof(((st *)0)->m[0]))
// Helper to get element size: sizeof(arr[0])
#define _ELEM_SIZE(st, m) sizeof(((st *)0)->m[0])

// Array of Primitives
#define R_ARRAY_INT(n, st, m, type_enum)                                       \
  {                                                                            \
    .name = n, .type = RPC_ARRAY, .offset = offsetof(st, m),                   \
    .size = _ELEM_SIZE(st, m), .count = _ARR_COUNT(st, m),                     \
    .sub.arr_elem = &(RPCParameter_t) {                                        \
      .type = type_enum, .offset = 0, .count = 0                               \
    }                                                                          \
  }

// Array of Objects
#define R_ARRAY_OBJ(n, st, m, schema)                                          \
  {                                                                            \
    .name = n, .type = RPC_ARRAY, .offset = offsetof(st, m),                   \
    .size = _ELEM_SIZE(st, m), .count = _ARR_COUNT(st, m),                     \
    .sub.arr_elem = &(RPCParameter_t) {                                        \
      .type = RPC_OBJECT, .offset = 0, .sub.obj_fields = schema                \
    }                                                                          \
  }

// 5. Array of Arrays (Multi-dimensional)
// For int matrix[ROWS][COLS], sub_count is COLS, sub_stride is sizeof(int),
// sub_type is INT
#define R_ARRAY_ARRAY(n, st, m, sub_count, sub_stride, sub_type_enum)          \
  {                                                                            \
    .name = n, .type = RPC_ARRAY, .offset = offsetof(st, m),                   \
    .size = _ELEM_SIZE(st, m),      /* Stride of Outer Array (Row Size) */     \
        .count = _ARR_COUNT(st, m), /* Number of Rows */                       \
        .sub.arr_elem = &(RPCParameter_t) {                                    \
      .type = RPC_ARRAY, .offset = 0,                                          \
      .size = sub_stride,     /* Stride of Inner Array Element (Col Size) */   \
          .count = sub_count, /* Number of Cols */                             \
          .sub.arr_elem = &(RPCParameter_t) {                                  \
        .type = sub_type_enum, .offset = 0                                     \
      }                                                                        \
    }                                                                          \
  }

#define END_SCHEMA {.name = NULL}

RPCResult_t json_parse(json_t const *json, const RPCParameter_t *param,
                       void *base_ptr);

/**
 * @brief Serializes a C structure into a JSON string based on the provided
 * schema.
 *
 * @param base_ptr Pointer to the base of the C structure to serialize.
 * @param param    Pointer to the root RPCParameter_t schema definition.
 * @param dest     Destination buffer for the JSON string.
 * @param max_len  Maximum size of the destination buffer.
 * @return         The number of bytes written (excluding null terminator), or
 * -1 on error.
 */
int json_dump(const void *base_ptr, const RPCParameter_t *param, char *dest,
              size_t max_len);