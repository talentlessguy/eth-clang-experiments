#ifndef JSON_RPC_H
#define JSON_RPC_H

#include <cjson/cJSON.h>

char *send_json_rpc_request(const char *url, const char *method, cJSON *params);

#endif /* JSON_RPC_H */
