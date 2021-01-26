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

#ifndef SRC_VCPU_STAT_H
#define SRC_VCPU_STAT_H

/* vcpu_stat items get fun */
GET_VALUE(pid)
GET_DELTA_FUN(hvc_exit_stat)
GET_DELTA_FUN(wfe_exit_stat)
GET_DELTA_FUN(wfi_exit_stat)
GET_DELTA_FUN(mmio_exit_user)
GET_DELTA_FUN(mmio_exit_kernel)
GET_DELTA_FUN(exits)
GET_DELTA_FUN(fp_asimd_exit_stat)
GET_DELTA_FUN(irq_exit_stat)
GET_DELTA_FUN(sys64_exit_stat)
GET_DELTA_FUN(mabt_exit_stat)
GET_DELTA_FUN(fail_entry_exit_stat)
GET_DELTA_FUN(internal_error_exit_stat)
GET_DELTA_FUN(unknown_ec_exit_stat)
GET_DELTA_FUN(cp15_32_exit_stat)
GET_DELTA_FUN(cp15_64_exit_stat)
GET_DELTA_FUN(cp14_mr_exit_stat)
GET_DELTA_FUN(cp14_ls_exit_stat)
GET_DELTA_FUN(cp14_64_exit_stat)
GET_DELTA_FUN(smc_exit_stat)
GET_DELTA_FUN(sve_exit_stat)
GET_DELTA_FUN(debug_exit_stat)
GET_DELTA_FUN(steal)
GET_VALUE(st_max)
GET_DELTA_FUN(vcpu_utime)
GET_DELTA_FUN(vcpu_stime)
GET_DELTA_FUN(gtime)

int get_vcpu_list(struct domain_list *list);
int get_vcpu_stat(struct domain *dom, struct domain_list *vcpu_list);
void refresh_delta_vcpu_stat(struct domain *new, struct domain *old);
void sum_vcpu_stat(struct domain *dom, struct domain *thread);
#endif
