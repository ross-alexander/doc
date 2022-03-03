
    tar -C /usr/src -xf linux-5.11.10.tar.gz
	make -C /usr/src/linux-5.11.10 O=/usr/src/linux-5.11.10-x86-std defconfig
	cd /usr/src/linux-5.11.10-x86-std

Copy .config into directory from old kernel build

    make -j24

	make install INSTALL_PATH=/locker/pkg/linux-5.11.10 
	make modules_install INSTALL_MOD_PATH=/locker/pkg/linux-5.11.10
	make headers_install INSTALL_HDR_PATH=/locker/pkg/linux-5.11.10/usr


	cd /locker/build/glibc-2.33
	mkdir build
	cd build
	CC="gcc -march=znver1" ../configure --prefix=/usr --enable-tls --enable-kernel=5.10.0 --with-headers=/usr/src/linux-5.11.10-x86-std/usr/include
