~~~
mojo 13:57:07 fixincludes$ pwd
/locker/build/gcc-11.1.0/build/fixincludes


mojo 13:56:31 fixincludes$ TARGET_MACHINE=x86_64-pc-linux-gnu MACRO_LIST=../gcc/macro_list ./fixinc.sh /tmp/foo /usr/include/bits
Fixing headers into /tmp/foo for x86_64-pc-linux-gnu target
Forbidden identifiers: linux unix
Finding directories and links to directories
 Searching /usr/include/bits/.
Making symbolic directory links
Fixing directory /usr/include/bits into /tmp/foo
Applying stdio_va_list_clients    to wchar.h
Applying machine_name             to statx.h
Fixed:  statx.h
Quoted includes in statx.h
Applying stdio_stdarg_h           to stdio.h
Fixed:  stdio.h
Applying sysv68_string            to string.h
Cleaning up unneeded directories:
fixincludes is done
mojo 13:56:51 fixincludes$ vi ../gcc/macro_list
~~~
