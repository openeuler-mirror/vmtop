Name: vmtop
Version: 1.1
Release: 3
Summary: A tool for collecting and analyzing data of virtual machine
License: Mulan PSL V2
Group: Application/System
URL: https://gitee.com/openeuler/vmtop

Source:https://gitee.com/openeuler/vmtop/repository/archive/v%{version}.tar.gz#/%{name}-%{version}.tar.gz

Patch0001: bugfix-fix-ST-GUE-HYP-formula.patch
Patch0002: display-expand-row-size-in-TEXT-mode.patch
Patch0003: bugfix-exit-vmtop-when-arguments-are-invalid.patch
Patch0004: bugfix-check-unsigned-number-flip-before-getting-del.patch
Patch0005: vmtop-add-h-and-v.patch
Patch0006: vcpustat-modify-vcpu-info-acquirement-from-debugfs.patch
Patch0007: display-expand-CPU-display.patch
Patch0008: display-add-limit-to-usage-display.patch
Patch0009: vmtop-simplify-print_domain_field.patch
Patch0010: vcpu_stat-add-remaining-kvm-exits-items-to-display.patch
Patch0011: display-modify-filter-display-to-support-more-displa.patch
Patch0012: vcp_stat-add-Max-Scheduling-Delay-time-items-to-disp.patch
Patch0013: args-add-p-option.patch
Patch0014: key-add-page-up-down-key-response.patch
Patch0015: vcpu_stat-get-vcpu-stat-list-once-per-display-instea.patch
Patch0016: proc-del-prc-pid-comm-read.patch
Patch0017: display-del-screen-clear-after-key-response.patch
Patch0018: arch-add-x86-kvm-exits-items.patch
Patch0019: codestyle-del-unused-var.patch
Patch0020: bugfix-add-check-to-avoid-invalid-ptr-for-strcmp.patch
Patch0021: input-add-invalid-opt-check-in-input.patch
Patch0022: version-unified-with-release-version.patch

Requires: libvirt, ncurses

BuildRequires: ncurses-devel
BuildRequires: libtool
BuildRequires: autoconf
BuildRequires: automake
Buildrequires: libvirt-devel

Provides: vmtop = %{version}-%{release}

%description
This is a userspace tool which you can run it in host to help detecting VM's performance. By vmtop, you can quickly query vcpu info such as cpu usage, kvm exit times, memory usage and etc.

%prep
%setup -n %{name}
%autopatch -p1


%build
aclocal
autoconf
autoheader
automake --add-missing
./configure --libdir=%{_libdir} \
            --bindir=%{_bindir} \
            --sbindir=%{_sbindir} \
            --enable-secure-build
make

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/bin
install -m 550 vmtop ${RPM_BUILD_ROOT}/usr/bin/%{name}

%files
%license License/LICENSE
%{_bindir}/vmtop

%changelog
* Sat Feb 27 2021 Huawei Technologies Co., Ltd <alex.chen@huawei.com>
- version: unified with release version

* Sat Feb 27 2021 Huawei Technologies Co., Ltd <alex.chen@huawei.com>
- input: add invalid opt check in input

* Thu Jan 21 2021 Huawei Technologies Co., Ltd <alex.chen@huawei.com>
- bugfix: add check to avoid invalid ptr for strcmp

* Thu 14 Jan 2021 Jiajun Chen <1250062498@qq.com> -1.1-2
- vcp_stat: add Max Scheduling Delay time items to display
- args: add -p option
- key: add page up/down key response
- vcpu_stat: get vcpu stat list once per display instead of per vcpu
- proc: del /prc/pid/comm read
- display: del screen clear after key response
- arch: add x86 kvm exits items
- codestyle: del unused var

* Wed Oct 28 2020 Huawei Technologies Co., Ltd <alex.chen@huawei.com>
- display: modify filter display to support more display fields items

* Wed Oct 28 2020 Huawei Technologies Co., Ltd <alex.chen@huawei.com>
- vcpu_stat: add remaining kvm exits items to display

* Sat Oct 10 2020 Jiajun Chen <1250062498@qq.com> -1.1-0
- spec: modify source url

* Sun Sep 27 2020 nocjj <1250062498@qq.com>
- vcpustat: modify vcpu info acquirement from debugfs
- display: expand %CPU display
- display: add limit to usage display
- vmtop: simplify print_domain_field

* Mon Sep 21 2020 Ruyi Chen <chenruyi2@huawei.com>
- vmtop: add -h and -v

* Mon Sep 21 2020 Jiajun Chen <1250062498@qq.com> - 1.0-2
- bugfix: fix %ST, %GUE, %HYP formula
- display: expand row size in TEXT mode
- bugfix: exit vmtop when arguments are invalid
- bugfix: check unsigned number flip before getting delta

* Wed Sep 09 2020 Jiajun Chen <1250062498@qq.com> - 1.0-1
- vmtop：Show kvm exit items and add document to project

* Tue Aug 25 2020 Jiajun Chen <1250062498@qq.com> - 1.0-0
- vmtop: add spec and source code tar for project to build rpm
