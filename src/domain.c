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
 * Description: get domain's data
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "domain.h"
#include "utils.h"
#include "proc.h"
#include "vcpu_stat.h"

#define VAR_RUN_QEMU_PATH "/var/run/libvirt/qemu"
#define PID_STRING_MAX 12
#define CGROUP_PATH_SIZE 30
#define TASK_STRING_SIZE 30

/* domain list operation */
void init_domains(struct domain_list *list)
{
    list->domains = NULL;
    list->num = 0;
}
void clear_domains(struct domain_list *list)
{
    for (int i = 0; i < list->num; i++) {
        if (list->domains[i].threads != NULL) {
            free(list->domains[i].threads);
            list->domains[i].threads = NULL;
        }
    }
    if (list->domains != NULL) {
        free(list->domains);
    }
    init_domains(list);
}

static struct domain *add_domains(struct domain_list *list)
{
    struct domain *new_list = malloc(sizeof(struct domain) * (list->num + 1));

    if (new_list == NULL) {
        return NULL;
    }
    memset(new_list, 0, sizeof(struct domain) * (list->num + 1));
    memcpy(new_list, list->domains, sizeof(struct domain) * list->num);
    free(list->domains);
    list->domains = new_list;
    list->num++;
    return &(list->domains[list->num - 1]);
}

static void copy_domains(struct domain_list *now, struct domain_list *pre)
{
    clear_domains(pre);
    pre->num = now->num;
    if (pre->num <= 0) {
        return;
    }
    pre->domains = malloc(sizeof(struct domain) * pre->num);
    if (pre->domains == NULL) {
        pre->num = 0;
        return;
    }
    memcpy(pre->domains, now->domains, sizeof(struct domain) * pre->num);
    for (int i = 0; i < pre->num; i++) {
        if (pre->domains[i].nlwp <= 0) {
            continue;
        }
        pre->domains[i].threads = malloc(sizeof(struct domain) *
                                         pre->domains[i].nlwp);
        if (pre->domains[i].threads == NULL) {
            continue;
        }
        memcpy(pre->domains[i].threads, now->domains[i].threads,
               sizeof(struct domain) * pre->domains[i].nlwp);
    }
}

static void pop_domains(struct domain_list *list)
{
    list->num--;
}

/*
 * get domain from domain list
 */
static struct domain *get_domain_from_id(int id, struct domain_list *list)
{
    for (int i = 0; i < list->num; i++) {
        if (list->domains != NULL && list->domains[i].domain_id == id) {
            return &(list->domains[i]);
        }
    }
    return NULL;
}

static struct domain *get_thread_from_pid(pid_t pid, struct domain *dom)
{
    for (int i = 0; i < dom->nlwp; i++) {
        if (dom->threads != NULL && dom->threads[i].pid == pid) {
            return &(dom->threads[i]);
        }
    }
    return NULL;
}

static int get_id_from_cgroup(pid_t pid)
{
    char path[CGROUP_PATH_SIZE];
    FILE *fp = NULL;
    char buf[BUF_SIZE];
    char *tmp = NULL;
    int id = -1;

    if (snprintf(path, CGROUP_PATH_SIZE, "/proc/%u/cgroup", pid) < 0) {
        return id;
    }
    fp = fopen(path, "r");
    if (fp == NULL) {
        return id;
    }
    /* parse id from "cpuset:machine.slice/machine-qemu\x2d$id" */
    while (fgets(buf, BUF_SIZE - 1, fp)) {
        if (strstr(buf, "cpuset:") == NULL) {
            memset(buf, 0, BUF_SIZE);
            continue;
        }
        tmp = strstr(buf, "machine-qemu\\x2d");
        if (tmp == NULL) {
            continue;
        }
        tmp += strlen("machine-qemu\\x2d");
        id = atoi(tmp);
        break;
    }
    fclose(fp);
    return id;
}

static int get_child_pid(struct domain *dom)
{
    char path[TASK_STRING_SIZE];
    DIR *dirptr = NULL;
    struct dirent *dirt = NULL;
    char *end = NULL;
    int i = 0;

    if (snprintf(path, TASK_STRING_SIZE, "/proc/%u/task", dom->pid) < 0) {
        return -1;
    }
    dirptr = opendir(path);
    if (dirptr == NULL) {
        return -1;
    }
    if (dom->nlwp <= 0) {
        return -1;
    }
    dom->threads = (struct domain *)malloc(sizeof(struct domain) * dom->nlwp);
    if (dom->threads == NULL) {
        closedir(dirptr);
        return -1;
    }
    memset(dom->threads, 0, sizeof(struct domain) * dom->nlwp);

    while ((dirt = readdir(dirptr)) != NULL && i < dom->nlwp) {
        if (!strcmp(dirt->d_name, ".") || !strcmp(dirt->d_name, "..")) {
            continue;
        }
        if (dirt->d_type != DT_DIR) {
            continue;
        }
        dom->threads[i].pid = strtoul(dirt->d_name, &end, 10);
        if (end <= dirt->d_name || dom->threads[i].pid < 1) {
            dom->threads[i].pid = 0;
            continue;
        }
        dom->threads[i].type = ISTHREAD;
        dom->threads[i].ppid = dom->pid;
        if (get_proc_stat(&(dom->threads[i])) < 0 ||
            get_proc_comm(&(dom->threads[i])) < 0) {
            continue;
        }
        if (strstr(dom->threads[i].vmname, "CPU") != NULL
            && get_vcpu_stat(&(dom->threads[i])) > 0) {
            dom->threads[i].type = ISVCPU;
            dom->smp_vcpus++;
        }
        dom->threads[i].smp_vcpus = 1;
        i++;
    }
    closedir(dirptr);
    return dom->nlwp;
}

static int set_domain(struct domain *dom, const char *name)
{
    int len = strlen(name);
    char path[BUF_SIZE];
    char pid[PID_STRING_MAX];
    char *end = NULL;

    if (len >= DOMAIN_NAME_MAX - 1) {
        return -1;
    }
    strcpy(dom->vmname, name);
    dom->vmname[len - strlen(".pid")] = '\0';

    if (snprintf(path, BUF_SIZE, "%s/%s", VAR_RUN_QEMU_PATH, name) < 0) {
        return -1;
    }
    if (read_file(pid, PID_STRING_MAX, path) < 0) {
        return -1;
    }
    dom->pid = strtoul(pid, &end, 10);
    if (end <= pid || dom->pid < 1) {
        return -1;
    }

    dom->domain_id = get_id_from_cgroup(dom->pid);
    if (dom->domain_id < 0) {
        return -1;
    }
    dom->type = ISDOMAIN;
    if (get_proc_stat(dom) < 0 || get_child_pid(dom) < 0) {
        return -1;
    }
    return 1;
}

/*
 * check whether filename end with ".pid"
 */
static int check_pid_file(const char *d_name)
{
    char *extern_name = ".pid";
    int extern_len = strlen(extern_name);
    int len = strlen(d_name);

    return strcmp(d_name + len - extern_len, extern_name);
}

static int get_qemu_id(struct domain_list *list)
{
    DIR *dir = NULL;
    struct dirent *dirent = NULL;

    dir = opendir(VAR_RUN_QEMU_PATH);
    if (dir == NULL) {
        return -1;
    }
    while ((dirent = readdir(dir)) != NULL) {
        if (!strcmp(dirent->d_name, ".") || !strcmp(dirent->d_name, "..")) {
            continue;
        }
        if (dirent->d_type != DT_REG) {
            continue;
        }
        if (check_pid_file(dirent->d_name)) {
            continue;
        }
        struct domain *dom = add_domains(list);
        if (set_domain(dom, dirent->d_name) < 0) {
            pop_domains(list);
        }
    }
    closedir(dir);
    return list->num;
}

static void refresh_threads(struct domain *dom, struct domain *old_dom)
{
    for (int i = 0; i < dom->nlwp; i++) {
        int pid = dom->threads[i].pid;
        struct domain *old_thread = get_thread_from_pid(pid, old_dom);
        if (old_thread == NULL) {
            continue;
        }
        refresh_delta_stat(&(dom->threads[i]), old_thread);
        if (dom->threads[i].type == ISVCPU) {
            refresh_delta_vcpu_stat(&(dom->threads[i]), old_thread);
            sum_vcpu_stat(dom, &(dom->threads[i]));
        }
    }
}

int refresh_domains(struct domain_list *now, struct domain_list *pre)
{
    int num;

    copy_domains(now, pre);    /* save last data int pre */
    clear_domains(now);
    num = get_qemu_id(now);

    for (int i = 0; i < now->num; i++) {
        int id = now->domains[i].domain_id;
        struct domain *old_domain = get_domain_from_id(id, pre);

        if (old_domain == NULL) {
            continue;
        }
        refresh_delta_stat(&(now->domains[i]), old_domain);
        refresh_threads(&(now->domains[i]), old_domain);
    }

    return num;
}

int get_task_num(struct domain_list *list)
{
    int sum = 0;

    for (int i = 0; i < list->num; i++) {
        sum += list->domains[i].nlwp;
    }
    sum += list->num;
    return sum;
}
