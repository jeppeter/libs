#! /bin/sh

RemoveDpkg()
{
        _pkg=$1
        /bin/echo -n "Remove $_pkg"
        _output=`(dpkg --remove $_pkg ) 2>&1`
        _res=$?
        if [ $_res -ne 0  ]
        then
                /bin/echo -e '\e[31m[FAILED]\e[0m' >&2
                /bin/echo "result:"
                /bin/echo -e '\033[34m$_res\033[0m' >&2
                exit 3
        fi

        /bin/echo -e '\e[32m[SUCCESS]\e[0m'

        _output=`(dpkg --purge $_pkg) 2>&1`
}

RemoveDpkgMul()
{
	_pkgs=$*
    /bin/echo -n "Remove $_pkgs"
    _output=`(dpkg --remove $_pkgs ) 2>&1`
	_res=$?
	if [ $_res -ne 0 ]
	then
        /bin/echo -e '\e[31m[FAILED]\e[0m' >&2
        /bin/echo "result:"
        /bin/echo -e '\033[34m$_res\033[0m' >&2
        exit 3
	fi
    /bin/echo -e '\e[32m[SUCCESS]\e[0m'

        _output=`(dpkg --purge $_pkgs) 2>&1`
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
RemoveDpkg manpages
RemoveDpkg memtest86+
RemoveDpkg mlocate
RemoveDpkg os-prober
RemoveDpkg  popularity-contest
RemoveDpkg pppoeconf
RemoveDpkg pppconfig
RemoveDpkg ppp
RemoveDpkg python-apt
RemoveDpkg python-apt-common
RemoveDpkg python-gdbm
RemoveDpkg python-pam
RemoveDpkg python-pkg-resources
RemoveDpkg python-twisted-bin
RemoveDpkg ssh-import-id
RemoveDpkg  python-requests
RemoveDpkg python-urllib3
RemoveDpkg python-six


# remove dns utils
RemoveDpkg dnsutils
RemoveDpkg bind9-host
RemoveDpkg libbind9-90
RemoveDpkg libisccfg90
RemoveDpkg libdns99
RemoveDpkg libisccc90
RemoveDpkg libisc95
RemoveDpkgMul  emacs emacs23 emacs23-bin-common emacs23-common emacs23-common-non-dfsg emacsen-common gconf-service gconf-service-backend  gconf2-common libcroco3 libfribidi0 libgconf-2-4 libgd3 libgif4 libgpm2 libice6 liblockfile1 libm17n-0 libotf0 librsvg2-2  librsvg2-common libsm6 libvpx1 libxpm4 libxt6 m17n-contrib m17n-db x11-common

RemoveDpkg uml-utilities

if [ 0 ] 
then
  account-plugin-aim account-plugin-facebook account-plugin-flickr
  account-plugin-google account-plugin-jabber account-plugin-salut
  account-plugin-twitter account-plugin-windows-live account-plugin-yahoo
  accountsservice acpi-support activity-log-manager
  activity-log-manager-control-center adium-theme-ubuntu aisleriot alsa-base
  alsa-utils anacron apg app-install-data app-install-data-partner appmenu-qt
  appmenu-qt5 apport apport-gtk apport-symptoms apt-xapian-index aptdaemon
  aptdaemon-data apturl apturl-common aspell aspell-en at-spi2-core
  avahi-autoipd avahi-daemon avahi-utils bamfdaemon baobab bc bind9-host
  binutils bluez bluez-alsa bluez-cups bluez-gstreamer branding-ubuntu brasero
  brasero-cdrkit brasero-common brltty bsdmainutils checkbox checkbox-qt
  cheese-common colord command-not-found-data compiz compiz-core compiz-gnome
  compiz-plugins-default cpp cpp-4.8 cracklib-runtime cryptsetup-bin cups
  cups-browsed cups-bsd cups-client cups-common cups-daemon cups-filters
  cups-pk-helper cups-ppdc cups-server-common dbus-x11 dc dconf-cli
  dconf-gsettings-backend dconf-service deja-dup deja-dup-backend-gvfs
  deja-dup-backend-ubuntuone desktop-file-utils dialog dictionaries-common
  diffstat dmz-cursor-theme dnsmasq-base doc-base dosfstools duplicity
  dvd+rw-tools empathy empathy-common enchant eog espeak-data evince
  evince-common evolution-data-server evolution-data-server-common
  evolution-data-server-goa example-content file-roller firefox folks-common
  fonts-freefont-ttf fonts-kacst fonts-kacst-one fonts-khmeros-core fonts-lao
  fonts-liberation fonts-lklug-sinhala fonts-nanum fonts-opensymbol
  fonts-sil-abyssinica fonts-sil-padauk fonts-takao-pgothic fonts-thai-tlwg
  fonts-tibetan-machine fonts-tlwg-garuda fonts-tlwg-kinnari fonts-tlwg-loma
  fonts-tlwg-mono fonts-tlwg-norasi fonts-tlwg-purisa fonts-tlwg-sawasdee
  fonts-tlwg-typewriter fonts-tlwg-typist fonts-tlwg-typo fonts-tlwg-umpush
  fonts-tlwg-waree foomatic-db-compressed-ppds foomatic-filters freerdp-x11
  friends friends-dispatcher friends-facebook friends-twitter gcc gcc-4.8
  gconf-service gconf-service-backend gconf2 gconf2-common gcr gdb gedit
  gedit-common genisoimage geoclue geoclue-ubuntu-geoip gettext ghostscript
  ghostscript-x gir1.2-accounts-1.0 gir1.2-appindicator3-0.1 gir1.2-atk-1.0
  gir1.2-atspi-2.0 gir1.2-dbusmenu-glib-0.4 gir1.2-dee-1.0 gir1.2-ebook-1.2
  gir1.2-ebookcontacts-1.2 gir1.2-edataserver-1.2 gir1.2-freedesktop
  gir1.2-gdata-0.0 gir1.2-gdkpixbuf-2.0 gir1.2-glib-2.0 gir1.2-gmenu-3.0
  gir1.2-gnomebluetooth-1.0 gir1.2-gnomekeyring-1.0 gir1.2-goa-1.0
  gir1.2-gst-plugins-base-1.0 gir1.2-gstreamer-1.0 gir1.2-gtk-3.0
  gir1.2-gudev-1.0 gir1.2-ibus-1.0 gir1.2-javascriptcoregtk-3.0
  gir1.2-messagingmenu-1.0 gir1.2-networkmanager-1.0 gir1.2-notify-0.7
  gir1.2-packagekitglib-1.0 gir1.2-pango-1.0 gir1.2-peas-1.0 gir1.2-rb-3.0
  gir1.2-signon-1.0 gir1.2-soup-2.4 gir1.2-syncmenu-0.1 gir1.2-totem-1.0
  gir1.2-totem-plparser-1.0 gir1.2-unity-5.0 gir1.2-vte-2.90 gir1.2-webkit-3.0
  gir1.2-wnck-3.0 gkbd-capplet glib-networking glib-networking-common
  glib-networking-services gnome-accessibility-themes gnome-bluetooth
  gnome-calculator gnome-contacts gnome-control-center
  gnome-control-center-data gnome-control-center-datetime
  gnome-control-center-signon gnome-control-center-unity gnome-desktop3-data
  gnome-disk-utility gnome-font-viewer gnome-icon-theme
  gnome-icon-theme-symbolic gnome-keyring gnome-mahjongg gnome-menus
  gnome-mines gnome-orca gnome-power-manager gnome-screensaver
  gnome-screenshot gnome-session gnome-session-bin gnome-session-canberra
  gnome-session-common gnome-settings-daemon gnome-sudoku gnome-system-log
  gnome-system-monitor gnome-terminal gnome-terminal-data gnome-user-guide
  gnome-user-share gnomine groff-base growisofs gsettings-desktop-schemas
  gsfonts gstreamer0.10-alsa gstreamer0.10-nice gstreamer0.10-plugins-base
  gstreamer0.10-plugins-base-apps gstreamer0.10-plugins-good
  gstreamer0.10-pulseaudio gstreamer0.10-tools gstreamer0.10-x
  gstreamer1.0-alsa gstreamer1.0-clutter gstreamer1.0-nice
  gstreamer1.0-plugins-base gstreamer1.0-plugins-base-apps
  gstreamer1.0-plugins-good gstreamer1.0-pulseaudio gstreamer1.0-tools
  gstreamer1.0-x gtk2-engines-murrine gtk3-engines-unico gucharmap
  guile-2.0-libs gvfs gvfs-backends gvfs-bin gvfs-common gvfs-daemons
  gvfs-fuse gvfs-libs hardening-includes hicolor-icon-theme hplip hplip-data
  hud humanity-icon-theme hunspell-en-us hwdata ibus ibus-gtk ibus-gtk3
  ibus-pinyin ibus-pinyin-db-android ibus-pinyin-db-open-phrase ibus-table
  im-config indicator-applet indicator-application indicator-appmenu
  indicator-bluetooth indicator-datetime indicator-keyboard indicator-messages
  indicator-power indicator-printers indicator-session indicator-sound
  indicator-sync init-system-helpers inputattach install-info intel-gpu-tools
  intltool-debian iproute iptables iputils-arping iso-codes kerneloops-daemon
  landscape-client-ui-install language-selector-common language-selector-gnome
  laptop-detect libaa1 libaccount-plugin-1.0-0 libaccount-plugin-generic-oauth
  libaccount-plugin-google libaccounts-glib0 libaccounts-qt5-1
  libaccountsservice0 libappindicator3-1 libapt-pkg-perl libarchive-zip-perl
  libarchive13 libart-2.0-2 libasan0 libasound2-plugins libaspell15
  libasprintf-dev libassuan0 libatasmart4 libatk-adaptor libatk-bridge2.0-0
  libatkmm-1.6-1 libatomic1 libatspi2.0-0 libaudio2 libauthen-sasl-perl
  libautodie-perl libavahi-core7 libavahi-glib1 libavahi-gobject0 libavc1394-0
  libbamf3-2 libbind9-90 libboost-date-time1.53.0 libbrasero-media3-1 libburn4
  libc-dev-bin libc6-dbg libc6-dev libcairo-gobject2 libcairo-perl
  libcairomm-1.0-1 libcamel-1.2-43 libcanberra-gtk-module libcanberra-gtk0
  libcanberra-gtk3-0 libcanberra-gtk3-module libcanberra-pulse libcanberra0
  libcap2-bin libcdio-cdda1 libcdio-paranoia1 libcdio13 libcdparanoia0
  libcdr-0.0-0 libcheese-gtk23 libcheese7 libclone-perl libcloog-isl4
  libclucene-contribs1 libclucene-core1 libclutter-1.0-0 libclutter-1.0-common
  libclutter-gst-2.0-0 libclutter-gtk-1.0-0 libcmis-0.3-3 libcogl-common
  libcogl-pango12 libcogl12 libcolamd2.7.1 libcolord1 libcolorhug1
  libcolumbus1 libcolumbus1-common libcompizconfig0 libcrack2 libcroco3
  libcrypt-passwdmd5-perl libcryptsetup4 libcupscgi1 libcupsfilters1
  libcupsimage2 libcupsmime1 libcupsppdc1 libcurl3 libdaemon0
  libdbusmenu-glib4 libdbusmenu-gtk3-4 libdbusmenu-gtk4 libdbusmenu-qt2
  libdbusmenu-qt5 libdconf1 libdecoration0 libdee-1.0-4
  libdevmapper-event1.02.1 libdigest-hmac-perl libdjvulibre-text
  libdjvulibre21 libdmapsharing-3.0-2 libdns99 libdotconf1.0 libdpkg-perl
  libdrm-intel1 libdrm-nouveau2 libdrm-radeon1 libdv4 libebackend-1.2-6
  libebook-1.2-14 libebook-contacts-1.2-0 libecal-1.2-15 libedata-book-1.2-17
  libedata-cal-1.2-20 libedataserver-1.2-17 libegl1-mesa libegl1-mesa-drivers
  libelfg0 libemail-valid-perl libenchant1c2a libespeak1 libevdocument3-4
  libevent-2.0-5 libevview3-3 libexempi3 libexif12 libexiv2-12
  libexttextcat-2.0-0 libexttextcat-data libfarstream-0.1-0 libfarstream-0.2-2
  libfftw3-single3 libfile-basedir-perl libfile-copy-recursive-perl
  libfile-desktopentry-perl libfile-fcntllock-perl libfile-mimeinfo-perl
  libfolks-eds25 libfolks-telepathy25 libfolks25 libfontembed1 libfontenc1
  libframe6 libfreerdp-plugins-standard libfreerdp1 libfriends0 libfs6
  libgail-3-0 libgail-common libgail18 libgbm1 libgc1c2 libgcc-4.8-dev
  libgck-1-0 libgconf-2-4 libgcr-3-1 libgcr-3-common libgcr-base-3-1
  libgcr-ui-3-1 libgd3 libgdata-common libgdata13 libgee-0.8-2 libgee2
  libgeis1 libgeoclue0 libgettextpo-dev libgettextpo0 libgexiv2-2
  libgirepository-1.0-1 libgl1-mesa-dri libgl1-mesa-glx libglamor0
  libglapi-mesa libglew1.8 libglewmx1.8 libglib-perl libglib2.0-bin
  libglibmm-2.4-1c2a libglu1-mesa libgmime-2.6-0 libgmp10 libgnome-bluetooth11
  libgnome-control-center1 libgnome-desktop-3-7 libgnome-keyring-common
  libgnome-keyring0 libgnome-menu-3-0 libgnomekbd-common libgnomekbd8
  libgoa-1.0-0 libgoa-1.0-common libgomp1 libgpgme11 libgphoto2-6
  libgphoto2-l10n libgphoto2-port10 libgpm2 libgpod-common libgpod4 libgrail6
  libgrip0 libgs9 libgs9-common libgssdp-1.0-3 libgstreamer-plugins-base0.10-0
  libgstreamer-plugins-base1.0-0 libgstreamer-plugins-good1.0-0
  libgstreamer0.10-0 libgstreamer1.0-0 libgtk-3-0 libgtk-3-bin libgtk-3-common
  libgtk2-perl libgtkmm-3.0-1 libgtksourceview-3.0-1
  libgtksourceview-3.0-common libgtop2-7 libgtop2-common libgucharmap-2-90-7
  libgudev-1.0-0 libgupnp-1.0-4 libgupnp-igd-1.0-4 libgusb2 libgutenprint2
  libgweather-3-3 libgweather-common libgxps2 libharfbuzz-icu0 libhpmud0
  libhud-client2 libhunspell-1.3-0 libhyphen0 libibus-1.0-5 libical1 libice6
  libicu48 libido3-0.1-0 libiec61883-0 libieee1284-3 libijs-0.35
  libimobiledevice4 libindicator3-7 libio-pty-perl libio-socket-inet6-perl
  libio-socket-ssl-perl libipc-run-perl libipc-system-simple-perl libisc95
  libisccc90 libisccfg90 libisl10 libisofs6 libitm1 libiw30 libjack-jackd2-0
  libjavascriptcoregtk-3.0-0 libjbig2dec0 libjson-glib-1.0-0
  libjson-glib-1.0-common libjte1 libkpathsea6 liblangtag-common liblangtag1
  liblcms1 liblcms2-2 liblightdm-gobject-1-0 liblircclient0
  liblist-moreutils-perl libllvm3.3 liblouis-data liblouis2 libltdl7
  liblua5.2-0 liblvm2app2.2 liblzo2-2 libmailtools-perl libmeanwhile1
  libmessaging-menu0 libmetacity-private0a libmhash2 libminiupnpc8
  libmission-control-plugins0 libmng1 libmnl0 libmpc3 libmpfr4 libmspub-0.0-0
  libmtdev1 libmtp-common libmtp-runtime libmtp9 libmysqlclient18
  libmythes-1.2-0 libnatpmp1 libnautilus-extension1a libneon27-gnutls
  libnet-dns-perl libnet-domain-tld-perl libnet-ip-perl libnet-libidn-perl
  libnet-smtp-ssl-perl libnet-ssleay-perl libnetfilter-conntrack3 libnettle4
  libnfnetlink0 libnice10 libnm-glib-vpn1 libnm-glib4 libnm-gtk-common
  libnm-gtk0 libnm-util2 libnotify-bin libnotify4 libnss-mdns libnux-4.0-0
  libnux-4.0-common liboauth0 libopencc1 libopenobex1 libopenvg1-mesa
  liborc-0.4-0 liborcus-0.6-0 libp11-kit-gnome-keyring libpackagekit-glib2-16
  libpam-cap libpam-freerdp libpam-gnome-keyring libpanel-applet-4-0
  libpango-perl libpangomm-1.4-1 libpaper-utils libpaper1 libpcsclite1
  libpeas-1.0-0 libpeas-common libperl5.14 libperlio-gzip-perl libpipeline1
  libplist1 libpocketsphinx1 libpolkit-agent-1-0 libpolkit-backend-1-0
  libpolkit-gobject-1-0 libpoppler-glib8 libpoppler43 libportaudio2
  libprotobuf7 libprotoc7 libproxy1 libproxy1-plugin-gsettings
  libproxy1-plugin-networkmanager libpulse-mainloop-glib0 libpulsedsp
  libpurple-bin libpurple0 libpwquality-common libpwquality1 libpython3.3
  libqpdf10 libqt4-dbus libqt4-declarative libqt4-designer libqt4-help
  libqt4-network libqt4-opengl libqt4-script libqt4-scripttools libqt4-sql
  libqt4-sql-mysql libqt4-sql-sqlite libqt4-svg libqt4-test libqt4-xml
  libqt4-xmlpatterns libqt53d5 libqt5core5 libqt5dbus5 libqt5gui5
  libqt5location5 libqt5network5 libqt5opengl5 libqt5printsupport5 libqt5qml5
  libqt5quick5 libqt5sensors5 libqt5sql5 libqt5sql5-sqlite libqt5test5
  libqt5v8-5 libqt5webkit5 libqt5widgets5 libqt5xml5 libqtassistantclient4
  libqtcore4 libqtgui4 libqtwebkit4 libquadmath0 libquvi-scripts libquvi7
  libraptor2-0 librasqal3 libraw1394-11 libraw9 librdf0 libreadline5
  libreoffice-base-core libreoffice-calc libreoffice-common libreoffice-core
  libreoffice-draw libreoffice-gnome libreoffice-gtk libreoffice-impress
  libreoffice-math libreoffice-ogltrans libreoffice-pdfimport
  libreoffice-presentation-minimizer libreoffice-style-galaxy
  libreoffice-style-human libreoffice-writer librest-0.7-0 librhythmbox-core7
  librsvg2-2 librsvg2-common librsync1 libsamplerate0 libsane libsane-common
  libsane-hpaio libsbc1 libsecret-1-0 libsecret-common libsensors4
  libsgutils2-2 libshout3 libsigc++-2.0-0c2a libsignon-extension1
  libsignon-glib1 libsignon-plugins-common1 libsignon-qt5-1 libsm6
  libsmbclient libsnmp-base libsnmp30 libsocket6-perl libsonic0
  libsoup-gnome2.4-1 libsoup2.4-1 libspectre1 libspeechd2 libspeex1
  libspeexdsp1 libsphinxbase1 libssh-4 libstartup-notification0
  libsub-identify-perl libsync-menu1 libsystemd-journal0 libt1-5
  libtag1-vanilla libtag1c2a libtalloc2 libtdb1 libtelepathy-farstream3
  libtelepathy-glib0 libtelepathy-logger3 libtevent0 libtext-levenshtein-perl
  libtheora0 libtimezonemap1 libtotem-plparser17 libtotem0 libtsan0
  libtxc-dxtn-s2tc0 libudisks2-0 libufe-xidgetter0 libunistring0
  libunity-core-6.0-8 libunity-gtk2-parser0 libunity-gtk3-parser0
  libunity-misc4 libunity-protocol-private0 libunity-scopes-json-def-desktop
  libunity-webapps0 libunity9 libupower-glib1 libupstart1 liburi-perl
  liburl-dispatcher1 libusbmuxd2 libutempter0 libuuid-perl libv4l-0
  libv4lconvert0 libvisio-0.0-0 libvisual-0.4-0 libvisual-0.4-plugins
  libvncserver0 libvorbisfile3 libvpx1 libvte-2.90-9 libvte-2.90-common
  libwacom-common libwacom2 libwavpack1 libwayland-client0 libwayland-cursor0
  libwayland-server0 libwbclient0 libwebkitgtk-3.0-0 libwebkitgtk-3.0-common
  libwhoopsie-preferences0 libwhoopsie0 libwmf0.2-7 libwmf0.2-7-gtk
  libwnck-3-0 libwnck-3-common libwnck-common libwnck22 libwpd-0.9-9
  libwpg-0.2-2 libwps-0.2-2 libx11-xcb1 libx86-1 libxapian22 libxatracker1
  libxaw7 libxcb-dri2-0 libxcb-glx0 libxcb-icccm4 libxcb-image0
  libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-shape0 libxcb-sync0
  libxcb-util0 libxcb-xfixes0 libxfont1 libxkbcommon0 libxkbfile1
  libxklavier16 libxmu6 libxp6 libxpm4 libxres1 libxslt1.1 libxt6 libxtst6
  libxv1 libxvmc1 libxxf86dga1 libxxf86vm1 libyaml-tiny-perl libyelp0
  libzeitgeist-1.0-1 libzeitgeist-2.0-0 libzephyr4 light-themes lightdm
  lightdm-remote-session-freerdp lightdm-remote-session-uccsconfigure lintian
  linux-libc-dev linux-sound-base lp-solve make man-db manpages manpages-dev
  mcp-account-manager-uoa media-player-info metacity-common
  mobile-broadband-provider-info modemmanager mousetweaks mscompress mtools
  mysql-common nautilus nautilus-data nautilus-sendto nautilus-sendto-empathy
  nautilus-share network-manager network-manager-gnome network-manager-pptp
  network-manager-pptp-gnome notification-daemon notify-osd notify-osd-icons
  nux-tools obex-data-server obexd-client onboard onboard-data oneconf
  oneconf-common openprinting-ppds overlay-scrollbar overlay-scrollbar-gtk2
  overlay-scrollbar-gtk3 p11-kit patch patchutils pcmciautils pinyin-database
  pkg-config plymouth-label plymouth-theme-ubuntu-logo pm-utils policykit-1
  policykit-1-gnome policykit-desktop-privileges poppler-data poppler-utils
  ppp pptp-linux printer-driver-c2esp printer-driver-foo2zjs
  printer-driver-gutenprint printer-driver-hpcups printer-driver-min12xxw
  printer-driver-pnm2ppa printer-driver-postscript-hp printer-driver-ptouch
  printer-driver-pxljr printer-driver-sag-gdi printer-driver-splix
  protobuf-compiler pulseaudio pulseaudio-module-bluetooth
  pulseaudio-module-x11 pulseaudio-utils python-apt python-apt-common
  python-aptdaemon python-aptdaemon.gtk3widgets python-cairo
  python-commandnotfound python-configglue python-crypto python-cups
  python-cupshelpers python-dbus python-dbus-dev python-debian
  python-debtagshw python-defer python-dirspec python-gconf python-gdbm
  python-gi python-gi-cairo python-gnomekeyring python-gobject
  python-gobject-2 python-gtk2 python-httplib2 python-imaging
  python-imaging-compat python-libxml2 python-lxml python-mako
  python-markupsafe python-notify python-oauthlib python-oneconf
  python-openssl python-pam python-pexpect python-piston-mini-client
  python-pkg-resources python-protobuf python-pycurl python-pyinotify
  python-qt4 python-qt4-dbus python-renderpm python-reportlab
  python-reportlab-accel python-serial python-sip python-six python-smbc
  python-twisted-bin python-twisted-core python-twisted-names
  python-twisted-web python-ubuntu-sso-client python-ubuntuone-client
  python-ubuntuone-control-panel python-ubuntuone-storageprotocol
  python-xapian python-xdg python-zeitgeist python-zope.interface
  python3-apport python3-apt python3-aptdaemon python3-aptdaemon.gtk3widgets
  python3-aptdaemon.pkcompat python3-brlapi python3-cairo python3-chardet
  python3-crypto python3-dbus python3-debian python3-defer python3-dirspec
  python3-distupgrade python3-feedparser python3-gi python3-gi-cairo
  python3-httplib2 python3-louis python3-lxml python3-oauthlib python3-oneconf
  python3-piston-mini-client python3-pkg-resources python3-problem-report
  python3-pyatspi python3-six python3-software-properties python3-speechd
  python3-uno python3-update-manager python3-xdg python3-xkit qdbus qpdf
  qt-at-spi qtchooser remmina remmina-common remmina-plugin-rdp
  remmina-plugin-vnc remote-login-service rfkill rhythmbox rhythmbox-data
  rhythmbox-mozilla rhythmbox-plugin-cdrecorder rhythmbox-plugin-magnatune
  rhythmbox-plugin-zeitgeist rhythmbox-plugins rhythmbox-ubuntuone rtkit
  samba-common samba-common-bin sane-utils seahorse session-migration
  sessioninstaller sgml-base shotwell shotwell-common signon-keyring-extension
  signon-plugin-oauth2 signon-plugin-password signon-ui signond simple-scan
  smbclient sni-qt software-center software-center-aptdaemon-plugins
  software-properties-common software-properties-gtk sound-theme-freedesktop
  speech-dispatcher sphinx-voxforge-hmm-en sphinx-voxforge-lm-en
  ssh-askpass-gnome ssl-cert syslinux syslinux-common syslinux-legacy
  system-config-printer-common system-config-printer-gnome
  system-config-printer-udev t1utils telepathy-gabble telepathy-haze
  telepathy-idle telepathy-indicator telepathy-logger
  telepathy-mission-control-5 telepathy-salut thin-client-config-agent
  thunderbird thunderbird-gnome-support toshset totem totem-common
  totem-mozilla totem-plugins transmission-common transmission-gtk
  ttf-indic-fonts-core ttf-punjabi-fonts ttf-ubuntu-font-family
  ttf-wqy-microhei ubuntu-artwork ubuntu-desktop ubuntu-docs
  ubuntu-drivers-common ubuntu-extras-keyring ubuntu-mono
  ubuntu-release-upgrader-core ubuntu-release-upgrader-gtk ubuntu-settings
  ubuntu-sounds ubuntu-sso-client ubuntu-sso-client-qt ubuntu-system-service
  ubuntu-wallpapers ubuntu-wallpapers-saucy ubuntuone-client
  ubuntuone-client-data ubuntuone-control-panel ubuntuone-control-panel-qt
  udisks udisks2 unattended-upgrades unity unity-asset-pool unity-greeter
  unity-gtk-module-common unity-gtk2-module unity-gtk3-module
  unity-lens-applications unity-lens-files unity-lens-friends unity-lens-music
  unity-lens-photos unity-lens-video unity-scope-audacious
  unity-scope-calculator unity-scope-chromiumbookmarks unity-scope-clementine
  unity-scope-colourlovers unity-scope-devhelp unity-scope-firefoxbookmarks
  unity-scope-gdrive unity-scope-gmusicbrowser unity-scope-gourmet
  unity-scope-guayadeque unity-scope-home unity-scope-manpages
  unity-scope-musicstores unity-scope-musique unity-scope-openclipart
  unity-scope-texdoc unity-scope-tomboy unity-scope-video-remote
  unity-scope-virtualbox unity-scope-yelp unity-scope-zotero
  unity-scopes-master-default unity-scopes-runner unity-services
  unity-webapps-common unity-webapps-service uno-libs3 unzip update-inetd
  update-manager update-manager-core update-notifier update-notifier-common
  upower ure usb-creator-common usb-creator-gtk usb-modeswitch
  usb-modeswitch-data usbmuxd vbetool vino wamerican
  webaccounts-extension-common whoopsie whoopsie-preferences wireless-tools
  wodim wpasupplicant x11-apps x11-common x11-session-utils x11-utils
  x11-xfs-utils x11-xkb-utils x11-xserver-utils xauth xbitmaps xcursor-themes
  xdg-user-dirs xdg-user-dirs-gtk xdg-utils xdiagnose xfonts-base
  xfonts-encodings xfonts-mathml xfonts-scalable xfonts-utils xinit xinput
  xorg xorg-docs-core xserver-common xserver-xorg xserver-xorg-core
  xserver-xorg-glamoregl xserver-xorg-input-all xserver-xorg-input-evdev
  xserver-xorg-input-mouse xserver-xorg-input-synaptics
  xserver-xorg-input-vmmouse xserver-xorg-input-wacom xserver-xorg-video-all
  xserver-xorg-video-ati xserver-xorg-video-cirrus xserver-xorg-video-fbdev
  xserver-xorg-video-intel xserver-xorg-video-mach64 xserver-xorg-video-mga
  xserver-xorg-video-modesetting xserver-xorg-video-neomagic
  xserver-xorg-video-nouveau xserver-xorg-video-openchrome
  xserver-xorg-video-qxl xserver-xorg-video-r128 xserver-xorg-video-radeon
  xserver-xorg-video-s3 xserver-xorg-video-savage
  xserver-xorg-video-siliconmotion xserver-xorg-video-sis
  xserver-xorg-video-sisusb xserver-xorg-video-tdfx xserver-xorg-video-trident
  xserver-xorg-video-vesa xserver-xorg-video-vmware xterm xul-ext-ubufox
  xul-ext-unity xul-ext-webaccounts xul-ext-websites-integration xvt yelp
  yelp-xsl zeitgeist zeitgeist-core zeitgeist-datahub zenity zenity-common zip
fi