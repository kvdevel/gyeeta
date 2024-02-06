//  SPDX-FileCopyrightText: 2022 Exact Solutions, Inc.
//  SPDX-License-Identifier: GPL-3.0-or-later

#include			"gy_common_inc.h"
#include			"gy_tls_proto.h"

using namespace gyeeta;

const char client_hello[] = 
"\x16\x03\x01\x02\x00\x01\x00\x01\xfc\x03\x03\x04\x68\xa6\x72\xfc" \
"\xab\xb2\x61\x7e\x30\x86\x74\x00\x51\x2b\x4f\x66\xd8\x2b\xfb\x6a" \
"\x0f\xab\xe3\xc7\x18\x4a\x85\x17\xe7\x2f\x37\x20\xd4\x37\x1e\x31" \
"\x64\xbc\xac\x89\x1d\xfb\x5c\xe4\xe8\x71\xc2\xd6\x54\x11\x1b\x12" \
"\x5c\x9d\x77\xd0\x02\xf0\x98\x4a\xff\x65\xa1\x79\x00\x1a\x13\x01" \
"\x13\x02\x13\x03\x13\x04\xc0\x2b\xc0\x2f\xc0\x2c\xc0\x30\xcc\xa9" \
"\xcc\xa8\x00\x9e\x00\x9f\x00\xff\x01\x00\x01\x99\x00\x0b\x00\x04" \
"\x03\x00\x01\x02\x00\x0a\x00\x0a\x00\x08\x00\x1d\x00\x17\x00\x18" \
"\x00\x19\x00\x23\x00\x00\x33\x74\x00\x00\x00\x10\x00\x1a\x00\x18" \
"\x02\x68\x32\x05\x68\x32\x2d\x31\x36\x05\x68\x32\x2d\x31\x34\x08" \
"\x68\x74\x74\x70\x2f\x31\x2e\x31\x00\x16\x00\x00\x00\x17\x00\x00" \
"\x00\x0d\x00\x2a\x00\x28\x04\x03\x05\x03\x06\x03\x08\x07\x08\x08" \
"\x08\x09\x08\x0a\x08\x0b\x08\x04\x08\x05\x08\x06\x04\x01\x05\x01" \
"\x06\x01\x03\x03\x03\x01\x03\x02\x04\x02\x05\x02\x06\x02\x00\x2b" \
"\x00\x09\x08\x03\x04\x03\x03\x03\x02\x03\x01\x00\x2d\x00\x02\x01" \
"\x01\x00\x33\x00\x26\x00\x24\x00\x1d\x00\x20\x09\x7a\x8a\xfa\xda" \
"\x79\xc9\x82\xde\xda\x5a\x1f\x4f\x7c\x91\x33\x6d\x6f\x15\x37\xf5" \
"\x21\xfd\x9a\x0a\xf1\xd2\x49\xff\xb1\xcd\x4e\x00\x15\x00\xe6\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00"
;

const char server_hello[] =
"\x16\x03\x03\x00\x58\x02\x00\x00\x54\x03\x03\xcf\x21\xad\x74\xe5" \
"\x9a\x61\x11\xbe\x1d\x8c\x02\x1e\x65\xb8\x91\xc2\xa2\x11\x16\x7a" \
"\xbb\x8c\x5e\x07\x9e\x09\xe2\xc8\xa8\x33\x9c\x20\xd4\x37\x1e\x31" \
"\x64\xbc\xac\x89\x1d\xfb\x5c\xe4\xe8\x71\xc2\xd6\x54\x11\x1b\x12" \
"\x5c\x9d\x77\xd0\x02\xf0\x98\x4a\xff\x65\xa1\x79\x13\x02\x00\x00" \
"\x0c\x00\x2b\x00\x02\x03\x04\x00\x33\x00\x02\x00\x17\x14\x03\x03" \
"\x00\x01\x01"
;


const char change_cipher[] = 
"\x14\x03\x03\x00\x01\x01"
;

const char app_data[] =
"\x17\x03\x03\x01\x00\x81\xd1\x90\x20\xf5\xc8\x0a\x28\x8a\xeb\xc9" \
"\xe8\xc4\xf5\x11\x41\xec\xaa\xdd\x11\x58\xb7\xc2\x51\x72\xfb\xeb" \
"\xeb\x61\xaf\xd0\x03\x48\xb0\x02\x5d\x84\x29\x87\x7b\xcd\x11\x62" \
"\xf0\xe5\xa5\x3b\x92\x0a\x7c\xcb\x96\x00\x05\x85\x66\x8e\x88\x96" \
"\x38\x63\x37\x3f\x46\x12\xb7\xfa\x6f\xfa\x83\xbd\x00\xe0\x2a\x30" \
"\x75\x99\x02\xab\xf4\x2f\x91\x30\xb6\x6c\xa4\x66\x15\x58\x87\x0f" \
"\x0e\xbb\xb3\x94\x75\x73\xd4\x3e\x03\xed\x63\x3f\xec\xfe\x3f\x9d" \
"\x98\x02\x83\x2f\x94\xaa\xc4\x31\x2b\xe9\x99\x4f\x61\x19\xde\xb5" \
"\x7c\xf7\x88\x03\x04\xd2\xdf\xb1\x0f\x0c\x81\x56\x99\xda\x30\x8d" \
"\x0e\x5c\xb2\x1f\x51\x7b\x77\xed\xeb\x33\x51\x8b\xb7\x21\x51\x82" \
"\x33\x44\x64\x85\x97\x6d\xac\x27\xe7\x8e\xab\x9b\x82\x94\xc7\xaa" \
"\xaf\x51\x55\x59\x10\xb0\x34\x42\x55\x01\xb8\x30\x77\x5f\x1d\xfb" \
"\xe3\x64\x25\x47\x38\x77\xaf\xbe\xc8\xd8\x90\x99\xd1\x6a\xe4\x03" \
"\xcd\x4f\xa3\x81\xe0\xdd\xb1\x11\x69\xd0\x50\x87\x85\x9a\xf1\xbd" \
"\x2c\x4f\x4a\xc8\x5c\x01\x0e\x99\x28\xda\xee\xa8\x52\x99\x87\xc1" \
"\x4b\xf2\xda\x05\x4a\x32\x93\x52\xe6\xcc\x78\x0c\xbd\xfa\x36\xe0" \
"\xe0\x7e\x3a\xa3\x05"
;

int main()
{
	char				combuf[sizeof(server_hello) - 1 + sizeof(change_cipher)];

	std::memcpy(combuf, server_hello, sizeof(server_hello) - 1);
	std::memcpy(combuf + sizeof(server_hello) - 1, change_cipher, sizeof(change_cipher));

	assert(true == TLS_PROTO::is_tls_req_resp((const uint8_t *)client_hello, sizeof(client_hello) - 1, DirPacket::DirInbound, true /* is_init */));
	assert(true == TLS_PROTO::is_tls_req_resp((const uint8_t *)server_hello, sizeof(server_hello) - 1, DirPacket::DirOutbound, true /* is_init */));
	assert(true == TLS_PROTO::is_tls_req_resp((const uint8_t *)change_cipher, sizeof(change_cipher) - 1, DirPacket::DirInbound, false /* is_init */));
	assert(true == TLS_PROTO::is_tls_req_resp((const uint8_t *)app_data, sizeof(app_data) - 1, DirPacket::DirOutbound, false /* is_init */));

	assert(true == TLS_PROTO::is_tls_req_resp((const uint8_t *)combuf, sizeof(combuf) - 1, DirPacket::DirOutbound, true /* is_init */));

	assert(false == TLS_PROTO::is_tls_req_resp((const uint8_t *)app_data, sizeof(app_data) - 1, DirPacket::DirOutbound, true /* is_init */));
	assert(false == TLS_PROTO::is_tls_req_resp((const uint8_t *)server_hello, sizeof(server_hello) - 1, DirPacket::DirInbound, true /* is_init */));
	assert(false == TLS_PROTO::is_tls_req_resp((const uint8_t *)change_cipher, sizeof(change_cipher) - 1, DirPacket::DirInbound, true /* is_init */));
}	

