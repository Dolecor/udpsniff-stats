bin_PROGRAMS = udp-sniff print-stats

udp_sniff_SOURCES =         \
    udpsniff/main.c         \
    udpsniff/control.c      \
    udpsniff/control.h      \
    udpsniff/mq_interface.c \
    udpsniff/mq_interface.h \
    udpsniff/exec_options/exec_option.c       \
    udpsniff/exec_options/exec_option1.c      \
    udpsniff/exec_options/exec_option2.c      \
    udpsniff/exec_options/exec_options.h      \
    udpsniff/exec_options/priv_exec_options.h \
    udpsniff/exec_options/priv_exec_option1.h \
    udpsniff/exec_options/priv_exec_option2.h \
    common.h               \
    mq_common.c            \
    mq_common.h            \
    netinet_helper.c       \
    netinet_helper.h
udp_sniff_LDFLAGS = -pthread
udp_sniff_LDADD = -lrt

print_stats_SOURCES =           \
    printstats/main.c           \
    printstats/printstats.c     \
    printstats/printstats.h     \
    printstats/mq_interface.c   \
    printstats/mq_interface.h   \
    common.h                    \
    mq_common.c                 \
    mq_common.h                 \
    netinet_helper.c            \
    netinet_helper.h
print_stats_LDADD = -lrt
