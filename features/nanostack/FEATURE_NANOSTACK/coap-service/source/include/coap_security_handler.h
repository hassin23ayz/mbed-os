/*
 * Copyright (c) 2015-2016 ARM Limited. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __COAP_SECURITY_HANDLER_H__
#define __COAP_SECURITY_HANDLER_H__

#include "ns_types.h"

#ifdef NS_USE_EXTERNAL_MBED_TLS
#include "mbedtls/ssl.h"
#ifdef MBEDTLS_SSL_TLS_C
#define COAP_SECURITY_AVAILABLE
#endif
#endif

#define COOKIE_SIMPLE_LEN 8
typedef struct simple_cookie {
    unsigned char value[COOKIE_SIMPLE_LEN];
    size_t        len;
} simple_cookie_t;

#define KEY_BLOCK_LEN 40
typedef struct key_block {
    unsigned char value[KEY_BLOCK_LEN];
} key_block_t;

typedef int send_cb(int8_t socket_id, void *handle, const void *buf, size_t);
typedef int receive_cb(int8_t socket_id, unsigned char *, size_t);
typedef void start_timer_cb(int8_t timer_id, uint32_t min, uint32_t fin);
typedef int timer_status_cb(int8_t timer_id);

#define DTLS_HANDSHAKE_TIMEOUT_MIN 25000
#define DTLS_HANDSHAKE_TIMEOUT_MAX 201000

typedef enum {
    DTLS = 0,
    TLS = 1
}SecureSocketMode;

typedef enum {
    Certificate,
    PSK,
    ECJPAKE
}SecureConnectionMode;

typedef struct {
    unsigned char *_server_cert;
    uint8_t _server_cert_len;
    unsigned char *_pub_cert_or_identifier;
    uint8_t _pub_len;
    unsigned char *_priv;
    uint8_t _priv_len;
} coap_security_keys_t;

typedef struct coap_security_s coap_security_t;

#ifdef COAP_SECURITY_AVAILABLE

coap_security_t *coap_security_create(int8_t socket_id, int8_t timer_id, void *handle,
                                          SecureConnectionMode mode,
                                          send_cb *send_cb,
                                          receive_cb *receive_cb,
                                          start_timer_cb *start_timer_cb,
                                          timer_status_cb *timer_status_cb);

void coap_security_destroy(coap_security_t *sec);

int coap_security_handler_connect(coap_security_t *sec, bool is_server, SecureSocketMode sock_mode, coap_security_keys_t keys);

int coap_security_handler_connect_non_blocking(coap_security_t *sec, bool is_server, SecureSocketMode sock_mode, coap_security_keys_t keys, uint32_t timeout_min, uint32_t timeout_max);

int coap_security_handler_continue_connecting(coap_security_t *sec);

int coap_security_handler_send_message(coap_security_t *sec, unsigned char *message, size_t len);

int coap_security_send_close_alert(coap_security_t *sec);

int coap_security_handler_read(coap_security_t *sec, unsigned char* buffer, size_t len);

bool coap_security_handler_is_started(const coap_security_t *sec);

const void *coap_security_handler_keyblock(const coap_security_t *sec);

#else

NS_DUMMY_DEFINITIONS_OK

/* Dummy definitions, including needed error codes */
#define MBEDTLS_ERR_SSL_TIMEOUT (-1)
#define MBEDTLS_ERR_SSL_WANT_READ (-2)
#define MBEDTLS_ERR_SSL_WANT_WRITE (-3)
#define MBEDTLS_ERR_SSL_UNEXPECTED_MESSAGE (-4)

#define coap_security_create(socket_id, timer_id, handle, \
                             mode, send_cb, receive_cb, start_timer_cb, timer_status_cb) ((coap_security_t *) 0)
#define coap_security_destroy(sec) ((void) 0)
#define coap_security_handler_connect(sec, is_server, sock_mode, keys) (-1)
#define coap_security_handler_connect_non_blocking(sec, is_server, sock_mode, keys, timeout_min, timeout_max) (-1)
#define coap_security_handler_continue_connecting(sec) (-1)
#define coap_security_handler_send_message(sec, message, len) (-1)
#define coap_security_send_close_alert(sec) (-1)
#define coap_security_handler_read(sec, buffer, len) (-1)
#define coap_security_handler_is_started(sec) false
#define coap_security_handler_keyblock(sec) ((void *) 0)

#endif /* COAP_SECURITY_AVAILABLE */

#endif
