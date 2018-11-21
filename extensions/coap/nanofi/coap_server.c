/**
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "coap_server.h"
#include "coap_functions.h"
/**
 * Create a new CoAPServer
 */
CoAPServer * const create_server(const char *const server_hostname, const char * const port){
  CoAPServer *server = (CoAPServer*)malloc(sizeof(CoAPServer));
  memset(server,0x00, sizeof(CoAPServer));
  if ( create_endpoint_context(&server->ctx,server_hostname,port) ) {
    free_server(server);
  }
  //coap_resource_t * r = coap_resource_init(NULL, 0);
  //coap_add_resource(server->ctx,r);
//  server->base_resource = coap_resource_init(NULL, 0);

  //coap_add_attr(server->base_resource, coap_make_str_const("title"), coap_make_str_const(title), 0);

  return server;
}

CoAPEndpoint *const create_endpoint(CoAPServer * const server, const char * const resource_path, uint8_t method, coap_method_handler_t handler){
  CoAPEndpoint *endpoint = (CoAPEndpoint*)malloc(sizeof(CoAPEndpoint));
  memset(endpoint,0x00, sizeof(CoAPEndpoint));
  endpoint->server = server;
  if (NULL != resource_path)
    printf("creating resource %s\n",resource_path);
  int8_t flags = COAP_RESOURCE_FLAGS_NOTIFY_CON;
  //if (resource_path == NULL)
//    flags = 0;
  endpoint->resource = coap_resource_init(resource_path == NULL ? NULL : coap_make_str_const(resource_path), flags);
  endpoint->resource->observable=1;
  coap_add_attr(endpoint->resource, coap_make_str_const("title"), coap_make_str_const("\"Internal Clock\""), 0);
  assert( !add_endpoint(endpoint,method,handler) );
  coap_add_resource(server->ctx,endpoint->resource);
  return endpoint;

}

int8_t add_endpoint(CoAPEndpoint * const endpoint, uint8_t method, coap_method_handler_t handler){
  if (endpoint == NULL || handler == NULL)
    return -1;

  coap_register_handler(endpoint->resource, method, handler);
  coap_resource_set_get_observable(endpoint->resource, 1);
  return 0;
}


/**
 * FRee the CoAP messages that are provided.
 */
void free_endpoint(CoAPEndpoint * const endpoint){
  if (endpoint){
    free((void*)endpoint);
  }
}
void free_server(CoAPServer * const server){
  if (server){
    coap_delete_all_resources( server->ctx );
    coap_free_context( server->ctx );
    free(server);
  }
}
