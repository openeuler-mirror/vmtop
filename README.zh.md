# vmtop

用于收集和分析虚拟机数据的工具。

### 介绍

vmtop 是运行于主机端的用户模式工具，能够以动态方式实时查看虚拟机资源使用量，包括 CPU 和内存使用量，以及 vCPU KVM 退出事件。vmtop 为定位虚拟化问题和性能优化提供了极大的便利，是一种能集成多方信息以便监控虚拟机的实用工具。

### 构建

本项目使用 ncurses、ncurses-devel、autoconf、libvirt、libvirt-devel，请提前在本地安装好这些软件库。

构建 vmtop 的步骤非常简单：

```sh
1.  aclocal
2.  autoconf
3.  autoheader
4.  automake --add-missing
5.  ./configure
6.  make
```

之后就可以在源代码字典中找到 vmtop。

### 使用说明

#### 使用方法

直接以命令行模式运行。

```sh
vmtop [-option]
```

#### 启动选项

- -d: 设置显示刷新间隔
- -H: 显示线程信息
- -n: 设置显示刷新次数，默认不限制次数
- -b: 以文字模式显示，以便将结果保存到文件

#### 快捷键

- H: 开启或关闭线程信息显示
- q: 退出 vmtop
- f: 显示字段过滤


### 参与贡献

#### Git

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request

#### 代码风格

新增文件的风格建议与已有文件保持一致。对于新增文件：

- C 语言：使用内核编码风格
- 其他语言：与已有文件保持一致

#### 补丁

使用 'git format-patch' 来格式化补丁，使用 'git send-email' 来向 [@openeuler](virt@openeuler.org) 邮件列表发送邮件。对于补丁集，建议在格式化补丁时添加 '--cover-letter' 选项，以便我们更好地了解补丁的功能特性。

### 维护人员

[@zhanghailiang](https://gitee.com/zhanghailiang_luckyh)
[@nocjj](https://gitee.com/nocjj)

### 授权

[Mulan](License/LICENSE)