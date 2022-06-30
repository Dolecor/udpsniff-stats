/*
 * Copyright (c) 2022 Dmitry Dolenko
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef UBUS_BINARY_HELPER_H
#define UBUS_BINARY_HELPER_H

#include <libubox/blobmsg.h>

static inline int blobmsg_add_binary(struct blob_buf *buf, const char *name,
                                     const char *data, size_t len)
{
    return blobmsg_add_field(buf, BLOBMSG_TYPE_UNSPEC, name, data, len);
}

static inline char *blobmsg_get_binary(struct blob_attr *attr, size_t *len)
{
    if (!attr)
        return NULL;

    *len = blobmsg_data_len(attr);
    return (char *)blobmsg_data(attr);
}

#endif /* UBUS_BINARY_HELPER_H */
