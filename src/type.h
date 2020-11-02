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
 ********************************************************************************/

#ifndef SRC_TYPE_H
#define SRC_TYPE_H

#include <unistd.h>

typedef unsigned long long u64;

#define DOMAIN_NAME_MAX 256
#define BUF_SIZE 1024
#define MAX_VCPU_NUM 1024

#define DELTA_VALUE(v) delta_ ## v
#define DFX_VALUE(v) v, DELTA_VALUE(v)

#define GET_NAME(v) domain_get_ ## v
#define GET_VALUE(v)                                      \
    static inline void *GET_NAME(v)(struct domain *dom) \
    {                                                   \
        return (void *)(&(dom->v));                     \
    }

#define GET_DELTA_NAME(v) domain_get_delta_ ## v
#define GET_DELTA_VALUE(v)                                      \
    static inline void *GET_DELTA_NAME(v)(struct domain *dom) \
    {                                                         \
        return (void *)(&(dom->DELTA_VALUE(v)));              \
    }

#define DELTA_NAME(v)  domain_delta_ ## v
#define DELTA_FUN(v)                                     \
    static inline void DELTA_NAME(v)(struct domain *new, \
                                     struct domain *old) \
    {                                                    \
        if (new->v >= old->v) {                          \
            new->DELTA_VALUE(v) = new->v - old->v;       \
        } else {                                         \
            new->DELTA_VALUE(v) = old->DELTA_VALUE(v);   \
        }                                                \
    }

#define SUM_NAME(v) domain_sum_ ## v
#define SUM_FUN(v)                                        \
    static inline void SUM_NAME(v)(struct domain *dom,    \
                                   struct domain *thread) \
    {                                                     \
        dom->DELTA_VALUE(v) += thread->DELTA_VALUE(v);    \
    }

#define GET_DELTA_FUN(v) \
    GET_VALUE(v)  \
    GET_DELTA_VALUE(v) \
    DELTA_FUN(v)  \
    SUM_FUN(v)

struct file_item {
    const char *format;
    void *(*get_fun)(void *);
    void (*delta_fun)(void *, void *);
    void (*sum_fun)(void *, void *);
};

struct domain {
    int type;
    int domain_id;
    char vmname[DOMAIN_NAME_MAX];
    pid_t pid;

    char state;
    int
        ppid,
        pgrd,
        session,
        tty,
        tpgid,
        nlwp,
        exit_signal,
        processor;
    long int
        priority,
        nice,
        alarm,
        rss;
    unsigned int
        flags,
        min_flt,
        cmin_flt,
        maj_flt,
        cmaj_flt,
        vsize,
        rss_rlim,
        start_code,
        end_code,
        start_stack,
        kstk_esp,
        kstk_eip,
        rtprio,
        sched,
        wchan;
    u64
        DFX_VALUE(utime),
        DFX_VALUE(stime),
        cutime,
        cstime,
        start_time;
    /* vcpu_stat items */
    u64
#ifdef __aarch64__
        DFX_VALUE(hvc_exit_stat),
        DFX_VALUE(wfe_exit_stat),
        DFX_VALUE(wfi_exit_stat),
        DFX_VALUE(mmio_exit_user),
        DFX_VALUE(mmio_exit_kernel),
        DFX_VALUE(exits),
        DFX_VALUE(fp_asimd_exit_stat),
        DFX_VALUE(irq_exit_stat),
        DFX_VALUE(sys64_exit_stat),
        DFX_VALUE(mabt_exit_stat),
        DFX_VALUE(fail_entry_exit_stat),
        DFX_VALUE(internal_error_exit_stat),
        DFX_VALUE(unknown_ec_exit_stat),
        DFX_VALUE(cp15_32_exit_stat),
        DFX_VALUE(cp15_64_exit_stat),
        DFX_VALUE(cp14_mr_exit_stat),
        DFX_VALUE(cp14_ls_exit_stat),
        DFX_VALUE(cp14_64_exit_stat),
        DFX_VALUE(smc_exit_stat),
        DFX_VALUE(sve_exit_stat),
        DFX_VALUE(debug_exit_stat),
#else
        DFX_VALUE(pf_fixed),
        DFX_VALUE(pf_guest),
        DFX_VALUE(tlb_flush),
        DFX_VALUE(invlpg),
        DFX_VALUE(exits),
        DFX_VALUE(io_exits),
        DFX_VALUE(mmio_exits),
        DFX_VALUE(signal_exits),
        DFX_VALUE(irq_window_exits),
        DFX_VALUE(nmi_window_exits),
        DFX_VALUE(halt_exits),
        DFX_VALUE(halt_successful_poll),
        DFX_VALUE(halt_attempted_poll),
        DFX_VALUE(halt_wakeup),
        DFX_VALUE(request_irq_exits),
        DFX_VALUE(irq_exits),
        DFX_VALUE(host_state_reload),
        DFX_VALUE(fpu_reload),
        DFX_VALUE(insn_emulation),
        DFX_VALUE(insn_emulation_fail),
        DFX_VALUE(hypercalls),
        DFX_VALUE(irq_injections),
        DFX_VALUE(nmi_injections),
        DFX_VALUE(cr_exits),
        DFX_VALUE(msr_rd_exits),
        DFX_VALUE(msr_wr_exits),
        DFX_VALUE(apic_wr_exits),
        DFX_VALUE(ept_vio_exits),
        DFX_VALUE(ept_mis_exits),
        DFX_VALUE(pause_exits),
#endif
        DFX_VALUE(steal),
        st_max,
        DFX_VALUE(vcpu_utime),
        DFX_VALUE(vcpu_stime),
        DFX_VALUE(gtime);
    struct domain *threads;
    int smp_vcpus;
};

struct domain_list {
    struct domain *domains;
    int num;
};

enum process_type {
    ISDOMAIN,
    ISVCPU,
    ISTHREAD,
    ISEND
};
#endif
