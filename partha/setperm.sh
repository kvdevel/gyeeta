#!/bin/bash

PATH=$PATH:/usr/bin:/sbin:/usr/sbin:.
export PATH

BPFCORE=0

check_processor()
{
	if [ $( uname -a | grep -cw x86_64 ) -eq 0 ]; then 
		printf "\n\nThis package can be installed only on machines with 64 bit x86 processors (x86_64).\n\nExiting Installation...\n\n"
		exit 1
	fi

	PROCINFO=$( cat /proc/cpuinfo )

	if [ $( echo "$PROCINFO" | grep -w flags | grep -ic avx ) -eq 0 ]; then
		printf "\n\nThis package can be installed on machines with processors having avx instruction support (Intel Sandybridge (2012) or above).\n\nYour processor seems to be a very old one. Please install on a machine with a newer processor.\n\nExiting Installation...\n\n"
		exit 1
	fi
}


check_linux_kernel_version()
{
	KERN_VER=`uname -r`

	KERN_NUM1=$( echo $KERN_VER | awk -F. '{print $1}' )
	KERN_NUM2=$( echo $KERN_VER | awk -F. '{print $2}' )
	KERN_NUM3=$( echo $KERN_VER | awk -F. '{print $3}' | awk -F- '{print $1}' )

	MIN_VER="4.4.0"

	if [ $KERN_NUM1 -lt 4 ]; then
		printf "ERROR : Linux Kernel version $KERN_NUM1 is less than minimum $MIN_VER. Install Failed...\n\n"
		exit 1
	elif [ $KERN_NUM1 -eq 4 ] && [ $KERN_NUM2 -lt 4 ]; then
		printf "ERROR : Linux Kernel version $KERN_NUM1 is less than minimum $MIN_VER. Install Failed...\n\n"
		exit 1
	fi
}

check_lib_deps()
{
	if [ ! -f ./$1 ]; then
		printf "\nERROR : Please run this script from partha install dir where partha binary $1 is present...Exiting...\n\n"
		exit 1
	fi

	PCVER=$( ./$1 --version 2>&1 )

	if [ $? -ne 0 ]; then
		printf "\nERROR : Missing libraries or invalid binary : $PCVER\n\n"

		LDDOUT=`ldd ./$1 2>&1`
		if [ `echo "$LDOUT" | grep -c "not found"` -gt 0 ]; then
			printf "\nMissing libraries are : \n"
			echo "$LDOUT" | grep "not found"

			printf "\nERROR : Exiting ...\n\n"
			exit 1
		else
			echo
		fi

		printf "\nERROR : Exiting ...\n\n"
		exit 1
	fi

}

check_core_headers()
{
	if [ -f /sys/kernel/btf/vmlinux ]; then
		if [ $( ls -l /sys/kernel/btf/ | wc -l ) -gt 0 ]; then
			BPFCORE=1
			return
		fi	
	fi	
	
	if [ -d /lib/modules/$(uname -r)/build/ ]; then
		return
	fi	

	printf "\nERROR : Missing Kernel Headers Package : These are required by partha : Please install your Distribution package as per instructions below : \n\n"

	printf '\t\tFor Ubuntu or Debian based systems : \n\t\tsudo apt-get -y install linux-headers-$(uname -r)\n'
	printf "\t\tIf above package not found error seen, run the command (sudo apt update) and then retry thereafter\n\n"

	printf '\t\tFor CentOS, RHEL, Fedora, Amazon Linux based systems : \n\t\tsudo yum -y install kernel-devel-$(uname -r)\n\n'

	printf "\t\tFor OpenSUSE based systems : \n\t\tsudo zypper -n install kernel-default-devel-$(uname -r | awk -F- '{print $1}')\n"
	printf "\t\tIf above package not found error seen, run the command (sudo zypper -n dist-upgrade) and then retry thereafter\n\n"

	exit 1
}	

set_capabilities()
{
	command -v setcap > /dev/null
	
	if [ $? -ne 0 ]; then
		printf "\nERROR : Missing setcap package : Please install your distribution based setcap package as per below : \n\n"
		
		printf "\t\tFor Ubuntu or Debian based systems : \n\t\tsudo apt-get -y install libcap2-bin\n"
		printf "\t\tIf above package not found error seen, run the command (sudo apt update) and then retry thereafter\n\n"

		printf "\t\tFor CentOS, RHEL, Fedora, Amazon Linux based systems : \n\t\tsudo yum -y install libcap\n\n"

		printf "\t\tFor OpenSUSE based systems : \n\t\tsudo zypper -n install libcap-progs\n"
		printf "\t\tIf above package not found error seen, run the command (sudo zypper -n dist-upgrade) and then retry thereafter\n\n"

		exit 1
	fi	

	sudo -n setcap cap_chown,cap_dac_override,cap_dac_read_search,cap_fowner,cap_fsetid,cap_ipc_lock,cap_kill,cap_mac_admin,cap_mknod,cap_sys_chroot,cap_sys_resource,cap_setpcap,cap_sys_ptrace,cap_sys_admin,cap_net_admin,cap_net_raw,cap_sys_module,cap_setuid+ep ./partha-bpf && \
	sudo -n setcap cap_chown,cap_dac_override,cap_dac_read_search,cap_fowner,cap_fsetid,cap_ipc_lock,cap_kill,cap_mac_admin,cap_mknod,cap_sys_chroot,cap_sys_resource,cap_setpcap,cap_sys_ptrace,cap_sys_admin,cap_net_admin,cap_net_raw,cap_sys_module,cap_setuid+ep ./partha-bcc
	
	if [ $? -ne 0 ]; then 
		printf "\n\nERROR : Failed to set capabilities for partha : Please check if being run as sudo or root\n\n"
		exit 1
	fi	
}	

check_mount_suid()
{
	local MOUNTDIR=$( df -k . | tail -1 | awk '{print $1}' ) 

	local ISSUID=$( grep $MOUNTDIR /proc/self/mounts | grep -cw nosuid )

	if [ $ISSUID -ne 0 ]; then
		printf "\nERROR : partha dir $PWD (Mount dir $MOUNTDIR) has nosuid mount option set. partha needs Kernel Capabilities and cannot work on a nosuid mount...Exiting...\n\n"
		exit 1
	fi	
}


if [ ! -f ./partha-bpf ] ||  [ ! -f ./partha-bcc ] || [ ! -f ./runpartha.sh ]; then
	printf "\n\nERROR : Please run this script from partha install dir where runpartha.sh and partha binaries partha-bpf and partha-bcc are present.\n\n"
	exit 1
fi	

sudo -n bash -c "echo" &> /dev/null

if [ $? -ne 0 ]; then
	printf "\n\nPlease run this script as root or sudo ...\n\n"
	exit 1
fi	

check_processor
check_linux_kernel_version
check_core_headers
check_lib_deps partha-bpf
check_lib_deps partha-bcc
set_capabilities
check_mount_suid

printf "\n\npartha Package permissions set successfully : To start partha, please set the config file partha_main.json file in cfg dir : You can refer to sample_partha_main.json for a sample config...\n\n"

exit 0
