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
#define GDF(f)   (void *)GET_NAME(f), (void *)DELTA_NAME(f), NULL
#define GF(f)    (void *)GET_NAME(f), NULL, NULL
    {"%c", GF(state)},
    {"%d", GF(ppid)},
    {"%d", GF(pgrd)},
    {"%d", GF(session)},
    {"%d", GF(tty)},
    {"%d", GF(tpgid)},
    {"%lu", GF(flags)},
    {"%lu", GF(min_flt)},
    {"%lu", GF(cmin_flt)},
    {"%lu", GF(maj_flt)},
    {"%lu", GF(cmaj_flt)},
    {"%llu", GDF(utime)},
    {"%llu", GDF(stime)},
    {"%llu", GF(cutime)},
    {"%llu", GF(cstime)},
    {"%ld", GF(priority)},
    {"%ld", GF(nice)},
    {"%d", GF(nlwp)},
    {"%ld", GF(alarm)},
    {"%llu", GF(start_time)},
    {"%lu", GF(vsize)},
    {"%ld", GF(rss)},
    {"%lu", GF(rss_rlim)},
    {"%lu", GF(start_code)},
    {"%lu", GF(end_code)},
    {"%lu", GF(start_stack)},
    {"%lu", GF(kstk_esp)},
    {"%lu", GF(kstk_eip)},
    {"%*s", NULL, NULL, NULL},    /* discard signal */
    {"%*s", NULL, NULL, NULL},    /* discard blocked */
    {"%*s", NULL, NULL, NULL},    /* discard sigignore */
    {"%*s", NULL, NULL, NULL},    /* discard sigcatch */
    {"%lu", GF(wchan)},
    {"%*u", NULL, NULL, NULL},    /* dsicard nswap */
    {"%*u", NULL, NULL, NULL},    /* discard cnswap */
    {"%d", GF(exit_signal)},
    {"%d", GF(processor)},
    {"%lu", GF(rtprio)},
    {"%lu", GF(sched)}
#undef GF
#undef GDF
};

const int stat_size = sizeof(proc_stab) / sizeof(struct file_item);

int get_proc_stat(struct domain *dom)
{
    char buf[BUF_SIZE];
    char path[STAT_PATH_SIZE];
    char *tmp = NULL;
    char *p = NULL;
    char *p_next = NULL;
    int i = 0;

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

    /* read from state item of "...) S ..." */
    tmp = strrchr(buf, ')');
    tmp = tmp + 2;

    for (p = strtok_r(tmp, " \t\r\n", &p_next); p && i < stat_size;
         p = strtok_r(NULL, " \t\r\n", &p_next)) {
        if (proc_stab[i].get_fun != NULL) {
            sscanf(p, proc_stab[i].format, (*proc_stab[i].get_fun)(dom));
        }
        ++i;
    }
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

int get_proc_comm(struct domain *dom)
{
    char path[STAT_PATH_SIZE];
    int len;

    if (snprintf(path, STAT_PATH_SIZE, "/proc/%u/comm", dom->pid) < 0) {
        return -1;
    }

    len = read_file(dom->vmname, DOMAIN_NAME_MAX, path);
    if (len > 1) {
        dom->vmname[len - 1] = '\0';
    }
    return len;
}
