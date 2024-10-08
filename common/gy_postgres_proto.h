//  SPDX-FileCopyrightText: 2022 Exact Solutions, Inc.
//  SPDX-License-Identifier: GPL-3.0-or-later

#pragma				once

#include			"gy_common_inc.h"
#include			"gy_misc.h"

namespace gyeeta {

class API_PARSE_HDLR;
struct PARSE_PKT_HDR;
class SVC_SESSION;
class POSTGRES_SESSINFO;

class POSTGRES_PROTO 
{
public :
	/*
	 * Refer to https://www.postgresql.org/docs/current/protocol-message-formats.html
	 */
	enum PG_MSG_TYPES_E : char 
	{
		MSG_FB_UNKNOWN 			= 0,

		// Frontend (Request)
		MSG_F_QUERY 			= 'Q',
		MSG_F_CLOSE 			= 'C',
		MSG_F_COPYFAIL 			= 'f',
		MSG_F_PARSE 			= 'P',
		MSG_F_BIND 			= 'B',
		MSG_F_DESCRIBE 			= 'D',
		MSG_F_EXECUTE 			= 'E',
		MSG_F_SYNC 			= 'S',
		MSG_F_FLUSH 			= 'H',
		MSG_F_PASSWD 			= 'p',
		MSG_F_FUNCTION_CALL		= 'F',
		MSG_F_TERMINATE 		= 'X',

		// Dummy Msg Type for Startup
		MSG_F_STARTUP			= '\x07',

		// Backend (Response)
		MSG_B_DATAROW 			= 'D',
		MSG_B_READY_FOR_QUERY 		= 'Z',
		MSG_B_COPYOUT_RESP 		= 'H',
		MSG_B_COPYIN_RESP 		= 'G',
		MSG_B_COPYBOTH_RESP 		= 'W',
		MSG_B_ERROR_RESP 		= 'E',
		MSG_B_CMD_COMPLETE 		= 'C',
		MSG_B_CLOSE_COMPLETE 		= '3',
		MSG_B_PARSE_COMPLETE 		= '1',
		MSG_B_BIND_COMPLETE 		= '2',
		MSG_B_KEYDATA 			= 'K',
		MSG_B_PARAM_DESC 		= 't',
		MSG_B_PARAM_STATUS 		= 'S',
		MSG_B_ROW_DESC 			= 'T',
		MSG_B_PORTAL_SUSPENDED 		= 's',
		MSG_B_EMPTY_RESPONSE 		= 'I',
		MSG_B_AUTH_RESP			= 'R',
		MSG_B_NODATA 			= 'n',
		MSG_B_NOTICE_RESP		= 'N',
		MSG_B_NOTIFY_RESP		= 'A',
		MSG_B_FUNCTION_CALL_RESP	= 'V',
		MSG_B_NEGOTIATE_PROTO		= 'v',

		// Frontend & Backend
		MSG_FB_COPYDATA 		= 'd',
		MSG_FB_COPYDONE 		= 'c',

	};	

	enum : int
	{
		MSG_F_AUTH_START		= 196608,		// 0x00030000
		MSG_F_CANCEL			= 80877102,		// 0x04d2162e
		MSG_F_SSL_REQ			= 80877103,		// 0x04d2162f
		MSG_F_GSSENC_REQ		= 80877104,		// 0x04d21630
	};

	enum PG_AUTH_RESP_E : int
	{
		B_AUTH_SUCCESS			= 0,
		B_AUTH_KERBEROS			= 2,
		B_AUTH_CLEARTEXT_PWD		= 3,
		B_AUTH_MD5_PWD			= 5,
		B_AUTH_GSSAPI			= 7,
		B_AUTH_SSPI			= 9,
		B_AUTH_SASL			= 10,
	};	

	/*
	 * tran_type definitions :
	 */
	enum PG_TRAN_TYPES_E : int
	{
		TYPE_PG_LOGIN			= 1 << 0,
		TYPE_PG_NORMAL_REQ		= 1 << 1,
		TYPE_PG_FUNCTION_CALL		= 1 << 2,
		TYPE_PG_DSQL_PREPARE		= 1 << 3,
		TYPE_PG_DSQL_EXEC		= 1 << 4,
		TYPE_PG_DSQL_BIND		= 1 << 5,
		TYPE_PG_DSQL_DEALLOC		= 1 << 6,
		TYPE_PG_DSQL_DESCRIBE		= 1 << 7,
		TYPE_PG_BULK_TRAN		= 1 << 8,
		TYPE_PG_CANCEL_QUERY		= 1 << 9,

		TYPE_PG_MISC			= 1 << 30,
		TYPE_PG_LOGOUT			= 1 << 31,
	};


	/*
	 * select typname,oid from pg_type;
	 */ 
	enum PG_OID_E : int 
	{
		OID_PG_BOOL			=    16,
		OID_PG_BYTEA			=    17,
		OID_PG_CHAR			=    18,
		OID_PG_NAME			=    19,
		OID_PG_INT8			=    20,
		OID_PG_INT2			=    21,
		OID_PG_INT2VECTOR		=    22,
		OID_PG_INT4			=    23,
		OID_PG_REGPROC			=    24,
		OID_PG_TEXT			=    25,
		OID_PG_OID			=    26,
		OID_PG_TID			=    27,
		OID_PG_XID			=    28,
		OID_PG_CID			=    29,
		OID_PG_OIDVECTOR		=    30,
		OID_PG_JSON			=   114,
		OID_PG_XML			=   142,
		OID_PG_POINT			=   600,
		OID_PG_LSEG			=   601,
		OID_PG_PATH			=   602,
		OID_PG_BOX			=   603,
		OID_PG_POLYGON			=   604,
		OID_PG_LINE			=   628,
		OID_PG_FLOAT4			=   700,
		OID_PG_FLOAT8			=   701,
		OID_PG_UNKNOWN			=   705,
		OID_PG_CIRCLE			=   718,
		OID_PG_MONEY			=   790,
		OID_PG_MACADDR			=   829,
		OID_PG_INET			=   869,
		OID_PG_CIDR			=   650,
		OID_PG_MACADDR8			=   774,
		OID_PG_ACLITEM			=  1033,
		OID_PG_BPCHAR			=  1042,
		OID_PG_VARCHAR			=  1043,
		OID_PG_DATE			=  1082,
		OID_PG_TIME			=  1083,
		OID_PG_TIMESTAMP		=  1114,
		OID_PG_TIMESTAMPTZ		=  1184,
		OID_PG_INTERVAL			=  1186,
		OID_PG_TIMETZ			=  1266,
		OID_PG_BIT			=  1560,
		OID_PG_VARBIT			=  1562,
		OID_PG_NUMERIC			=  1700,
		OID_PG_REFCURSOR		=  1790,
		OID_PG_UUID			=  2950,
		OID_PG_TSVECTOR			=  3614,
		OID_PG_GTSVECTOR		=  3642,
		OID_PG_TSQUERY			=  3615,
		OID_PG_JSONB			=  3802,
		OID_PG_JSONPATH			=  4072,
		OID_PG_INT4RANGE		=  3904,
		OID_PG_NUMRANGE			=  3906,
		OID_PG_TSRANGE			=  3908,
		OID_PG_TSTZRANGE		=  3910,
		OID_PG_DATERANGE		=  3912,
		OID_PG_INT8RANGE		=  3926,
		OID_PG_INT4MULTIRANGE		=  4451,
		OID_PG_NUMMULTIRANGE		=  4532,
		OID_PG_TSMULTIRANGE		=  4533,
		OID_PG_TSTZMULTIRANGE		=  4534,
		OID_PG_DATEMULTIRANGE		=  4535,
		OID_PG_INT8MULTIRANGE		=  4536,
		OID_PG_XID8			=  5069,
		OID_PG_RECORD			=  2249,
		OID_PG_CSTRING			=  2275,
		OID_PG_VOID			=  2278,
	};

	/*
	 * We currently only supports a small subset of binary param types
	 */
	static inline bool binary_param_ok(PG_OID_E oid) noexcept
	{
		switch (oid) {

		case OID_PG_CHAR : 
		case OID_PG_INT8 : 
		case OID_PG_INT2 : 
		case OID_PG_INT4 : 
		case OID_PG_FLOAT4 : 
		case OID_PG_FLOAT8 : 
		case OID_PG_TEXT : 
		case OID_PG_VARCHAR : 
		case OID_PG_CSTRING : 
			return true;

		default :
			return false;
		}	
	}	

	static inline bool is_int_param(PG_OID_E oid) noexcept
	{
		switch (oid) {

		case OID_PG_INT8 : 
		case OID_PG_INT2 : 
		case OID_PG_INT4 : 
			return true;

		default :
			return false;
		}	
	}	

	static inline bool is_float_param(PG_OID_E oid) noexcept
	{
		return (oid == OID_PG_FLOAT4 || oid == OID_PG_FLOAT8);
	}	
	

	static tribool is_valid_req(const uint8_t *pdata, uint32_t caplen, uint32_t wirelen, bool is_init = false, LAST_PKT_SNIPPET *plastsnippet = nullptr) noexcept
	{
		if (is_init) {
			uint32_t			version, msglen;

			if (caplen < 8) {
				return false;
			}	

			msglen = unaligned_read_be32(pdata);
			if (msglen != caplen) {
				return false;
			}

			version = unaligned_read_be32(pdata + 4);

			if ((version & 0xFFFFFF00) == MSG_F_AUTH_START) {
				if (caplen < 12) {
					return false;
				}	

				int				maxlen = caplen - 8, len;
				const char			*ptmp = (const char *)(pdata + 8);
	
				do {
					if ((0 == strcmp(ptmp, "user")) || (0 == strcmp(ptmp, "database")) || (0 == strcmp(ptmp, "options")) || 
						(0 == strcmp(ptmp, "application_name")) || (0 == strcmp(ptmp, "replication"))) {
						return true;
					}	
					else {
						len = strnlen(ptmp, maxlen);

						ptmp += len + 1;
						maxlen -= len + 1;

						if (maxlen < 1) {
							break;
						}	

						len = strnlen(ptmp, maxlen);

						ptmp += len + 1;
						maxlen -= len + 1;
					}	

				} while (maxlen > 0);
				
				return false;
			}
			else if (version == MSG_F_CANCEL) {
				if (caplen != 16) {
					return false;
				}	

				return true;
			}	
			else if (version == MSG_F_SSL_REQ || version == MSG_F_GSSENC_REQ) {
				if (caplen == 8) {
					return true;
				}

				return false;
			}	
			
			return false;
		}

		if (caplen < 5) {
			return false;
		}	
		
		int				maxlen = caplen, ntkns = 0;
		const char			*pstart = (const char *)pdata;

		do {

			uint32_t			tknlen;
			char				tkn = *pstart;

			tknlen = unaligned_read_be32(pstart + 1);

			if (tknlen < 4) {
				return false;
			}	

			// Not a protocol limit...
			if (tknlen > GY_UP_MB(128)) {
				return false;
			}	

			ntkns++;

			switch (tkn) {

			case MSG_F_QUERY :
			case MSG_F_CLOSE :
			case MSG_F_COPYFAIL :
			case MSG_F_PARSE :
			case MSG_F_BIND :
			case MSG_F_DESCRIBE :
			case MSG_F_EXECUTE :
			case MSG_F_PASSWD :
			case MSG_F_FUNCTION_CALL :
			case MSG_FB_COPYDATA :
			
				if (tknlen < 5) return false;
				break;
				
			case MSG_F_SYNC :
			case MSG_F_FLUSH :
			case MSG_F_TERMINATE :
			case MSG_FB_COPYDONE :
				if (tknlen != 4) return false;
				break;

			default :
				return false;
			}	

			pstart += tknlen + 1;
			maxlen -= (tknlen + 1);


		} while (maxlen >= 5);

		if (maxlen == 0) {
			return true;
		}

		if (ntkns > 2) {
			return true;
		}

		return indeterminate;
	}	

	static tribool is_valid_resp(const uint8_t *pdata, uint32_t caplen, uint32_t wirelen, bool is_init = false, LAST_PKT_SNIPPET *plastsnippet = nullptr) noexcept
	{
		if (is_init) {
			if (caplen >= 9) {
				uint32_t			msglen, type;
				char				c = (char)*pdata;

				if (c != MSG_B_AUTH_RESP) {
					return false;
				}	

				msglen = unaligned_read_be32(pdata + 1);

				if (caplen != msglen + 1) {
					return false;
				}

				type = unaligned_read_be32(pdata + 5);

				if (type == B_AUTH_SUCCESS || type == B_AUTH_KERBEROS || type == B_AUTH_CLEARTEXT_PWD 
					|| type == B_AUTH_MD5_PWD || type == B_AUTH_GSSAPI || type == B_AUTH_SSPI || type == B_AUTH_SASL) {
					
					return true;
				}	

				return false;
			}
			else if (caplen == 1) {
				char				c = (char)*pdata;

				if (c == 'S' || c == 'N') {
					// Maybe SSL Resp
					return indeterminate;
				}	
			}	

			return false;
		}

		if (caplen < 5) {
			return false;
		}	
		
		int				maxlen = caplen, ntkns = 0;
		const char			*pstart = (const char *)pdata;

		do {
			uint32_t			tknlen;
			char				tkn = *pstart, c;

			tknlen = unaligned_read_be32(pstart + 1);

			if (tknlen < 4) {
				return false;
			}	

			// Not a protocol limit...
			if (tknlen > GY_UP_MB(128)) {
				return false;
			}	

			ntkns++;

			switch (tkn) {

			case MSG_B_READY_FOR_QUERY :

				if (tknlen != 5) return false;

				if (maxlen >= (int)tknlen + 1) {
					c = pstart[5];
					if (c != 'I' && c != 'T' && c != 'E') return false;
				}

				break;

			case MSG_B_ERROR_RESP :
			case MSG_B_CMD_COMPLETE :

				if (tknlen <= 5) return false;

				if (maxlen > 5) {
					c = pstart[5];
					if (!(c >= 'A' && c <= 'Z')) return false;
				}

				break;
			
			case MSG_B_KEYDATA :

				if (tknlen != 12) return false;
				break;

			case MSG_B_DATAROW :
			case MSG_B_PARAM_DESC :
			case MSG_B_ROW_DESC :

				if (tknlen < 6) return false;
				break;

			case MSG_B_FUNCTION_CALL_RESP :

				if (tknlen < 8) return false;
				break;

			case MSG_B_AUTH_RESP :
			case MSG_B_COPYOUT_RESP :
			case MSG_B_COPYIN_RESP :
			case MSG_B_COPYBOTH_RESP :
			case MSG_B_NEGOTIATE_PROTO :	
			case MSG_B_NOTICE_RESP :
			case MSG_B_NOTIFY_RESP :
			case MSG_B_PARAM_STATUS :
			case MSG_FB_COPYDATA :

				if (tknlen < 5) return false;
				break;

			case MSG_B_CLOSE_COMPLETE :
			case MSG_B_PARSE_COMPLETE :
			case MSG_B_BIND_COMPLETE :
			case MSG_B_PORTAL_SUSPENDED :
			case MSG_B_EMPTY_RESPONSE :
			case MSG_B_NODATA :
			case MSG_FB_COPYDONE :

				if (tknlen != 4) return false;
				break;

			default :
				return false;
			}	

			pstart += tknlen + 1;
			maxlen -= (tknlen + 1);

		} while (maxlen >= 5);

		if (maxlen == 0) {
			return true;
		}

		if (ntkns > 2) {
			return true;
		}

		return indeterminate;
		
	}	

	static tribool is_valid_req_resp(const uint8_t *pdata, uint32_t caplen, uint32_t wirelen, DirPacket dir, bool is_init = false, LAST_PKT_SNIPPET *plastsnippet = nullptr) noexcept
	{
		if (dir == DirPacket::DirInbound) {
			return is_valid_req(pdata, caplen, wirelen, is_init, plastsnippet);
		}	

		return is_valid_resp(pdata, caplen, wirelen, is_init, plastsnippet);
	}	


	API_PARSE_HDLR				& apihdlr_;
	uint32_t				api_max_len_;
	uint32_t				max_pg_req_token_;
	uint32_t				max_pg_resp_token_;

	POSTGRES_PROTO(API_PARSE_HDLR & apihdlr, uint32_t api_max_len);

	~POSTGRES_PROTO() noexcept;
	
	POSTGRES_PROTO(const POSTGRES_PROTO & other) noexcept			= default;

	POSTGRES_PROTO & operator=(const POSTGRES_PROTO & other) noexcept	= default;

	void handle_request_pkt(POSTGRES_SESSINFO & sess, SVC_SESSION & svcsess, PARSE_PKT_HDR & hdr, uint8_t *pdata);

	void handle_response_pkt(POSTGRES_SESSINFO & sess, SVC_SESSION & svcsess, PARSE_PKT_HDR & hdr, uint8_t *pdata);

	void handle_session_end(POSTGRES_SESSINFO & sess, SVC_SESSION & svcsess, PARSE_PKT_HDR & hdr);

	void handle_ssl_change(POSTGRES_SESSINFO & sess, SVC_SESSION & svcsess, PARSE_PKT_HDR & hdr, uint8_t *pdata);

	std::pair<POSTGRES_SESSINFO *, void *> alloc_sess(SVC_SESSION & svcsess, PARSE_PKT_HDR & hdr);

	void destroy(POSTGRES_SESSINFO *pobj, void *pdata) noexcept;

	static void print_stats(STR_WR_BUF & strbuf, time_t tcur, time_t tlast) noexcept;

	API_PARSE_HDLR & get_api_hdlr() noexcept
	{
		return apihdlr_;
	}

	uint32_t get_api_max_len() const noexcept
	{
		return api_max_len_;
	}	


};

} // namespace gyeeta

