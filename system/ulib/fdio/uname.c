// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <string.h>
#include <unistd.h>

#include <zircon/device/ioctl.h>
#include <zircon/device/ioctl-wrapper.h>
#include <zircon/types.h>

// IOCTL implemented by Fuchsia's netstack.
#define IOCTL_NETC_GET_NODENAME \
    IOCTL(IOCTL_KIND_DEFAULT, IOCTL_FAMILY_NETCONFIG, 8)

IOCTL_WRAPPER_VAROUT(ioctl_netc_get_nodename, IOCTL_NETC_GET_NODENAME, char);

__EXPORT
int uname(struct utsname* uts) {
    if (!uts) {
        errno = EFAULT;
        return -1;
    }
    *uts = (struct utsname){
        .sysname = "Fuchsia",
        .nodename = "",
        .release = "",
        .version = "",
#if defined(__x86_64__)
        .machine = "x86_64",
#elif defined(__aarch64__)
        .machine = "aarch64",
#else
        .machine = "",
#endif
    };
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    ioctl_netc_get_nodename(s, uts->nodename, sizeof(uts->nodename));
    if (!uts->nodename[0])
        strcpy(uts->nodename, "fuchsia");
    close(s);
    return 0;
}
