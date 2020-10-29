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
 * Description: display frame of vmtop
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "vmtop.h"
#include "field.h"
#include "domain.h"
#include "utils.h"
#include "../config.h"

#define TIME_STR_MAX 40
#define TERM_MODE 0
#define TEXT_MODE 1
#define FLUSH_SCR()  if (scr_mode == TERM_MODE) refresh(); else fflush(stdout)

int delay_time;
int quit_flag;
int scr_row_size;    /* screen height, and will refresh in every loop */
int scr_col_size;    /* screen width, and will refresh in every loop */
int showd_task;      /* the num that has been showd, include task be hidden */
int begin_task;
int begin_field;
unsigned int thread_mode;    /* decide whether to show threads */
int display_loop;
int scr_mode;
struct domain_list scr_cur;
struct domain_list scr_pre;

static int (*print_scr)(const char *fmt, ...);

static void init_screen(void)
{
    initscr();                /* init to curses mode */
    keypad(stdscr, TRUE);     /* enable function key */
    noecho();                 /* disable key echo while getch */
    cbreak();                 /* disable line buffer */
    curs_set(0);              /* set curse no display */
}

static void init_parameter(void)
{
    init_domains(&scr_cur);
    init_domains(&scr_pre);
    begin_task = 1;
    begin_field = 1;
    thread_mode = 0;    /* default not to show threads */
    quit_flag = 0;
    delay_time = 1;    /* default delay 1s between display */
    display_loop = -1;
    scr_mode = TERM_MODE;
    quit_flag = 0;
    delay_time = 1;    /* default delay 1s between display */
    scr_row_size = 2048;    /* defualt size row */
    scr_col_size = 1024;    /* default size col */
    monitor_id = -1;    /* default monitor all domains */
}

static void parse_args(int argc, char *argv[])
{
    int opt;
    char *arg_ops = "hvHd:n:bp:";
    while ((opt = getopt(argc, argv, arg_ops)) != -1) {
        switch (opt) {
        case 'd': {
            delay_time = atoi(optarg);
            if (delay_time < 1) {
                delay_time = 1;
            }
            break;
        }
        case 'H': {
            thread_mode = 1;
            break;
        }
        case 'h': {
            printf(help_text);
            exit(0);
        }
        case 'v': {
            printf(version_text, PACKAGE_VERSION);
            exit(0);
        }
        case 'n': {
            display_loop = atoi(optarg);
            if (display_loop == 0) {
                display_loop = -1;
            }
            break;
        }
        case 'b': {
            scr_mode = TEXT_MODE;
            break;
        }
        case 'p': {
            monitor_id = atoi(optarg);
            break;
        }
        default:
            exit(1);    /* exit vmtop when args are invalid */
            break;
        }
    }
    return;
}

static void summary(void)
{
    char time_buf[TIME_STR_MAX];

    if (get_time_str(time_buf, TIME_STR_MAX) < 0 &&
        snprintf(time_buf, TIME_STR_MAX, "err to get time") < 0) {
        return;
    }

    print_scr(summary_text, time_buf, PACKAGE_VERSION, scr_cur.num);
    return;
}

static void show_header(void)
{
    int showd_width = 0;
    int showd_field = 0;
    attron(A_REVERSE);
    for (int i = 0; i < FD_END; i++) {
        if (fields[i].display_flag == 1 &&
            (i == FD_DID || i == FD_VMNAME || ++showd_field >= begin_field)) {
            showd_width += fields[i].align;
            if (showd_width >= scr_col_size) {
                break;
            }
            print_scr("%*s", fields[i].align, fields[i].name);
        }
    }
    attroff(A_REVERSE);
    print_scr("\n");
    FLUSH_SCR();
}

static double justify_usage(double usage, struct domain *dom)
{
    double ret = usage;
    if (usage >= 100.0 * dom->smp_vcpus) {
        ret = 100.0 * dom->smp_vcpus;
    }
    return ret;
}

/*
 * show single field of a domain task, align with header
 */
static void print_domain_field(struct domain *dom, int field)
{
    int i = field;

    switch (i) {
    case FD_VMNAME: {
        print_scr("%*.*s", fields[i].align, fields[i].align - 2, dom->vmname);
        break;
    }
    case FD_DID: {
        if (dom->type == ISDOMAIN) {
            print_scr("%*d", fields[i].align, dom->domain_id);
        } else {
            print_scr("%*s", fields[i].align, "|_");
        }
        break;
    }
    case FD_PID: {
        print_scr("%*lu", fields[i].align, dom->pid);
        break;
    }
    case FD_CPU: {
        u64 cpu_jeffies = dom->DELTA_VALUE(utime) + dom->DELTA_VALUE(stime);
        double usage = (double)cpu_jeffies * 100 /
                       sysconf(_SC_CLK_TCK) / delay_time;

        print_scr("%*.1f", fields[i].align, justify_usage(usage, dom));
        break;
    }
    /* kvm exit fields show */
    case FD_EXTHVC:
    case FD_EXTWFE:
    case FD_EXTWFI:
    case FD_EXTMMIOU:
    case FD_EXTMMIOK:
    case FD_EXTFP:
    case FD_EXTIRQ:
    case FD_EXTSYS64:
    case FD_EXTMABT:
    case FD_EXTSUM:
    case FD_EXTERR:
    case FD_EXTUKN:
    case FD_EXTCP153:
    case FD_EXTCP156:
    case FD_EXTCP14M:
    case FD_EXTCP14L:
    case FD_EXTCP146:
    case FD_EXTSMC:
    case FD_EXTSVE:
    case FD_EXTDBG:
    case FD_EXTFAIL: {
        print_scr("%*llu", fields[i].align, *(u64 *)(*fields[i].get_fun)(dom));
        break;
    }
    case FD_STATE: {
        print_scr("%*c", fields[i].align, *(char *)(*fields[i].get_fun)(dom));
        break;
    }
    case FD_P: {
        print_scr("%*d", fields[i].align, *(int *)(*fields[i].get_fun)(dom));
        break;
    }
    case FD_ST:
    case FD_GUE: {
        u64 time = *(u64 *)(*fields[i].get_fun)(dom);
        double usage = (double)time * 100 / 1000000000.0f / delay_time;

        print_scr("%*.1f", fields[i].align, justify_usage(usage, dom));
        break;
    }
    case FD_HYP: {
        u64 hyp_time = dom->DELTA_VALUE(vcpu_utime) - dom->DELTA_VALUE(gtime) +
                       dom->DELTA_VALUE(vcpu_stime);
        double usage = (double)hyp_time * 100 / 1000000000.0f / delay_time;

        print_scr("%*.1f", fields[i].align, justify_usage(usage, dom));
        break;
    }
    case FD_WAITMAX: {
        u64 st_max = *(u64 *)(*fields[i].get_fun)(dom);
        /* show Max Scheduling Delay time in ms unit */
        print_scr("%*.3f", fields[i].align, st_max / 1000000.0f);
        break;
    }
    default:
        break;
    }
    return;
}

static void show_task(struct domain *task)
{
    int showd_width = 0;    /* make show width do not beyond screen */
    int showd_field = 0;
    showd_task++;
    if (showd_task < begin_task ||
        showd_task - begin_task > scr_row_size - 5) {
        return;
    }
    clrtoeol();
    FLUSH_SCR();    /* clear line before display */
    for (int i = 0; i < FD_END; i++) {
        if (fields[i].display_flag == 1 &&
            (i == FD_DID || i == FD_VMNAME || ++showd_field >= begin_field)) {
            showd_width += fields[i].align;
            if (showd_width >= scr_col_size) {
                break;
            }
            print_domain_field(task, i);
        }
    }
    FLUSH_SCR();
    print_scr("\n");
}

static void show_domains_threads(struct domain *dom)
{
    if (!dom) {
        return;
    }
    for (int i = 0; i < dom->nlwp; i++) {
        struct domain *thread = &(dom->threads[i]);
        if (thread == NULL) {
            continue;
        }
        show_task(thread);
    }
}

static void show_domains(struct domain_list *list)
{
    showd_task = 0;
    for (int i = 0; i < list->num; i++) {
        struct domain *dom = &(list->domains[i]);
        show_task(dom);
        if (thread_mode == 1) {
            show_domains_threads(dom);
        }
    }
    clrtobot();    /* clear to bottom to avoid image residue */
    FLUSH_SCR();
}

/*
 * print fields list in vertical arrangement
 */
static void print_field(int high_light)
{
    int x, y, x_local, y_local;
    unsigned int attr_flag;

    getyx(stdscr, y_local, x_local);    /* get cursor coordinates */
    y = y_local;
    x = x_local + 3;    /* leave 3 spaces in the beginning for beauty */

    for (int i = 0; i < FD_END; i++) {
        /*
         * if y local is more than scr_row_size, fields list display will
         * out of screen range. So start another col to show fields list
         * with 20 intervals.
         */
        if (y >= scr_row_size) {
            y = y_local;
            x = x + 20;
        }

        attr_flag = A_NORMAL;
        if (i == high_light) {
            attr_flag |= A_REVERSE;     /* high light chosen field */
        }
        if (fields[i].display_flag == 1) {
            mvprintw(y, x - 2, "*");    /* add '*' in front of field */
        } else {
            mvprintw(y, x - 2, " ");    /* clear '*' in front of field */
        }
        attron(attr_flag);
        mvprintw(y, x, "%s", fields[i].name);
        attroff(attr_flag);
        y++;
    }
}

/*
 * show field list to user
 * so that user can choose which field to display
 */
static void show_filter(void)
{
    int key;
    int high_light = 0;

    clear();    /* clear screen before new display */
    while (1) {
        mvprintw(0, 0, filter_help);    /* print help text at (0, 0) local */
        EMPTY_LINE();
        print_field(high_light);
        key = getch();
        if (key != ERR) {
            switch (key) {
            case KEY_UP: {
                high_light--;
                if (high_light < 0) {
                    high_light = FD_END - 1;
                }
                break;
            }
            case KEY_DOWN: {
                high_light++;
                if (high_light >= FD_END) {
                    high_light = 0;
                }
                break;
            }
            case ' ': {
                int flag = fields[high_light].display_flag;
                fields[high_light].display_flag = !flag;
                break;
            }
            case 'q': {
                return;
            }
            default:
                break;
            }
        }
    }
    return;
}

static void parse_keys(int key)
{
    switch (key) {
    case 'f': {
        show_filter();
        break;
    }
    case 'q': {
        quit_flag = !quit_flag;
        break;
    }
    case KEY_UP: {
        int task_num = thread_mode ? get_task_num(&scr_cur) : scr_cur.num;

        begin_task++;
        if (begin_task > task_num) {
            begin_task = task_num;
        }
        break;
    }
    case KEY_DOWN: {
        begin_task--;
        if (begin_task < 1) {
            begin_task = 1;
        }
        break;
    }
    case KEY_LEFT: {
        begin_field--;
        if (begin_field < 1) {
            begin_field = 1;
        }
        break;
    }
    case KEY_RIGHT: {
        int field_num = get_show_field_num();
        begin_field++;
        if (begin_field >= field_num) {
            begin_field = field_num - 1;
        }
        break;
    }
    case 'H': {
        thread_mode = !thread_mode;
        begin_task = 1;    /* mode change, so show from first task */
        break;
    }
    default:
        break;
    }
    return;
}

int main(int argc, char *argv[])
{
    int key;

    init_parameter();
    parse_args(argc, argv);
    if (scr_mode == TERM_MODE) {
        print_scr = printw;
        init_screen();
    } else {
        print_scr = printf;
    }

    do {
        if (scr_mode == TERM_MODE) {
            getmaxyx(stdscr, scr_row_size, scr_col_size);
            move(0, 0);
        }
        refresh_domains(&scr_cur, &scr_pre);

        /* frame make */
        summary();
        print_scr("\n");
        show_header();
        show_domains(&scr_cur);

        if (scr_mode == TERM_MODE) {
            /*
             * set getch wait for delay time
             * if timeout return ERR and continue
             */
            halfdelay(delay_time * 10);
            key = getch();
            if (key != ERR) {
                parse_keys(key);
                clear();
            }
        } else {
            usleep(delay_time * 1000000);    /* wait delay time in text mode */
        }
        FLUSH_SCR();
        if (display_loop > 0) {
            display_loop--;
        }
    } while (!quit_flag && display_loop);
    if (scr_mode == TERM_MODE) {
        endwin();    /* quit from curses mode */
    }
    return 0;
}
