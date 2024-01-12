//  SPDX-FileCopyrightText: 2022 Exact Solutions, Inc.
//  SPDX-License-Identifier: GPL-3.0-or-later

#pragma				once

#include			"gy_common_inc.h"
#include			"gy_comm_proto.h"

namespace gyeeta {

static constexpr uint32_t			MAX_PARSE_API_LEN	{16 * 1024};	
static constexpr uint32_t			DFLT_MAX_API_LEN	{4096};
static constexpr uint32_t			MAX_PARSE_EXT_LEN	{1400};	

enum PROTO_TYPES : uint16_t 
{
	PROTO_UNINIT				= 0,

	PROTO_HTTP1,
	PROTO_HTTP2,
	PROTO_POSTGRES,
	PROTO_MYSQL,
	PROTO_MONGO,
	PROTO_REDIS,

	PROTO_UNKNOWN
};	

static constexpr const char * proto_to_string(PROTO_TYPES proto) noexcept
{
	constexpr const char		*protostr[PROTO_UNKNOWN + 1] = {
		[PROTO_UNINIT] = "Uninitialized", [PROTO_HTTP1] = "HTTP1.x", [PROTO_HTTP2] = "HTTP2", 
		[PROTO_POSTGRES] = "Postgres", [PROTO_MYSQL] = "MySQL", [PROTO_MONGO] = "Mongo", 
		[PROTO_REDIS] = "Redis", 

		[PROTO_UNKNOWN] = "Unknown",
	};	

	if ((unsigned)proto < PROTO_UNKNOWN) {
		return protostr[proto] ? protostr[proto] : "";
	}	

	return "Invalid";
}	

enum PARSE_FIELD_E : uint16_t
{
	FIELD_NONE				= 0,

	FIELD_APPNAME				= 1,		/* AppName : Data Type char* */
	FIELD_USERNAME				= 2,		/* UserName : Data Type char* */
	FIELD_DBNAME				= 3,		/* DBName : Data Type char* */
	FIELD_ERRTXT				= 4,		/* Error Text : Data Type char* */
	FIELD_ERRCLASS				= 5,		/* Error Class : Data Type uint16_t */
	FIELD_SESSID				= 6,		/* DB Session ID : Data Type uint32_t */
	FIELD_PREP_REQNUM			= 7,		/* DB Prepare SQL reqnum_ : Data Type uint64_t */
	FIELD_PREP_REQTIME			= 8,		/* DB Prepare SQL treq : Data Type time_t */
	FIELD_HOSTPID				= 9,		/* DB Process ID : Data Type uint32_t */
	FIELD_NROWS				= 10,		/* Number of rows returned : Data Type uint32_t */
	FIELD_CURSORNAME			= 11,		/* DB Cursor Name : Data Type char* */
	
	FIELD_CONTENTTYPE,					/* HTTP Content Type : Data Type char* */

	FIELD_MAX,
};	

static constexpr const char			*parse_field_str[FIELD_MAX] =
{
	"", "appname", "username", "dbname", "errtxt", "errclass", "sessid", "prep_reqnum", "prep_reqtime", "hostpid",
	"nrows", "cursorname", "contenttype",
};	


/*
 * NOTE : There is no padding added and direct dereference is not to be done...
 */
struct PARSE_FIELD_LEN
{
	PARSE_FIELD_E				field_		{FIELD_NONE};
	uint16_t				len_		{0};

	/*uint8_t				field_[len_] follows;*/		// No padding done

	PARSE_FIELD_LEN() noexcept		= default;

	PARSE_FIELD_LEN(PARSE_FIELD_E field, uint16_t len) noexcept
		: field_(field), len_(len)
	{}	
};	


struct alignas(8) API_TRAN
{
	uint64_t				treq_usec_	{0};		/* Time of Request Start */	
	uint64_t				tres_usec_	{0};		/* Time of Response Start */	
	uint64_t				tupd_usec_	{0};		/* last in/out data timestamp for processing */
	uint64_t				tin_usec_	{0};		/* last data inbound time */

	uint64_t				reqlen_		{0};		/* Total Bytes In for Request */
	uint64_t				reslen_		{0};		/* Total Bytes Out for Request */
	uint64_t				reqnum_		{0};		/* Session Request Counter from 1 onwards */
	uint64_t				response_usec_	{0};		/* Response Time in usec */
	uint64_t				reaction_usec_	{0};		/* Reaction Time in usec */
	uint64_t				tconnect_usec_	{0};		/* Session Connect Time */

	GY_IP_ADDR				cliip_;
	GY_IP_ADDR				serip_;
	uint64_t				glob_id_	{0};
	
	int					errorcode_	{0};
	uint32_t				app_sleep_ms_	{0};
	uint32_t				tran_type_	{0};
	
	PROTO_TYPES				proto_		{PROTO_UNINIT};
	uint16_t				cliport_	{0};
	uint16_t				serport_	{0};

	// Succeeding Payload info
	uint16_t				request_len_	{0};		/* Request Length (includes '\0' byte) : Directly follows after this struct */

	// Length of extra data following the Reqstring : 1 byte (next_fields_) + next_fields_ * sizeof(PARSE_FIELD_LENs) + data 
	uint16_t				lenext_		{0};		

	uint8_t					padlen_		{0};		/* Pad to 8 bytes */

	/*char					request_[request_len_] follows;*/
	/*uint8_t				ext_[lenext_] follows;*/
	/*char					padding_[padding_len_]; follows to make the entire 8 byte aligned */

	static constexpr size_t			MAX_NUM_REQS 	{256};		// Send in batches

	API_TRAN() noexcept			= default;

	API_TRAN(uint64_t tlastpkt_usec, uint64_t tconnect_usec, const IP_PORT & cli_ipport, const IP_PORT & ser_ipport, uint64_t glob_id, PROTO_TYPES proto) noexcept
		: treq_usec_(tlastpkt_usec), tupd_usec_(treq_usec_), tin_usec_(treq_usec_), tconnect_usec_(tconnect_usec),
		cliip_(cli_ipport.ipaddr_), serip_(ser_ipport.ipaddr_), glob_id_(glob_id), proto_(proto), cliport_(cli_ipport.port_), serport_(ser_ipport.port_)
	{}

	~API_TRAN() noexcept			= default;

	// Returns the Request string len and ext fields len and padding
	uint32_t get_elem_size() const noexcept
	{
		return get_act_len() + padlen_;
	}	

	// Includes sizeof(*this)
	uint32_t get_act_len() const noexcept
	{
		return sizeof(*this) + request_len_ + lenext_;
	}	

	void set_padding_len() noexcept
	{
		size_t 			currsz, newsz;

		currsz = get_act_len();
		newsz = gy_align_up_2(currsz, 8);

		padlen_			= newsz - currsz;
	}

	uint8_t get_pad_len() const noexcept
	{
		return padlen_;
	}	

	static inline uint32_t get_max_elem_size(uint32_t api_max_len) noexcept
	{
		return sizeof(API_TRAN) + api_max_len + MAX_PARSE_EXT_LEN + 32;
	}	

	void reset() noexcept
	{
		/* Reset specific fields only */

		tres_usec_			= 0;
		tupd_usec_			= 0;
		reqlen_				= 0;
		reslen_				= 0;
		reqnum_				= 0;
		response_usec_			= 0;
		reaction_usec_			= 0;
		errorcode_			= 0;
		app_sleep_ms_			= 0;
		tran_type_			= 0;
		request_len_			= 0;
		lenext_				= 0;
		padlen_				= 0;
	}	

	void update_req_stats(uint64_t tlastpkt_usec, uint32_t datalen) noexcept
	{
		tupd_usec_		= tlastpkt_usec;
		tin_usec_		= tlastpkt_usec;
		reqlen_			+= datalen;
	}	

	void update_resp_stats(uint64_t tlastpkt_usec, uint32_t datalen) noexcept
	{
		tupd_usec_		= tlastpkt_usec;

		if (tres_usec_ == 0) {
			tres_usec_	= tlastpkt_usec;
		}

		reslen_			+= datalen;
	}	

	void set_resp_times() noexcept
	{
		response_usec_ 	= tupd_usec_ - treq_usec_;
		reaction_usec_	= tres_usec_ - treq_usec_;
	}	

	static bool validate(const comm::COMM_HEADER *phdr, const comm::EVENT_NOTIFY *pnotify) noexcept;
};	

static constexpr uint32_t			MAX_PARSE_TOT_EXT_LEN	{MAX_PARSE_EXT_LEN + sizeof(API_TRAN)};
static constexpr uint32_t			MAX_PARSE_TRAN_SZ	{MAX_PARSE_API_LEN + MAX_PARSE_TOT_EXT_LEN + 1 + 7};
static constexpr uint32_t			MAX_TRAN_STR_ELEM	{25000};

static constexpr uint16_t			MAX_USER_DB_LEN		{128};

struct TRAN_REQ_WR
{
	THR_POOL_ALLOC::UNIQUE_PTR	uniq_;
	API_TRAN			*ptran_;
	STR_WR_BUF			tdstrbuf_;
	STR_WR_BIN			ustrbuf_;

	// urec must be at least api_max_len + MAX_PARSE_TOT_EXT_LEN
	TRAN_REQ_WR(THR_POOL_ALLOC::UNIQUE_PTR urec, uint32_t sz, uint32_t api_max_len, uint64_t tlastpkt_usec, uint64_t tconnect_usec, 
						const IP_PORT & cli_ipport, const IP_PORT & ser_ipport, uint64_t glob_id, PROTO_TYPES proto)
		: uniq_(std::move(urec)), ptran_(
		({
			if (sz < API_TRAN::get_max_elem_size(api_max_len)) {
				GY_THROW_EXCEPTION("Invalid size %u for Tran Req Writer", sz);
			}	

			(API_TRAN *)urec.get();
		})	
		),
		tdstrbuf_((char *)(ptran_ + 1), api_max_len), ustrbuf_((uint8_t *)tdstrbuf_.buffer() + api_max_len, MAX_PARSE_EXT_LEN)	
	{
		new (ptran_) API_TRAN(tlastpkt_usec, tconnect_usec, cli_ipport, ser_ipport, glob_id, proto);

		ustrbuf_ << (uint8_t)0;
	}	

	~TRAN_REQ_WR() noexcept
	{
		ptran_ = nullptr;
	}	

	size_t get_data_len() const noexcept
	{
		return sizeof(*ptran_) + tdstrbuf_.size() + ustrbuf_.size();
	}	

	// Returns complete record size after packing
	size_t pack_on_complete() noexcept
	{
		ptran_->request_len_ = tdstrbuf_.size();

		if (ptran_->request_len_ > 0) {
			// Add '\0' byte
			
			uint8_t				*pstrdest = (uint8_t *)ptran_ + ptran_->request_len_;

			*pstrdest = 0;
			ptran_->request_len_++;
		}

		size_t				fsz = sizeof(*ptran_) + ptran_->request_len_;

		if (ustrbuf_.size() <= 1) {
			ptran_->lenext_ = 0;

			return fsz;
		}	
		else {
			ptran_->lenext_ = ustrbuf_.size();
		}	

		uint8_t				*pdest = (uint8_t *)ptran_ + fsz;
		uint8_t				*psrc = ustrbuf_.buffer();			

		memcpy_or_move(pdest, psrc, ptran_->lenext_);

		return fsz + ptran_->lenext_;
	}	

	uint8_t num_ext_fields() const noexcept
	{
		if (ustrbuf_.size() > 1) {
			return *ustrbuf_.data();
		}	

		return 0;
	}	

	void reset() noexcept
	{
		ptran_->reset();
		tdstrbuf_.reset();

		ustrbuf_.reset();
		ustrbuf_ << (uint8_t)0;
	}	
};	

// Returns number of bytes copied. Will return 0 on truncation. Also updates tran with the tdstrbuf and ustrbuf lens
static size_t copy_tran_data(uint8_t *pdest, uint32_t maxsz, API_TRAN & tran, const STR_WR_BUF & tdstrbuf, const STR_WR_BIN & ustrbuf) noexcept
{
	size_t				tlen = sizeof(API_TRAN) + tdstrbuf.size() + 1 + ustrbuf.size();

	if (!pdest || maxsz < tlen || tdstrbuf.size() == 0) {
		return 0;
	}	

	tran.request_len_		= tdstrbuf.size() + 1;
	tran.lenext_			= ustrbuf.size();

	std::memcpy(pdest, &tran, sizeof(API_TRAN));

	std::memcpy(pdest + sizeof(API_TRAN), tdstrbuf.buffer(), tdstrbuf.size());
	pdest[sizeof(API_TRAN) + tdstrbuf.size()] = 0;
	
	std::memcpy(pdest + sizeof(API_TRAN) + tran.request_len_, ustrbuf.buffer(), tran.lenext_);

	return tlen;
}	

struct PARSE_ALL_FIELDS
{
	std::string_view		appname_;
	std::string_view		username_;
	std::string_view		dbname_;
	std::string_view		errtxt_;
	uint16_t			errclass_		{0};
	uint32_t			sessid_			{0};
	uint64_t			dyn_prep_reqnum_	{0};
	time_t				dyn_prep_time_t_	{0};
	uint32_t			hostpid_		{0};
	uint32_t			nrows_			{0};
	std::string_view		cursorname_;
	std::string_view		contenttype_;

	PARSE_ALL_FIELDS() noexcept	= default;

	PARSE_ALL_FIELDS(uint8_t *pext, uint32_t lenext) noexcept
	{
		set_fields(pext, lenext);
	}	

	int set_fields(uint8_t *pext, uint32_t lenext) noexcept
	{
		if (lenext <= 1) return 0;

		STR_RD_BIN			ustr(pext, lenext);
		uint8_t				nfl;

		ustr >> nfl;

		for (int i = 0; i < (int)nfl; ++i) {
			PARSE_FIELD_LEN			fl;

			if (ustr.bytes_left() >= sizeof(PARSE_FIELD_LEN)) {
				ustr >> fl;
			}
			else {
				return i;
			}	
			
			switch (fl.field_) {
			
			case FIELD_APPNAME :
				if (ustr.bytes_left() >= fl.len_) {
					appname_ = std::string_view((const char *)ustr.get_curr_pos(), fl.len_ > 1 ? fl.len_ - 1 : 0);
					ustr += fl.len_;
				}
				else {
					return i;
				}	
				break;

			case FIELD_USERNAME :
				if (ustr.bytes_left() >= fl.len_) {
					username_ = std::string_view((const char *)ustr.get_curr_pos(), fl.len_ > 1 ? fl.len_ - 1 : 0);
					ustr += fl.len_;
				}
				else {
					return i;
				}	
				break;

			case FIELD_DBNAME :
				if (ustr.bytes_left() >= fl.len_) {
					dbname_ = std::string_view((const char *)ustr.get_curr_pos(), fl.len_ > 1 ? fl.len_ - 1 : 0);
					ustr += fl.len_;
				}
				else {
					return i;
				}	
				break;

			case FIELD_ERRTXT :
				if (ustr.bytes_left() >= fl.len_) {
					errtxt_ = std::string_view((const char *)ustr.get_curr_pos(), fl.len_ > 1 ? fl.len_ - 1 : 0);
					ustr += fl.len_;
				}
				else {
					return i;
				}	
				break;

			case FIELD_ERRCLASS :
				ustr >> errclass_;
				break;

			case FIELD_SESSID :
				ustr >> sessid_;
				break;

			case FIELD_PREP_REQNUM :
				ustr >> dyn_prep_reqnum_;
				break;

			case FIELD_PREP_REQTIME :
				ustr >> dyn_prep_time_t_;
				break;

			case FIELD_HOSTPID :
				ustr >> hostpid_;
				break;

			case FIELD_NROWS :
				ustr >> nrows_;
				break;

			case FIELD_CURSORNAME :
				if (ustr.bytes_left() >= fl.len_) {
					cursorname_ = std::string_view((const char *)ustr.get_curr_pos(), fl.len_ > 1 ? fl.len_ - 1 : 0);
					ustr += fl.len_;
				}
				else {
					return i;
				}	
				break;

			case FIELD_CONTENTTYPE :
				if (ustr.bytes_left() >= fl.len_) {
					contenttype_ = std::string_view((const char *)ustr.get_curr_pos(), fl.len_ > 1 ? fl.len_ - 1 : 0);
					ustr += fl.len_;
				}
				else {
					return i;
				}	
				break;

			default :
				return i;
			}	
		}

		return nfl;
	}	
};	

static std::string_view get_api_tran(const API_TRAN *ptran, PARSE_ALL_FIELDS & fields) noexcept
{
	const char			*preq = (const char *)(ptran + 1);

	if (ptran->request_len_ <= 1) {
		return {};
	}

	fields.set_fields((uint8_t *)ptran + sizeof(*ptran) + ptran->request_len_, ptran->lenext_);

	return {preq, (size_t)ptran->request_len_ - 1};
}	

} // namespace gyeeta
