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
    {"DID",    FIELDS_DISPLAY, 5  },
    {"VMname", FIELDS_DISPLAY, 14 },
    {"PID",    FIELDS_DISPLAY, 8  },
    {"%CPU",   FIELDS_DISPLAY, 6  },
    {"S",      FIELDS_DISPLAY, 5  },
    {"P",      FIELDS_DISPLAY, 5  }
};
