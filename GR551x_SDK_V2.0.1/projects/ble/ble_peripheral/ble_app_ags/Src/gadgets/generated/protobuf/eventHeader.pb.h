/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.2 */

#ifndef PB_HEADER_EVENTHEADER_PB_H_INCLUDED
#define PB_HEADER_EVENTHEADER_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Struct definitions */
typedef struct _header_EventHeaderProto {
    char namespace[32];
    char name[32];
    char messageId[32];
} header_EventHeaderProto;


/* Initializer values for message structs */
#define header_EventHeaderProto_init_default     {"", "", ""}
#define header_EventHeaderProto_init_zero        {"", "", ""}

/* Field tags (for use in manual encoding/decoding) */
#define header_EventHeaderProto_namespace_tag    1
#define header_EventHeaderProto_name_tag         2
#define header_EventHeaderProto_messageId_tag    3

/* Struct field encoding specification for nanopb */
#define header_EventHeaderProto_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   namespace,         1) \
X(a, STATIC,   SINGULAR, STRING,   name,              2) \
X(a, STATIC,   SINGULAR, STRING,   messageId,         3)
#define header_EventHeaderProto_CALLBACK NULL
#define header_EventHeaderProto_DEFAULT NULL

extern const pb_msgdesc_t header_EventHeaderProto_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define header_EventHeaderProto_fields &header_EventHeaderProto_msg

/* Maximum encoded size of messages (where known) */
#define header_EventHeaderProto_size             99

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
