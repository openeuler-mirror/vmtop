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
 * Description: field setting of vmtop
 ********************************************************************************/

#include "field.h"
#include "type.h"
#include "vcpu_stat.h"
#include "proc.h"

const char *summary_text = ""
    "vmtop - %s - %s\n"
    "Domains: %d running\n";
const char *filter_help = ""
    "field filter - select which field to be showed\n"
    "Use up/down to navigate, use space to set whether chosen filed to be showed\n"
    "'q' to quit to normal display\n";
const char *help_text = ""
    "usage: vmtop [option] [arg]\n"
    "-b     :output as text, which can be used for redirections\n"
    "-d sec :set the refresh interval to secs\n"
    "-h     :print this help message and exit\n"
    "-H     :displays VM thread information\n"
    "-n num :set the number of refresh times before automatic quit\n"
    "-v     :show VMTOP version and exit\n";
const char *version_text = ""
    "vmtop-%s\n";

FID fields[] = {
#define GDF(f) (void *)GET_DELTA_NAME(f)
#define GF(f) (void *)GET_NAME(f)
    /* name  .      flag     . align */
    {"DID",      FIELDS_DISPLAY, 5,  NULL                   },
    {"VM/task-name", FIELDS_DISPLAY, 14, NULL               },
    {"PID",      FIELDS_DISPLAY, 8,  NULL                   },
    {"%CPU",     FIELDS_DISPLAY, 8,  NULL                   },
    {"EXThvc",   FIELDS_DISPLAY, 10, GDF(hvc_exit_stat)     },
    {"EXTwfe",   FIELDS_DISPLAY, 10, GDF(wfe_exit_stat)     },
    {"EXTwfi",   FIELDS_DISPLAY, 10, GDF(wfi_exit_stat)     },
    {"EXTmmioU", FIELDS_DISPLAY, 10, GDF(mmio_exit_user)    },
    {"EXTmmioK", FIELDS_DISPLAY, 10, GDF(mmio_exit_kernel)  },
    {"EXTfp",    FIELDS_DISPLAY, 10, GDF(fp_asimd_exit_stat)},
    {"EXTirq",   FIELDS_DISPLAY, 10, GDF(irq_exit_stat)     },
    {"EXTsys64", FIELDS_DISPLAY, 10, GDF(sys64_exit_stat)   },
    {"EXTmabt",  FIELDS_DISPLAY, 10, GDF(mabt_exit_stat)    },
    {"EXTsum",   FIELDS_DISPLAY, 10, GDF(exits)             },
    {"S",        FIELDS_DISPLAY, 5,  GF(state)              },
    {"P",        FIELDS_DISPLAY, 5,  GF(processor)          },
    {"%ST",      FIELDS_DISPLAY, 8,  GDF(steal)             },
    {"%GUE",     FIELDS_DISPLAY, 8,  GDF(gtime)             },
    {"%HYP",     FIELDS_DISPLAY, 8,  NULL}
#undef GDF
};

int get_show_field_num(void)
{
    int sum = 0;

    for (int i = 0; i < FD_END; i++) {
        if (fields[i].display_flag == 1) {
            sum++;
        }
    }
    return sum;
}
