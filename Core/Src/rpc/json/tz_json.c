#include "tz_json.h"

RPCResult_t json_parse(json_t const *json, const RPCParameter_t *param,
                       void *base_ptr) {
  if (!param)
    return RPC_ERR_PARAM_NULL;
  if (!json)
    return RPC_OK; // Optional field missing in JSON -> OK, just skip

  // Calculate the exact memory address for this field
  // If base_ptr is NULL, we assume the offset IS the absolute address (not used
  // here but good for safety)
  void *ptr = (char *)base_ptr + param->offset;

  switch (param->type) {
  // --- Signed Integers ---
  case RPC_INT8:
    *((int8_t *)ptr) = (int8_t)json_getInteger(json);
    break;
  case RPC_INT16:
    *((int16_t *)ptr) = (int16_t)json_getInteger(json);
    break;
  case RPC_INT32:
    *((int32_t *)ptr) = (int32_t)json_getInteger(json);
    break;
  case RPC_INT64:
    *((int64_t *)ptr) = (int64_t)json_getInteger(json);
    break;

  // --- Unsigned Integers ---
  case RPC_UINT8:
    *((uint8_t *)ptr) = (uint8_t)json_getInteger(json);
    break;
  case RPC_UINT16:
    *((uint16_t *)ptr) = (uint16_t)json_getInteger(json);
    break;
  case RPC_UINT32:
    *((uint32_t *)ptr) = (uint32_t)json_getInteger(json);
    break;
  case RPC_UINT64:
    *((uint64_t *)ptr) = (uint64_t)json_getInteger(json);
    break;

  // --- Other Primitives ---
  case RPC_DOUBLE:
    *((double *)ptr) = json_getReal(json);
    break;
  case RPC_BOOLEAN:
    *((bool *)ptr) = json_getBoolean(json);
    break;

  case RPC_STRING: {
    const char *val = json_getValue(json);
    if (val) {
      strncpy((char *)ptr, val, param->count);
      ((char *)ptr)[param->count - 1] = '\0';
    }
    break;
  }

  // --- Recursive Types ---
  case RPC_OBJECT: {
    if (json_getType(json) != JSON_OBJ)
      return RPC_ERR_TYPE_MISMATCH;

    const RPCParameter_t *field = param->sub.obj_fields;
    // Iterate over the schema array
    while (field && field->name != NULL) {
      json_t const *prop = json_getProperty(json, field->name);
      if (prop) {
        RPCResult_t res = json_parse(prop, field, ptr);
        if (res != RPC_OK)
          return res;
      }
      field++;
    }
    break;
  }

  case RPC_ARRAY: {
    if (json_getType(json) != JSON_ARRAY)
      return RPC_ERR_TYPE_MISMATCH;

    json_t const *child = json_getChild(json);
    size_t i = 0;
    // Iterate JSON elements
    while (child != 0 && i < param->count) {
      // Address = Base + (Index * Stride)
      void *elem_ptr = (char *)ptr + (i * param->size);

      // Recurse for the element
      // Note: The element definition has offset 0, so it uses elem_ptr as base
      RPCResult_t res = json_parse(child, param->sub.arr_elem, elem_ptr);
      if (res != RPC_OK)
        return res;

      child = json_getSibling(child);
      i++;
    }
    break;
  }
  default:
    return RPC_ERR_UNKNOWN_TYPE;
  }
  return RPC_OK;
}

// --- Internal Recursive Dumper ---

static char *json_dump_internal(char *dest, const RPCParameter_t *param,
                                const void *base_ptr, size_t *remLen) {
  // If base_ptr is NULL, we assume the offset IS the address
  // But usually base_ptr is struct start, and we add offset.
  const void *ptr = (const char *)base_ptr + param->offset;
  const char *name = param->name;

  switch (param->type) {
  // --- Signed Integers ---
  case RPC_INT8:
    return json_int(dest, name, *((const int8_t *)ptr), remLen);
  case RPC_INT16:
    return json_int(dest, name, *((const int16_t *)ptr), remLen);
  case RPC_INT32:
    return json_int(dest, name, *((const int32_t *)ptr), remLen);
  case RPC_INT64:
    return json_verylong(dest, name, *((const int64_t *)ptr), remLen);

  // --- Unsigned Integers ---
  case RPC_UINT8:
    return json_uint(dest, name, *((const uint8_t *)ptr), remLen);
  case RPC_UINT16:
    return json_uint(dest, name, *((const uint16_t *)ptr), remLen);
  case RPC_UINT32:
    return json_uint(dest, name, *((const uint32_t *)ptr), remLen);
  case RPC_UINT64:
    // Cast to long long as json-maker doesn't have unsigned verylong
    return json_verylong(dest, name, (long long)*((const uint64_t *)ptr),
                         remLen);

  // --- Other Primitives ---
  case RPC_DOUBLE:
    return json_double(dest, name, *((const double *)ptr), remLen);
  case RPC_BOOLEAN:
    return json_bool(dest, name, *((const bool *)ptr), remLen);

  case RPC_STRING: {
    // Strings are char arrays in the struct, so ptr points to the first char
    return json_str(dest, name, (const char *)ptr, remLen);
  }

  // --- Recursive Types ---
  case RPC_OBJECT: {
    dest = json_objOpen(dest, name, remLen);
    const RPCParameter_t *field = param->sub.obj_fields;
    // Iterate over the schema array
    while (field && field->name != NULL) {
      dest = json_dump_internal(dest, field, ptr, remLen);
      field++;
    }
    return json_objClose(dest, remLen);
  }

  case RPC_ARRAY: {
    dest = json_arrOpen(dest, name, remLen);
    size_t i = 0;
    // Iterate elements
    while (i < param->count) {
      // Address = Base + (Index * Stride)
      const void *elem_ptr = (const char *)ptr + (i * param->size);

      // Recurse for the element
      // The element definition usually has name=NULL, which is what we want for
      // array items
      dest = json_dump_internal(dest, param->sub.arr_elem, elem_ptr, remLen);
      i++;
    }
    return json_arrClose(dest, remLen);
  }
  default:
    return dest; // Skip unknown
  }
}

int json_dump(const void *base_ptr, const RPCParameter_t *param, char *dest,
              size_t max_len) {
  if (!base_ptr || !param || !dest || max_len == 0)
    return -1;

  size_t remLen = max_len;
  char *p = dest;

  p = json_dump_internal(p, param, base_ptr, &remLen);
  p = json_end(p, &remLen); // Finalize (remove trailing comma)

  if (remLen == 0)
    return -1; // Overflow or exact fit without null terminator space (check
               // json-maker behavior)

  return (int)(p - dest);
}
