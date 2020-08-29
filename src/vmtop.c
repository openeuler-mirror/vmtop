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
struct domain_list scr_cur;

static void init_screen(void)
{
    initscr();                /* init to curses mode */
    keypad(stdscr, TRUE);     /* enable function key */
    noecho();                 /* disable key echo while getch */
    cbreak();                 /* disable line buffer */
    curs_set(0);              /* set curse no display */
}

static void parse_args(int argc, char *argv[])
{
    int opt;
    char *arg_ops = "d:";
    while ((opt = getopt(argc, argv, arg_ops)) != -1) {
        switch (opt) {
        case 'd': {
            delay_time = atoi(optarg);
            if (delay_time < 1) {
                delay_time = 1;
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
    attron(A_REVERSE);
    for (int i = 0; i < FD_END; i++) {
        if (fields[i].display_flag == 1) {
            printw("%*s", fields[i].align, fields[i].name);
        }
    }
    attroff(A_REVERSE);
    printw("\n");
}

/*
 * show single field of a domain task, align with header
 */
static void print_domain_field(struct domain *dom, int field)
{
    int i = field;

    switch (i) {
    case FD_VMNAME: {
        printw("%*s", fields[i].align, dom->vmname);
        break;
    }
    case FD_DID: {
        printw("%*d", fields[i].align, dom->domain_id);
        break;
    }
    case FD_PID: {
        printw("%*lu", fields[i].align, dom->pid);
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
    default:
        break;
    }
    return;
}

static void show_domains(struct domain_list *list)
{
    for (int i = 0; i < list->num; i++) {
        struct domain *dom = &(list->domains[i]);
        for (int j = 0; j < FD_END; j++) {
            print_domain_field(dom, j);
        }
        printw("\n");
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
    int attr_flag;

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
    default:
        break;
    }
    return;
}

int main(int argc, char *argv[])
{
    int key;

    parse_args(argc, argv);
    init_screen();

    quit_flag = 0;
    delay_time = 1;    /* default delay 1s between display*/

    do {
        refresh_domains(&scr_cur);

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
    } while (!quit_flag);
    endwin();    /* quit from curses mode */
    return 0;
}
