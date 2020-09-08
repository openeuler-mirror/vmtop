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

const char *summary_text = ""
    "vmtop - %s - %s\n"
    "Domains: %d running\n";
const char *filter_help = ""
    "field filter - select which field to be showed\n"
    "Use up/down to navigate, use space to set whether chosen filed to be showed\n"
    "'q' to quit to normal display\n";

FID fields[] = {
    /* name  .      flag     . align */
    {"DID",      FIELDS_DISPLAY, 5  },
    {"VM/task-name", FIELDS_DISPLAY, 14 },
    {"PID",      FIELDS_DISPLAY, 8  },
    {"%CPU",     FIELDS_DISPLAY, 6  },
    {"EXThvc",   FIELDS_DISPLAY, 10 },
    {"EXTwfe",   FIELDS_DISPLAY, 10 },
    {"EXTwfi",   FIELDS_DISPLAY, 10 },
    {"EXTmmioU", FIELDS_DISPLAY, 10 },
    {"EXTmmioK", FIELDS_DISPLAY, 10 },
    {"EXTfp",    FIELDS_DISPLAY, 10 },
    {"EXTirq",   FIELDS_DISPLAY, 10 },
    {"EXTsys64", FIELDS_DISPLAY, 10 },
    {"EXTmabt",  FIELDS_DISPLAY, 10 },
    {"EXTsum",   FIELDS_DISPLAY, 10 },
    {"S",        FIELDS_DISPLAY, 5  },
    {"P",        FIELDS_DISPLAY, 5  },
    {"%ST",      FIELDS_DISPLAY, 6  },
    {"%GUE",     FIELDS_DISPLAY, 6  },
    {"%HYP",     FIELDS_DISPLAY, 6  }
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
