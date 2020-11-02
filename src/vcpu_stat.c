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
 * Description: get kvm exit data from vcpu_stat
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include "type.h"
#include "vcpu_stat.h"
#include "domain.h"

#define KVM_VCPU_STAT_PATH "/sys/kernel/debug/kvm/vcpu_stat"

struct file_item vcpu_stat_stab[] = {
#define GDF(f)  (void *)GET_NAME(f), (void *)DELTA_NAME(f), (void *)SUM_NAME(f)
#define GF(f)   (void *)GET_NAME(f), NULL, NULL
    {"%u",   GF(pid)                      },
    {"%llu", GDF(hvc_exit_stat)           },
    {"%llu", GDF(wfe_exit_stat)           },
    {"%llu", GDF(wfi_exit_stat)           },
    {"%llu", GDF(mmio_exit_user)          },
    {"%llu", GDF(mmio_exit_kernel)        },
    {"%llu", GDF(exits)                   },
    {"%llu", GDF(fp_asimd_exit_stat)      },
    {"%llu", GDF(irq_exit_stat)           },
    {"%llu", GDF(sys64_exit_stat)         },
    {"%llu", GDF(mabt_exit_stat)          },
    {"%llu", GDF(fail_entry_exit_stat)    },
    {"%llu", GDF(internal_error_exit_stat)},
    {"%llu", GDF(unknown_ec_exit_stat)    },
    {"%llu", GDF(cp15_32_exit_stat)       },
    {"%llu", GDF(cp15_64_exit_stat)       },
    {"%llu", GDF(cp14_mr_exit_stat)       },
    {"%llu", GDF(cp14_ls_exit_stat)       },
    {"%llu", GDF(cp14_64_exit_stat)       },
    {"%llu", GDF(smc_exit_stat)           },
    {"%llu", GDF(sve_exit_stat)           },
    {"%llu", GDF(debug_exit_stat)         },
    {"%llu", GDF(steal)                   },
    {"%llu", GF(st_max)                   },
    {"%llu", GDF(vcpu_utime)              },
    {"%llu", GDF(vcpu_stime)              },
    {"%llu", GDF(gtime)                   }
#undef GF
#undef GDF
};

const int vcpu_stat_size = sizeof(vcpu_stat_stab) / sizeof(struct file_item);

int get_vcpu_list(struct domain_list *list)
{
    char buf[BUF_SIZE];
    unsigned int pid;
    FILE *fp = NULL;

    fp = fopen(KVM_VCPU_STAT_PATH, "r");
    if (!fp) {
        return -1;
    }
    clear_domains(list);
    while (fgets(buf, BUF_SIZE - 1, fp)) {
        char *p = NULL;
        char *p_next = NULL;
        int i = 0;

        /* limit vcpu nums to MAX_VCPU_NUM */
        if (list->num >= MAX_VCPU_NUM) {
            break;
        }
        struct domain *dom = add_domains(list);
        for (p = strtok_r(buf, " \t\r\n", &p_next); p && i < vcpu_stat_size;
             p = strtok_r(NULL, " \t\r\n", &p_next)) {
            if (vcpu_stat_stab[i].get_fun) {
                sscanf(p, vcpu_stat_stab[i].format,
                       (*vcpu_stat_stab[i].get_fun)(dom));
            }
            i++;
        }
    }
    fclose(fp);
    return list->num;
}

int get_vcpu_stat(struct domain *dom, struct domain_list *vcpu_list)
{
    struct domain *vcpu = NULL;
    int i;

    if (!dom || !vcpu_list) {
        return -1;
    }
    for (i = 0; i < vcpu_list->num; i++) {
        vcpu = &(vcpu_list->domains[i]);
        if (vcpu->pid == dom->pid) {
            break;
        }
    }
    if (i >= vcpu_list->num) {
        return 0;
    }
    for (i = 1; i < vcpu_stat_size; i++) {
        if (vcpu_stat_stab[i].get_fun) {
            memcpy((*vcpu_stat_stab[i].get_fun)(dom),
                   (*vcpu_stat_stab[i].get_fun)(vcpu), sizeof(u64));
        }
    }
    return 1;
}

/*
 * get delta value of kvm exit times over time
 */
void refresh_delta_vcpu_stat(struct domain *new, struct domain *old)
{
    if (!new || !old) {
        return;
    }
    for (int i = 0; i < vcpu_stat_size; i++) {
        if (vcpu_stat_stab[i].delta_fun) {
            (*vcpu_stat_stab[i].delta_fun)(new, old);
        }
    }
}

/*
 * get sum of kvm exit from threads of virtla machine
 */
void sum_vcpu_stat(struct domain *dom, struct domain *thread)
{
    if (!dom || !thread) {
        return;
    }
    for (int i = 0; i < vcpu_stat_size; i++) {
        if (vcpu_stat_stab[i].sum_fun) {
            (*vcpu_stat_stab[i].sum_fun)(dom, thread);
        }
    }
    dom->st_max += thread->st_max;
}
