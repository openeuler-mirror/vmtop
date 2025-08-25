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

#ifndef SRC_resctrl_H
#define SRC_resctrl_H

#include <stdbool.h>
#include "domain.h"

#define SYSINFO_PATH            "/var/run/sysinfo"
#define VMTOP_PATH              "/var/run/sysinfo/vmtop"
#define VMTOP_INFO_PATH         "/var/run/sysinfo/vmtop/vmtop_info"
#define VMTOP_INFO_BK_PATH      "/var/run/sysinfo/vmtop/vmtop_info_bak"
#define RESCTRL_FILESYSTEM_DIR  "/sys/fs/resctrl"
#define RESCTRL_MON_GROUPS_DIR  "/sys/fs/resctrl/mon_groups"
#define CTL_GROUP_PREFIX        "qemu-"
#define MON_DATA_DIR            "mon_data"
#define MON_DATA_PREFIX         "mon_MB_"
#define MON_DATA_FILE           "mbm_total_bytes"
#define VMTOP_GROUP_NAME_MAX     512
#define MAX_VM_TASKS_NUM         1024
#define MAX_VM_NUM               512
#define MAX_RESCTRL_VAL_LEN      32
#define MAX_RESCTRL_KEY_LEN      32
#define MAX_BANDWIDTH_LENGTH     24
#define SAMPLE_TIMES_INNER       2
#define SAMPLE_TIMES_OUTER       3
#define SAMPLE_INTERVAL_US_INNER 1000
#define SAMPLE_INTERVAL_US_OUTER 5000
#define SAMPLE_START             0

bool enable_resctrl(void);
int get_vm_ctl_bandwidth(struct domain *dom, int node_num);
int get_root_mon_bandwidth(long int *bandwidth, int node_num);
int write_resctrl_data_into_logfile(struct domain_list *scr_cur);
int get_numa_num(int *node_num);
#endif