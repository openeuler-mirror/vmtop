/******************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * vmtop licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *     http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Description: utils function
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include "utils.h"

int read_file(char *buf, int bufsize, const char *path)
{
    int fd;
    int len;
    char mpath[PATH_MAX];

    if (strlen(path) > PATH_MAX - 1 || realpath(path, mpath) == NULL) {
        return -1;
    }
    fd = open(mpath, O_RDONLY, 0);
    if (fd == -1) {
        return -1;
    }
    len = read(fd, buf, bufsize - 1);
    if (len > 0) {
        buf[len] = '\0';
    }
    close(fd);
    return len;
}

int get_time_str(char *buf, int bufsize)
{
    struct tm *tm_ptr;
    time_t now;

    time(&now);
    tm_ptr = localtime(&now);
    if (tm_ptr == NULL) {
        return -1;
    }
    if (strftime(buf, bufsize, "%Y-%m-%d %H:%M:%S", tm_ptr) == 0) {
        return -1;
    }
    return 1;
}
