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

#define VAR_RUN_QEMU_PATH "/var/run/libvirt/qemu"
#define PID_STRING_MAX 12
#define CGROUP_PATH_SIZE 30

/* domain list operation */
void init_domains(struct domain_list *list)
{
    if (list->domains != NULL) {
        free(list->domains);
    }
    list->domains = NULL;
    list->num = 0;
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

static void pop_domains(struct domain_list *list)
{
    list->num--;
}

static int get_id_from_cgroup(pid_t pid)
{
    char path[CGROUP_PATH_SIZE];
    FILE *fp = NULL;
    char buf[BUF_SIZE];
    char *tmp = NULL;
    int id = -1;

    if (snprintf(path, CGROUP_PATH_SIZE, "/proc/%lu/cgroup", pid) < 0) {
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

static int set_domain(struct domain *dom, const char *name)
{
    int len = strlen(name);
    char path[PATH_MAX];
    char pid[PID_STRING_MAX];
    char *end = NULL;
   
    if (len >= DOMAIN_NAME_MAX - 1) {
        return -1;
    }
    strcpy(dom->vmname, name);
    dom->vmname[len - strlen(".pid")] = '\0';

    if (snprintf(path, PATH_MAX, "%s/%s", VAR_RUN_QEMU_PATH, name) < 0) {
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
    if (get_proc_stat(dom) < 0) {
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

int refresh_domains(struct domain_list *list)
{
    int num;

    init_domains(list);
    num = get_qemu_id(list);

    return num;
}
