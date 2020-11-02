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
    {"DID",      FIELDS_DISPLAY, 5,  NULL                         },
    {"VM/task-name", FIELDS_DISPLAY, 14, NULL                     },
    {"PID",      FIELDS_DISPLAY, 8,  NULL                         },
    {"%CPU",     FIELDS_DISPLAY, 8,  NULL                         },
#ifdef __aarch64__
    {"EXThvc",   FIELDS_DISPLAY, 10, GDF(hvc_exit_stat)           },
    {"EXTwfe",   FIELDS_DISPLAY, 10, GDF(wfe_exit_stat)           },
    {"EXTwfi",   FIELDS_DISPLAY, 10, GDF(wfi_exit_stat)           },
    {"EXTmmioU", FIELDS_DISPLAY, 10, GDF(mmio_exit_user)          },
    {"EXTmmioK", FIELDS_DISPLAY, 10, GDF(mmio_exit_kernel)        },
    {"EXTfp",    FIELDS_DISPLAY, 10, GDF(fp_asimd_exit_stat)      },
    {"EXTirq",   FIELDS_DISPLAY, 10, GDF(irq_exit_stat)           },
    {"EXTsys64", FIELDS_DISPLAY, 10, GDF(sys64_exit_stat)         },
    {"EXTmabt",  FIELDS_DISPLAY, 10, GDF(mabt_exit_stat)          },
    {"EXTsum",   FIELDS_DISPLAY, 10, GDF(exits)                   },
    {"EXTerr",   FIELDS_HIDDEN,  10, GDF(internal_error_exit_stat)},
    {"EXTukn",   FIELDS_HIDDEN,  10, GDF(unknown_ec_exit_stat)    },
    {"EXTcp153", FIELDS_HIDDEN,  10, GDF(cp15_32_exit_stat)       },
    {"EXTcp156", FIELDS_HIDDEN,  10, GDF(cp15_64_exit_stat)       },
    {"EXT14m",   FIELDS_HIDDEN,  10, GDF(cp14_mr_exit_stat)       },
    {"EXT14l",   FIELDS_HIDDEN,  10, GDF(cp14_ls_exit_stat)       },
    {"EXT146",   FIELDS_HIDDEN,  10, GDF(cp14_64_exit_stat)       },
    {"EXTsmc",   FIELDS_HIDDEN,  10, GDF(smc_exit_stat)           },
    {"EXTsve",   FIELDS_HIDDEN,  10, GDF(sve_exit_stat)           },
    {"EXTdbg",   FIELDS_HIDDEN,  10, GDF(debug_exit_stat)         },
    {"EXTfail",  FIELDS_HIDDEN,  10, GDF(fail_entry_exit_stat)    },
#else
    {"PFfix",    FIELDS_HIDDEN,  10, GDF(pf_fixed)                },
    {"PFgu",     FIELDS_HIDDEN,  10, GDF(pf_guest)                },
    {"INvlpg",   FIELDS_HIDDEN,  10, GDF(invlpg)                  },
    {"EXTio",    FIELDS_HIDDEN,  10, GDF(io_exits)                },
    {"EXTmmio",  FIELDS_HIDDEN,  10, GDF(mmio_exits)              },
    {"EXTsum",   FIELDS_DISPLAY, 10, GDF(exits)                   },
    {"EXThalt",  FIELDS_HIDDEN,  10, GDF(halt_exits)              },
    {"EXTsig",   FIELDS_HIDDEN,  10, GDF(signal_exits)            },
    {"EXTirq",   FIELDS_HIDDEN,  10, GDF(irq_exits)               },
    {"EXTnmiW",  FIELDS_HIDDEN,  10, GDF(nmi_window_exits)        },
    {"EXTirqW",  FIELDS_HIDDEN,  10, GDF(irq_window_exits)        },
    {"IrqIn",    FIELDS_HIDDEN,  10, GDF(irq_injections)          },
    {"NmiIn",    FIELDS_HIDDEN,  10, GDF(nmi_injections)          },
    {"TLBfl",    FIELDS_HIDDEN,  10, GDF(tlb_flush)               },
    {"HostReL",  FIELDS_HIDDEN,  10, GDF(host_state_reload)       },
    {"Hyperv",   FIELDS_HIDDEN,  10, GDF(hypercalls)              },
    {"EXTcr",    FIELDS_HIDDEN,  10, GDF(cr_exits)                },
    {"EXTrmsr",  FIELDS_HIDDEN,  10, GDF(msr_rd_exits)            },
    {"EXTwmsr",  FIELDS_HIDDEN,  10, GDF(msr_wr_exits)            },
    {"EXTapic",  FIELDS_HIDDEN,  10, GDF(apic_wr_exits)           },
    {"EXTeptv",  FIELDS_DISPLAY, 10, GDF(ept_vio_exits)           },
    {"EXTeptm",  FIELDS_HIDDEN,  10, GDF(ept_mis_exits)           },
    {"EXTpau",   FIELDS_DISPLAY, 10, GDF(pause_exits)             },
#endif
    {"S",        FIELDS_DISPLAY, 5,  GF(state)                    },
    {"P",        FIELDS_DISPLAY, 5,  GF(processor)                },
    {"%ST",      FIELDS_DISPLAY, 8,  GDF(steal)                   },
    {"%GUE",     FIELDS_DISPLAY, 8,  GDF(gtime)                   },
    {"%HYP",     FIELDS_DISPLAY, 8,  NULL                         },
    {"WAITmax",  FIELDS_HIDDEN,  9,  GF(st_max)                   }
#undef GDF
#undef GF
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
