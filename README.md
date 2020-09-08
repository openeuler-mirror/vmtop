# vmtop

A tool for collecting and analyzing data of virtual machine.

### Description
vmtop is a user-mode tool running on the host, which can dynamically view the resource usage of virtual machines in real time, include CPU usage, memory usage, vcpu kvm exits. It brings great convenience to virtualization problems and performance optimization, and is a pratical tool that intergrates multi-party info to monitor virtual machines.

### Building
This project uses ncurses, ncurses-devel, autoconf, libvirt, libvirt-devel. Go to check them out if you do not have them locally installed.

The simple steps to make vmtop are:
```sh
1.  aclocal
2.  autoconf
3.  autoheader
4.  automake --add-missing
5.  ./configure
6.  make
```
And then you will find vmtop in the source code dictionary.

### Introductions
#### usage
Run directly from the commandline.
```sh
vmtop [-option]
```
#### start option
- d: Set delay time between two display
- H: Show threads
- n: Set display times, default no limit
- b: Display in text mode to save result in file

#### shortcut key
- H: Open thread mode or close thread mode
- q: Quit from vmtop
- f: Show fields filter


### Contribution

#### Git
1.  Fork the repository
2.  Create Feat_xxx branch
3.  Commit your code
4.  Create Pull Request

#### Style
It is much better to be consistent with the exiting files. For new files:
- C: use kernel code style
- others: keep same with exiting files

#### Patches
Use 'git format-patch' to format patched and use 'git send-email' to send mail to [@openeuler](virt@openeuler.org) mailing list. And if it is a patch set, it is much better to use '--cover-letter' option when you format patches, so that we can understand what the pacthset does.

### Maintainers
[@zhanghailiang](https://gitee.com/zhanghailiang_luckyh)
[@nocjj](https://gitee.com/nocjj)

### License
[Mulan](License/LICENSE)
