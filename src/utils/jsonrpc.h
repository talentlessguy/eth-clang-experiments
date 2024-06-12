#ifndef JSON_RPC_H
#define JSON_RPC_H

#include <json-glib/json-glib.h>

char *send_json_rpc_request(const char *url, const char *method, JsonNode *params);

#endif /* JSON_RPC_H */
