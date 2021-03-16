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
 * Description: get procfs data
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include "type.h"
#include "proc.h"
#include "utils.h"

#define STAT_PATH_SIZE 40

struct file_item proc_stab[] = {
#define GDF(f)   NULL, (void *)DELTA_NAME(f), NULL
    {"%llu", GDF(utime)},
    {"%llu", GDF(stime)}
#undef GDF
};

const int stat_size = sizeof(proc_stab) / sizeof(struct file_item);

int get_proc_stat(struct domain *dom)
{
    char buf[BUF_SIZE];
    char path[STAT_PATH_SIZE];
    char *tmp1 = NULL;
    char *tmp2 = NULL;
    int len;

    if (dom->type == ISDOMAIN) {
        if (snprintf(path, STAT_PATH_SIZE, "/proc/%u/stat", dom->pid) < 0) {
            return -1;
        }
    } else {
         if (snprintf(path, STAT_PATH_SIZE, "/proc/%u/task/%u/stat",
                      dom->ppid, dom->pid) < 0) {
            return -1;
        }
    }
    if (read_file(buf, BUF_SIZE, path) < 0) {
        return -1;
    }

    /* read comm from "pid (comm) S" */
    tmp1 = strrchr(buf, '(') + 1;
    tmp2 = strrchr(buf, ')');
    len = tmp2 - tmp1;
    if (len >= DOMAIN_NAME_MAX || len < 0) {
        return -1;
    }
    strncpy(dom->vmname, tmp1, len);
    dom->vmname[len] = '\0';

    /* read start from process state */
    sscanf(tmp2 + 2,
           "%c "
           "%d %d %*d %*d %*d "
           "%lu "
           "%lu %lu %lu %lu "
           "%llu %llu %llu %llu "
           "%*d %*d "
           "%d %ld "
           "%llu %lu %ld %lu "
           "%*u %*u %*u %*u %*u "
           "%*s %*s %*s %*s "
           "%*u %*u %*u %*d "
           "%d",
           &(dom->state),
           &(dom->ppid), &(dom->pgrd),
           &(dom->flags),
           &(dom->min_flt), &(dom->cmin_flt), &(dom->maj_flt), &(dom->cmaj_flt),
           &(dom->utime), &(dom->stime), &(dom->cutime), &(dom->cstime),
           &(dom->nlwp), &(dom->alarm),
           &(dom->start_time), &(dom->vsize), &(dom->rss), &(dom->rss_rlim),
           &(dom->processor));
    return 1;
}

void refresh_delta_stat(struct domain *new, struct domain *old)
{
    for (int i = 0; i < stat_size; i++) {
        if (proc_stab[i].delta_fun) {
            (*proc_stab[i].delta_fun)(new, old);
        }
    }
}
