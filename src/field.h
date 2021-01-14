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

#ifndef SRC_FIELD_H
#define SRC_FIELD_H

#define FIELDS_DISPLAY 1
#define FIELDS_HIDDEN 0

enum fields_type {
    FD_DID,
    FD_VMNAME,
    FD_PID,
    FD_CPU,
#ifdef __aarch64__
    FD_EXTHVC,
    FD_EXTWFE,
    FD_EXTWFI,
    FD_EXTMMIOU,
    FD_EXTMMIOK,
    FD_EXTFP,
    FD_EXTIRQ,
    FD_EXTSYS64,
    FD_EXTMABT,
    FD_EXTSUM,
    FD_EXTERR,
    FD_EXTUKN,
    FD_EXTCP153,
    FD_EXTCP156,
    FD_EXTCP14M,
    FD_EXTCP14L,
    FD_EXTCP146,
    FD_EXTSMC,
    FD_EXTSVE,
    FD_EXTDBG,
    FD_EXTFAIL,
#else
    FD_PFFIXED,
    FD_PFGUEST,
    FD_INVLPG,
    FD_IOEXITS,
    FD_MMIOEXITS,
    FD_EXTSUM,
    FD_EXTHALT,
    FD_EXTSIG,
    FD_EXTIRQ,
    FD_EXTNMIW,
    FD_EXTIRQW,
    FD_IRQIN,
    FD_NMIIN,
    FD_TLB,
    FD_HOSTREL,
    FD_HYPERV,
    FD_EXTCR,
    FD_EXTRMSR,
    FD_EXTWMSR,
    FD_EXTAPIC,
    FD_EXTEPTV,
    FD_EXTEPTM,
    FD_EXTPAU,
#endif
    FD_STATE,
    FD_P,
    FD_ST,
    FD_GUE,
    FD_HYP,
    FD_WAITMAX,
    FD_END
};

typedef struct _field {
    const char *name;
    int display_flag;
    int align;
    void *(*get_fun)(void *);
} FID;

extern FID fields[];
extern const char *summary_text;
extern const char *filter_help;
extern const char *help_text;
extern const char *version_text;

int get_show_field_num(void);
#endif
