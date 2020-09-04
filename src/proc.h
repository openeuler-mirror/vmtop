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

#ifndef SRC_PROC_H
#define SRC_PROC_H

GET_VALUE(state)
GET_VALUE(ppid)
GET_VALUE(pgrd)
GET_VALUE(session)
GET_VALUE(tty)
GET_VALUE(tpgid)
GET_VALUE(flags)
GET_VALUE(min_flt)
GET_VALUE(cmin_flt)
GET_VALUE(maj_flt)
GET_VALUE(cmaj_flt)
GET_DELTA_FUN(utime)
GET_DELTA_FUN(stime)
GET_VALUE(cutime)
GET_VALUE(cstime)
GET_VALUE(priority)
GET_VALUE(nice)
GET_VALUE(nlwp)
GET_VALUE(alarm)
GET_VALUE(start_time)
GET_VALUE(vsize)
GET_VALUE(rss)
GET_VALUE(rss_rlim)
GET_VALUE(start_code)
GET_VALUE(end_code)
GET_VALUE(start_stack)
GET_VALUE(kstk_esp)
GET_VALUE(kstk_eip)
GET_VALUE(wchan)
GET_VALUE(exit_signal)
GET_VALUE(processor)
GET_VALUE(rtprio)
GET_VALUE(sched)

int get_proc_stat(struct domain *dom);
void refresh_delta_stat(struct domain *new, struct domain *old);
int get_proc_comm(struct domain *dom);
#endif
