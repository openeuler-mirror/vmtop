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

GET_FUN(state)
GET_FUN(ppid)
GET_FUN(pgrd)
GET_FUN(session)
GET_FUN(tty)
GET_FUN(tpgid)
GET_FUN(flags)
GET_FUN(min_flt)
GET_FUN(cmin_flt)
GET_FUN(maj_flt)
GET_FUN(cmaj_flt)
GET_FUN(utime)
GET_FUN(stime)
GET_FUN(cutime)
GET_FUN(cstime)
GET_FUN(priority)
GET_FUN(nice)
GET_FUN(nlwp)
GET_FUN(alarm)
GET_FUN(start_time)
GET_FUN(vsize)
GET_FUN(rss)
GET_FUN(rss_rlim)
GET_FUN(start_code)
GET_FUN(end_code)
GET_FUN(start_stack)
GET_FUN(kstk_esp)
GET_FUN(kstk_eip)
GET_FUN(wchan)
GET_FUN(exit_signal)
GET_FUN(processor)
GET_FUN(rtprio)
GET_FUN(sched)

int get_proc_stat(struct domain *dom);
#endif
