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

int delay_time;
int quit_flag;
int scr_row_size;    /* screen height, and will refresh in every loop */
int scr_col_size;    /* screen width, and will refresh in every loop */
int showd_task;      /* the num that has been showd, include task be hidden */
int begin_task;
int begin_field;
unsigned int thread_mode;    /* decide whether to show threads */
int display_loop;
struct domain_list scr_cur;
struct domain_list scr_pre;

static void init_screen(void)
{
    initscr();                /* init to curses mode */
    keypad(stdscr, TRUE);     /* enable function key */
    noecho();                 /* disable key echo while getch */
    cbreak();                 /* disable line buffer */
    curs_set(0);              /* set curse no display */
    init_domains(&scr_cur);
    init_domains(&scr_pre);
    begin_task = 1;
    begin_field = 1;
    thread_mode = 0;    /* default not to show threads */
    quit_flag = 0;
    delay_time = 1;    /* default delay 1s between display */
    display_loop = -1;
}

static void parse_args(int argc, char *argv[])
{
    int opt;
    char *arg_ops = "Hd:n:";
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
        case 'n': {
            display_loop = atoi(optarg);
            if (display_loop == 0) {
                display_loop = -1;
            }
            break;
        }
        default:
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

    printw(summary_text, time_buf, PACKAGE_VERSION, scr_cur.num);
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
            printw("%*s", fields[i].align, fields[i].name);
        }
    }
    attroff(A_REVERSE);
    printw("\n");
    refresh();    /* refresh line to avoid last screen remains */
}

/*
 * show single field of a domain task, align with header
 */
static void print_domain_field(struct domain *dom, int field)
{
    int i = field;

    switch (i) {
    case FD_VMNAME: {
        printw("%*.*s", fields[i].align, fields[i].align - 2, dom->vmname);
        break;
    }
    case FD_DID: {
        if (dom->type == ISDOMAIN) {
            printw("%*d", fields[i].align, dom->domain_id);
        } else {
            printw("%*s", fields[i].align, "|_");
        }
        break;
    }
    case FD_PID: {
        printw("%*lu", fields[i].align, dom->pid);
        break;
    }
    case FD_CPU: {
        u64 cpu_jeffies = dom->DELTA_VALUE(utime) + dom->DELTA_VALUE(stime);
        double usage = (double)cpu_jeffies * 100 /
                       sysconf(_SC_CLK_TCK) / delay_time;
        printw("%*.1f", fields[i].align, usage);
        break;
    }
    /* kvm exit fields show */
    case FD_EXTHVC: {
        printw("%*llu", fields[i].align, dom->DELTA_VALUE(hvc_exit_stat));
        break;
    }
    case FD_EXTWFE: {
        printw("%*llu", fields[i].align, dom->DELTA_VALUE(wfe_exit_stat));
        break;
    }
    case FD_EXTWFI: {
        printw("%*llu", fields[i].align, dom->DELTA_VALUE(wfi_exit_stat));
        break;
    }
    case FD_EXTMMIOU: {
        printw("%*llu", fields[i].align, dom->DELTA_VALUE(mmio_exit_user));
        break;
    }
    case FD_EXTMMIOK: {
        printw("%*llu", fields[i].align, dom->DELTA_VALUE(mmio_exit_kernel));
        break;
    }
    case FD_EXTFP: {
        printw("%*llu", fields[i].align, dom->DELTA_VALUE(fp_asimd_exit_stat));
        break;
    }
    case FD_EXTIRQ: {
        printw("%*llu", fields[i].align, dom->DELTA_VALUE(irq_exit_stat));
        break;
    }
    case FD_EXTSYS64: {
        printw("%*llu", fields[i].align, dom->DELTA_VALUE(sys64_exit_stat));
        break;
    }
    case FD_EXTMABT: {
        printw("%*llu", fields[i].align, dom->DELTA_VALUE(mabt_exit_stat));
        break;
    }
    case FD_EXTSUM: {
        printw("%*llu", fields[i].align, dom->DELTA_VALUE(exits));
        break;
    }
    case FD_STATE: {
        printw("%*c", fields[i].align, dom->state);
        break;
    }
    case FD_P: {
        printw("%*d", fields[i].align, dom->processor);
        break;
    }
    case FD_ST: {
        printw("%*.1f", fields[i].align,
               (double)dom->DELTA_VALUE(steal) / 1000000.0f / delay_time);
        break;
    }
    case FD_GUE: {
        printw("%*.1f", fields[i].align,
               (double)dom->DELTA_VALUE(gtime) / 1000000.0f / delay_time);
        break;
    }
    case FD_HYP: {
        u64 hyp_time = dom->DELTA_VALUE(vcpu_utime) - dom->DELTA_VALUE(gtime) +
                       dom->DELTA_VALUE(vcpu_stime);
        printw("%*.1f", fields[i].align,
               (double)hyp_time / 1000000.0f / delay_time);
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
        showd_task - begin_task > scr_row_size - 4) {
        return;
    }
    clrtoeol();
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
    printw("\n");
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
}

/*
 * print fields list in vertical arrangement
 */
static void print_field(int high_light)
{
    int x = 3;    /* display x local */
    int y = 4;    /* display y local */
    unsigned int attr_flag;

    for (int i = 0; i < FD_END; i++) {
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

    init_screen();
    quit_flag = 0;
    delay_time = 1;    /* default delay 1s between display */
    parse_args(argc, argv);

    do {
        refresh_domains(&scr_cur, &scr_pre);

        getmaxyx(stdscr, scr_row_size, scr_col_size);

        /* display frame make */
        move(0, 0);
        summary();
        EMPTY_LINE();
        show_header();
        show_domains(&scr_cur);

        /*
         * set getch wait for delay time
         * if timeout retutn ERR and continue
         */
        halfdelay(delay_time * 10);
        key = getch();
        if (key != ERR) {
            parse_keys(key);
            clear();
        }
        if (display_loop > 0) {
            display_loop--;
        }
    } while (!quit_flag && display_loop);
    endwin();    /* quit from curses mode */
    return 0;
}
