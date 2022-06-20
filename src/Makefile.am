bin_PROGRAMS = udp-sniff print-stats

udp_sniff_SOURCES = \
    udpsniff/main.c \
    netinet_helper.c \
    udpsniff/control.c \
    udpsniff/exec_options/exec_option.c \
    udpsniff/exec_options/exec_option1.c \
    udpsniff/exec_options/exec_option2.c \
    mq_common.c \
    udpsniff/mq_interface.c
udp_sniff_LDFLAGS = -pthread
udp_sniff_LDADD = -lrt

print_stats_SOURCES = \
    printstats/main.c \
    netinet_helper.c \
    printstats/printstats.c \
    mq_common.c \
    printstats/mq_interface.c
print_stats_LDADD = -lrt
