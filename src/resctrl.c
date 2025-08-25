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
 * Description: get domain's data
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#include "utils.h"
#include "resctrl.h"

bool enable_resctrl(void)
{
    int ret = -1;
    ret = access(RESCTRL_MON_GROUPS_DIR, F_OK);
    if (ret != 0) {
        return false;
    }
    return true;
}

int get_numa_num(int *node_num)
{
    DIR *dirptr = NULL;
    struct dirent *dirt = NULL;
    int cur_node = -1;
    int max_node = -1;
    char path[VMTOP_GROUP_NAME_MAX] = {0};
    int ret = -1;

    ret = snprintf(path, VMTOP_GROUP_NAME_MAX, "%s/%s",
                   RESCTRL_FILESYSTEM_DIR, MON_DATA_DIR);
    if (ret <= 0) {
        return ret;
    }

    if ((dirptr = opendir(path)) == NULL) {
        return -1;
    }

    while ((dirt = readdir(dirptr)) != NULL) {
        if (strncmp(dirt->d_name, MON_DATA_PREFIX, strlen(MON_DATA_PREFIX)) == 0) {
            cur_node = atoi(dirt->d_name + strlen(MON_DATA_PREFIX));
            if (cur_node < 0 || cur_node >= MAX_NODE_NUM) {
                continue;
            }
            max_node = cur_node > max_node ? cur_node : max_node;
        }
    }
    closedir(dirptr);
    *node_num = max_node + 1;
    return max_node;
}

static long get_mon_MB_once(FILE *fp) {
    char bandwidth_str[MAX_BANDWIDTH_LENGTH] = {0};

    (void)fflush(fp);
    (void)fseek(fp, SAMPLE_START, SEEK_SET);
    if (fgets(bandwidth_str, MAX_BANDWIDTH_LENGTH, fp) != NULL) {
        return atol(bandwidth_str);
    }
    return -1;
}

static int get_mon_MB(long int *bandwidth, char *path, int node_num) {
    FILE *fps[MAX_NODE_NUM] = {0};
    int sample_times[MAX_NODE_NUM] = {0};
    long bandwidth_sum[MAX_NODE_NUM] = {0};
    char mon_data_path[VMTOP_GROUP_NAME_MAX] = {0};
    long bandwidth_tmp;
    int i, node;
    int ret = -1;

    for (node = 0; node < node_num; node++) {
        if (snprintf(mon_data_path, VMTOP_GROUP_NAME_MAX,
                     "%s/%s%02d/%s", path, MON_DATA_PREFIX, node, MON_DATA_FILE) < 0) {
            goto out;
        }
        fps[node] = fopen(mon_data_path, "r");
        if (fps[node] == NULL) {
            goto out;
        }
    }
    for (i = 0; i < SAMPLE_TIMES_INNER; i++) {
        for (node = 0; node < node_num; node++) {
            bandwidth_tmp = get_mon_MB_once(fps[node]);
            if (bandwidth_tmp != -1 && i > 0) {
                bandwidth_sum[node] += bandwidth_tmp;
                sample_times[node]++;
            }
        }
        (void)usleep(SAMPLE_INTERVAL_US_INNER);
    }
    for (i = 0; i < node_num; i++) {
        if (!sample_times[i]) {
            goto out;
        }
        bandwidth[i] += (bandwidth_sum[i] / sample_times[i]);
    }
    ret = 0;
out:
    for (node = 0; node < node_num; node++) {
        if (fps[node] != NULL) {
            fclose(fps[node]);
        }
    }
    return ret;
}

/* 
 * collect root monitor group bandwidth from
 * /sys/fs/resctrl/mon_data
 */
int get_root_mon_bandwidth(long int *bandwidth, int node_num)
{
    char path[VMTOP_GROUP_NAME_MAX] = {0};
    int ret = -1;

    ret = snprintf(path, VMTOP_GROUP_NAME_MAX, "%s/%s", RESCTRL_FILESYSTEM_DIR, MON_DATA_DIR);
    if (ret < 0) {
        return ret;
    }

    ret = get_mon_MB(bandwidth, path, node_num);
    if (ret < 0) {
        return ret;
    }

    return 0;
}

static int get_vm_ctl_path(int domain_id, char *vmname, char *path)
{
    DIR *dir;
    struct dirent *ent;
    char vm_ctl_prefix[VMTOP_GROUP_NAME_MAX] = {0};
    int found = 0;
    int ret = -1;

    if ((dir = opendir(RESCTRL_FILESYSTEM_DIR)) == NULL) {
        return -1;
    }

    ret = snprintf(vm_ctl_prefix, VMTOP_GROUP_NAME_MAX, "%s%d",
        CTL_GROUP_PREFIX, domain_id);
    if (ret < 0) {
        goto out;
    }
    while ((ent = readdir(dir)) != NULL) {
        if (strncmp(ent->d_name, vm_ctl_prefix, strlen(vm_ctl_prefix)) == 0) {
            ret = snprintf(path, VMTOP_GROUP_NAME_MAX, "%s/%s/%s",
                RESCTRL_FILESYSTEM_DIR, ent->d_name, MON_DATA_DIR);
            if (ret < 0) {
                goto out;
            }
            found = 1;
            break;
        }
    }
out:
    closedir(dir);
    if (found) {
        return 0;
    } else {
        return -1;
    }
}

/* 
 * collect vm control group bandwidth from
 * /sys/fs/resctrl/qemu-${domain_id}-${vmname}-xxx
 */
int get_vm_ctl_bandwidth(struct domain *dom, int node_num)
{
    char path[VMTOP_GROUP_NAME_MAX] = {0};
    int ret = -1;

    ret = get_vm_ctl_path(dom->domain_id, dom->vmname, path);
    if (ret < 0) {
        return -1;
    }
    ret = get_mon_MB(dom->ctl_bandwidth, path, node_num);
    if (ret < 0) {
        return -1;
    }
    return 0;
}