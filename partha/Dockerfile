# syntax=docker/dockerfile:1

FROM ubuntu:20.04

LABEL org.opencontainers.image.description="This container provides partha which is the Gyeeta Host Monitor Agent to be installed on all hosts which need to be monitored"

LABEL usage="docker run -td --rm --name partha --read-only --privileged --env CFG_JSON_FILE=/tmp/cfg.json -v /var/HOST_PATH_TO_CFG/partha_cfg.json:/tmp/cfg.json:ro  --network=host --pid=host --cgroupns=host -v parthavol:/hostdata -v /proc:/proc -v /sys/:/sys/ <partha Image>"

# LABEL for github repository link
LABEL org.opencontainers.image.source="https://github.com/gyeeta/gyeeta"

LABEL org.opencontainers.image.authors="https://github.com/gyeeta"

ARG PARTHA_VERSION
ENV PARTHA_VERSION=${PARTHA_VERSION}

RUN apt-get update \
	&& apt-get install -y --no-install-recommends \
	sudo libcap2-bin curl ca-certificates \
	&& rm -rf /var/lib/apt/lists/*

ENV SSL_CERT_DIR=/etc/ssl/certs

# tini handling...
ARG TINI_VERSION=v0.19.0
ARG TINI_SHA256="93dcc18adc78c65a028a84799ecf8ad40c936fdfc5f2a57b1acda5a8117fa82c"
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /tini
RUN chmod 0755 /tini
RUN if [ `sha256sum /tini | awk '{print $1}'` != "$TINI_SHA256" ]; then echo -e "ERROR : SHA256 of tini is different from expected value. Binary has changed. Please contact on Github.\n\n"; return 1; else return 0; fi

# Volume for partha logs and tmp dir
VOLUME ["/hostdata"]

RUN mkdir /hostdata 2> /dev/null; rmdir /lib/modules 2> /dev/null; ln -sf /hostdata/kernelsrc /lib/modules; rmdir /usr/src 2> /dev/null; ln -sf /proc/1/root/usr/src /usr/

COPY . /partha/

RUN sudo -n setcap cap_chown,cap_dac_override,cap_dac_read_search,cap_fowner,cap_fsetid,cap_ipc_lock,cap_kill,cap_mac_admin,cap_mknod,cap_sys_chroot,cap_sys_resource,cap_setpcap,cap_sys_ptrace,cap_sys_admin,cap_net_admin,cap_net_raw,cap_sys_module,cap_setuid+ep /partha/partha-bpf && sudo -n setcap cap_chown,cap_dac_override,cap_dac_read_search,cap_fowner,cap_fsetid,cap_ipc_lock,cap_kill,cap_mac_admin,cap_mknod,cap_sys_chroot,cap_sys_resource,cap_setpcap,cap_sys_ptrace,cap_sys_admin,cap_net_admin,cap_net_raw,cap_sys_module,cap_setuid+ep /partha/partha-bcc

ENTRYPOINT ["/tini", "-s", "-g", "--", "/partha/container_init.sh" ]

CMD ["start"]

