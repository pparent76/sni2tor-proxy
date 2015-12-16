SNI2tor Proxy
=========

Proxies incoming TLS connections to tor hidden services based on the hostname
contained in the initial request of the TLS session (SNI). This enables to provide
a proxy frontend to allow clients access some tor-hidden services without tor installed 
on the client machine, and while keeping end-to-end encryption bewteen the tor hidden
service provider, and the client. The proxy provider cannot decrypt the traffic.

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

        sudo apt-get install autotools-dev cdbs debhelper dh-autoreconf dpkg-dev gettext libev-dev libpcre3-dev libudns-dev pkg-config fakeroot libmysqlclient-dev

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

