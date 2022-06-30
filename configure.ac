AC_INIT([udpsniff-stats], [1.1], [dolecor@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AC_PROG_CC

AC_SUBST([IPC_LIBS])
AC_ARG_ENABLE(
    [ubus],
    [AS_HELP_STRING(
        [--enable-ubus],
        [Use ubus for IPC instead of POSIX mq])]
)
AM_CONDITIONAL([ENABLE_UBUS], [test x$enable_ubus = xyes])
AM_COND_IF([ENABLE_UBUS], [
    AC_CHECK_LIB([ubus], [ubus_connect_ctx],
                 [IPC_LIBS="-lubus  $IPC_LIBS"], [AC_MSG_ERROR([Could not find libubus])])
    AC_CHECK_LIB([ubox], [uloop_init],
                 [IPC_LIBS="-lubox  $IPC_LIBS"], [AC_MSG_ERROR([Could not find libubox])])
    AC_CHECK_LIB([blobmsg_json], [blobmsg_add_object],
                 [IPC_LIBS="-lblobmsg_json  $IPC_LIBS"], [AC_MSG_ERROR([Could not find libblobmsg_json])])
    ], [
    AC_CHECK_LIB([rt], [mq_open],
                 [IPC_LIBS="-lrt  $IPC_LIBS"], [AC_MSG_ERROR([Could not find lrt])])
])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT
