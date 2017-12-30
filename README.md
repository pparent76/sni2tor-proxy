SNI2tor Proxy
=========

Proxies incoming TLS connections to tor hidden services based on the hostname
contained in the initial request of the TLS session (SNI). This enables to provide
a proxy frontend to allow clients to access some tor-hidden services without tor installed 
on the client machine, and while keeping the anonymity of the tor service provider AND the end-to-end 
encryption bewteen the tor hidden service provider, and the client. The proxy provider running sni2tor-proxy 
cannot decrypt the traffic.

News
----
*Decembre 2015: first working version.

Features
--------
+ Name-based proxying of HTTPS without decrypting traffic. No keys or
  certificates required.
* Dynamic association table beteween tor hidden services and hostnames,
  thanks to a MySQL database.

Usage Pre-requisit
-----

You must have a Tor-SOCK proxy running on port 9050.

Usage
-----

    Usage: sni2torproxy [-l port] [-f port] [--dbuser user] [--dbpasswd passwd] [-dbname name]
        -l  Listening port used by sni2tor-proxy
        -f  Port of tor hidden services that sni2tor-proxy should use to forward data
        --dbuser username used to connect to MySQL database
        --dbpasswd  password used to connect to MySQL database.
        --dbname name of the MySQL database that should be used.

Database
-----
Association between hostnames( domain-names) and tor hidden services addresse, 
must be stored in a MySQL Database running on localhost.

The default parameters of the Database are as follow:
+ DataBase name: sni2tor-proxy
+ DataBase user: proxyuser
+ Database password: proxypasswd

You can change those parameters thanks to command line arguments.

The data is read the Table "Association", supposed to have at least 
2 Columns: (hostname , torservice) allowing to query an association.
These parameters cannot be changed for now.

Installation
------------

For Debian or Fedora based Linux distributions see building packages below.

**Prerequisites**

+ Autotools (autoconf, automake, gettext and libtool)
+ libev4, libpcre and libudns development headers
+ Perl and cURL for test suite
* libmysqlclient-dev

**Install**

    ./autogen.sh && ./configure && make check && sudo make install

**Building Debian/Ubuntu package**

This is the preferred installation method on recent Debian based distributions:

1. Install required packages

        sudo apt-get install autotools-dev cdbs debhelper dh-autoreconf dpkg-dev gettext libev-dev libpcre3-dev libudns-dev pkg-config fakeroot libmysqlclient-dev devscripts

2. Build a Debian package

        ./autogen.sh && dpkg-buildpackage

3. Install the resulting package

        sudo dpkg -i ../sniproxy_<version>_<arch>.deb

**Building Fedora/RedHat package**

This is the preferred installation method for modern Fedora based distributions.

1. Install required packages

        sudo yum install autoconf automake curl gettext-devel libev-devel pcre-devel perl pkgconfig rpm-build udns-devel

2. Build a distribution tarball:

        ./autogen.sh && ./configure && make dist

3. Build a RPM package

        rpmbuild --define "_sourcedir `pwd`" -ba redhat/sniproxy.spec

4. Install resulting RPM

        sudo yum install ../sniproxy-<version>.<arch>.rpm

I've used Scientific Linux 6 a fair amount, but I prefer Debian based
distributions. RPM builds are tested in Travis-CI on Ubuntu, but not natively.
This build process may not follow the current Fedora packaging standards, and
may not even work.

***Building on OS X with Homebrew***

1. install dependencies.

        brew install libev pcre udns autoconf automake gettext libtool

2. Read the warning about gettext and force link it so autogen.sh works. We need the GNU gettext for the macro `AC_LIB_HAVE_LINKFLAGS` which isn't present in the default OS X package.

        brew link --force gettext

3. Make it so

        ./autogen.sh && ./configure && make

OS X support is a best effort, and isn't a primary target platform.


Configuration Syntax
--------------------

    user daemon

    pidfile /tmp/sniproxy.pid

    error_log {
        syslog daemon
        priority notice
    }

    listener 127.0.0.1:443 {
        protocol tls
        table TableName

        # Specify a server to use if the initial client request doesn't contain
        # a hostname
        fallback 192.0.2.5:443
    }

    table TableName {
        # Match exact request hostnames
        example.com 192.0.2.10:4343
        # If port is not specified the listener port will be used
        example.net [2001:DB8::1:10]
        # Or use regular expression to match
        .*\\.com    [2001:DB8::1:11]:443
        # Combining regular expression and wildcard will resolve the hostname
        # client requested and proxy to it
        .*\\.edu    *:443
    }

DNS Resolution
--------------

Using hostnames or wildcard entries in the configuration requires sniproxy to
be built with [UDNS](http://www.corpit.ru/mjt/udns.html). SNIProxy will still
build without UDNS, but these features will be unavailable.

UDNS uses a single UDP socket for all queries, so it is recommended you use a
local caching DNS resolver (with a single socket each DNS query is protected by
spoofing by a single 16 bit query ID, which makes it relatively easy to spoof).

UDNS is currently not available in Debian stable, but a package can be easily
built from the Debian testing or Ubuntu source packages:

    mkdir udns_packaging
    cd udns_packaging
    wget http://archive.ubuntu.com/ubuntu/pool/universe/u/udns/udns_0.4-1.dsc
    wget http://archive.ubuntu.com/ubuntu/pool/universe/u/udns/udns_0.4.orig.tar.gz
    wget http://archive.ubuntu.com/ubuntu/pool/universe/u/udns/udns_0.4-1.debian.tar.gz
    tar xfz udns_0.4.orig.tar.gz
    cd udns-0.4/
    tar xfz ../udns_0.4-1.debian.tar.gz
    dpkg-buildpackage
    cd ..
    sudo dpkg -i libudns-dev_0.4-1_amd64.deb libudns0_0.4-1_amd64.deb

