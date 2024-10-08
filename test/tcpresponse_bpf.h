//  SPDX-FileCopyrightText: 2022 Exact Solutions, Inc.
//  SPDX-License-Identifier: GPL-3.0-or-later


#ifndef _TCPRESPONSE_BPF_H
#define _TCPRESPONSE_BPF_H

#include			"../common/gy_ebpf_bpf_common.h"

struct tcp_ipv4_resp_event_t 
{
	struct ipv4_tuple_t	tup;
	u64			bytes_received;
	u64			bytes_acked;
	u32			lsndtime;
	u32			lrcvtime;
};

struct tcp_ipv6_resp_event_t 
{
	struct ipv6_tuple_t	tup;
	u64			bytes_received;
	u64			bytes_acked;
	u32			lsndtime;
	u32			lrcvtime;
};


enum {
	ARR_CONFIG_ENABLE 	= 0,
	ARR_SKIPPED_IPv4	= 1,
	ARR_SKIPPED_IPv6	= 2,

	ARR_MAX_KEYS,
};	

#endif
