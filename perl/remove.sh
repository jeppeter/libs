#! /bin/sh

RemoveDpkg()
{
        _pkg=$1
        dpkg --remove $_pkg
        _res=$?
        if [ $_res -ne 0  ]
        then
                echo "remove ($_pkg) Error($_res)" >&2
                exit 3
        fi

        dpkg --purge $_pkg
}

RemoveDpkgMul()
{
	_pkgs=$*
	dpkg --remove $_pkgs
	_res=$?
	if [ $_res -ne 0 ]
	then
		echo "remove ($_pkgs) Error($_res)" >&2
		exit 3
	fi

	dpkg --purge $_pkgs
}

RemoveDir()
{
	_dir=$1
	if [  ! -d $_dir ]
	then
		echo "($_dir) not directory" >&2
		return
	fi
	rm -rf $_dir
	
}

RemoveDpkg  ubuntu-standard
RemoveDpkg man-db
RemoveDpkg bsdmainutils
RemoveDpkg ubuntu-virt-server
RemoveDpkg libvirt-bin
RemoveDpkg libnetcf1
RemoveDpkg libaugeas0
RemoveDpkg augeas-lenses
RemoveDpkg byobu
RemoveDpkg geoip-database
RemoveDpkg groff-base
RemoveDpkg hicolor-icon-theme
RemoveDpkg iproute
RemoveDpkg ufw
RemoveDpkg iptables
RemoveDpkg krb5-locales
RemoveDpkg laptop-detect
RemoveDpkg apparmor
RemoveDpkg libapparmor-perl
RemoveDpkg update-notifier-common
RemoveDpkg patch
RemoveDpkg policykit-1
RemoveDpkg libpolkit-agent-1-0:amd64
RemoveDpkg tcpd
RemoveDpkg linux-generic
RemoveDpkg linux-image-generic
RemoveDpkg libvirt0
RemoveDpkg landscape-common

RemoveDpkgMul tasksel-data tasksel
RemoveDpkg aptitude
RemoveDpkg apt-xapian-index
RemoveDpkg python-xapian
RemoveDpkg libxapian22

RemoveDpkg libpolkit-backend-1-0:amd64
RemoveDpkg info
RemoveDpkg bc
RemoveDpkg install-info
RemoveDpkg language-selector-common
RemoveDpkg software-properties-common
RemoveDpkg python3-software-properties
RemoveDpkg iso-codes
RemoveDpkg libboost-iostreams1.53.0:amd64
RemoveDpkg tmux
RemoveDpkg apport
RemoveDpkg python3-apport
RemoveDpkg python3-problem-report
RemoveDpkg update-manager-core
RemoveDir /var/lib/update-manager
RemoveDpkg ubuntu-release-upgrader-core
RemoveDpkgMul   python3-distupgrade python3-update-manager
RemoveDpkg command-not-found
RemoveDpkg python3-commandnotfound
RemoveDpkg python3-gdbm:amd64
RemoveDpkg unattended-upgrades
RemoveDpkg python3-gi
RemoveDpkg python3-apt
RemoveDpkg python3-dbus
RemoveDpkg vim
RemoveDpkg vim-runtime
RemoveDpkg w3m
RemoveDpkg telnet
RemoveDpkg sharutils
RemoveDpkg xml-core
RemoveDpkg sgml-base
RemoveDpkg  python-twisted-core
RemoveDpkg  python-zope.interface
RemoveDpkg python-openssl
RemoveDpkg python-newt
RemoveDpkg python-serial
RemoveDpkg python-debian
RemoveDpkg plymouth-theme-ubuntu-text
RemoveDpkg nano
RemoveDpkg mtr-tiny
RemoveDpkg wireless-tools
RemoveDpkg libiw30
RemoveDpkg xauth
RemoveDpkg lockfile-progs
RemoveDpkg liblockfile1:amd64
RemoveDpkg dnsmasq-base
RemoveDpkg libnetfilter-conntrack3:amd64
RemoveDpkg libnfnetlink0:amd64
RemoveDpkg wpasupplicant
RemoveDpkg libpcsclite1:amd64
RemoveDpkg libmnl0:amd64
RemoveDpkg whoopsie
RemoveDpkg curl
RemoveDpkg libcurl3:amd64
RemoveDpkg libevent-2.0-5:amd64
RemoveDpkg accountsservice
RemoveDpkg  libaccountsservice0
RemoveDpkg aptitude-common
RemoveDpkg apport-symptoms
RemoveDpkg biosdevname
RemoveDpkg command-not-found-data
RemoveDpkg friendly-recovery
RemoveDpkg busybox-static
RemoveDpkg  dosfstools
RemoveDpkg gir1.2-glib-2.0
RemoveDpkg  installation-report 
RemoveDpkg libept1.4.12 
RemoveDpkg libfribidi0:amd64
RemoveDpkg libgc1c2:amd64
RemoveDpkg libgcr-base-3-1:amd64
RemoveDpkg libgck-1-0:amd64
RemoveDpkg libgcr-3-common
RemoveDpkg libwhoopsie0
RemoveDpkg libxslt1.1:amd64
RemoveDpkg libgirepository-1.0-1
RemoveDpkg libgpm2:amd64 
RemoveDpkg libpipeline1:amd64
RemoveDpkg libpolkit-gobject-1-0:amd64
RemoveDpkg libreadline5:amd64
RemoveDpkg libcwidget3
RemoveDpkg libsigc++-2.0-0c2a:amd64
RemoveDpkg linux-headers-generic
RemoveDpkg linux-headers-3.11.0-12-generic
RemoveDpkg linux-headers-3.11.0-12
RemoveDpkg lshw
