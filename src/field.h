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
    FD_VMNAME,
    FD_DID,
    FD_PID,
    FD_END
};

typedef struct _field {
    const char *name;
    int display_flag;
    int align;
} FID;

extern FID fields[];
extern const char *summary_text;
extern const char *filter_help;

#endif
