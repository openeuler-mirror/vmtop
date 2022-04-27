# vmtop

A tool for collecting and analyzing data of virtual machine.

### Description

vmtop is a user-mode tool running on the host. You can use the tool to dynamically view the usage of virtual machine resources in real time, including CPU usage, memory usage, and vCPU KVM exits. vmtop brings great convenience to locating virtualization problems and performance optimization, and is a practical tool that integrates multi-party information for monitoring virtual machines.

### Building

This project uses ncurses, ncurses-devel, autoconf, libvirt, libvirt-devel. Check them out if you do not have them locally installed.

The simple steps to make vmtop are:

```sh
1.  aclocal
2.  autoconf
3.  autoheader
4.  automake --add-missing
5.  ./configure
6.  make
```

Then you will find vmtop in the source code dictionary.

### Introductions

#### Usage

Run directly from the command line mode.

```sh
vmtop [-option]
```

#### Start Options

- -d: sets the refresh interval
- -H: shows thread information
- -n: sets the number of refresh times, which is not limited by default
- -b: displays in text mode to save results to a file

#### Shortcut Keys

- H: displays or stops the thread information
- q: quits vmtop
- f: shows field filter


### Contribution

#### Git

1.  Fork the repository
2.  Create Feat_xxx branch
3.  Commit your code
4.  Create Pull Request

#### Style

You are advised to keep new files consistent with the existing files. For new files:

- in C: use kernel coding style
- in other languages: keep them consistent with the existing files

#### Patches

Use 'git format-patch' to format patches and use 'git send-email' to send mail to [@openeuler](virt@openeuler.org) mailing list. For patch sets, the '--cover-letter' option is recommended when you format the patches. This helps us understand the features of the patches.

### Maintainers

[@zhanghailiang](https://gitee.com/zhanghailiang_luckyh)
[@nocjj](https://gitee.com/nocjj)

### License

[Mulan](License/LICENSE)