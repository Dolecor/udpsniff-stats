bin_PROGRAMS = udp-sniff #print-stats

LDADD = -lrt $(IPC_LIBS)
if ENABLE_UBUS
    AM_CPPFLAGS = -DUSE_UBUS
    IPC_SRC =
else
    IPC_SRC =   \
        ipc/mq/mq_common.c                  \
        ipc/mq/mq_common.h                  \
        ipc/mq/mq_interface_provider.c      \
        ipc/mq/mq_interface_provider.h      \
        ipc/mq/mq_interface_subscriber.c    \
        ipc/mq/mq_interface_subscriber.h
endif

udp_sniff_SOURCES =     \
    udpsniff/main.c         \
    udpsniff/control.c      \
    udpsniff/control.h      \
    udpsniff/exec_options/exec_option.c       \
    udpsniff/exec_options/exec_option1.c      \
    udpsniff/exec_options/exec_option2.c      \
    udpsniff/exec_options/exec_options.h      \
    udpsniff/exec_options/priv_exec_options.h \
    udpsniff/exec_options/priv_exec_option1.h \
    udpsniff/exec_options/priv_exec_option2.h \
    common.h               \
    netinet_helper.c       \
    netinet_helper.h       \
    $(IPC_SRC)
udp_sniff_LDFLAGS = -pthread

#print_stats_SOURCES =           \
#    printstats/main.c           \
#    printstats/printstats.c     \
#    printstats/printstats.h     \
#    common.h                    \
#    netinet_helper.c            \
#    netinet_helper.h            \
#    $(IPC_SRC)
