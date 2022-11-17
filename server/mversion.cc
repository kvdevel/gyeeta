//  SPDX-FileCopyrightText: 2022 Exact Solutions, Inc.
//  SPDX-License-Identifier: GPL-3.0-or-later

#include		"gy_common_inc.h"

namespace gyeeta {
namespace madhava {

const char *		gversion = "0.1.1";

const char		gcopyright[] = "Copyright 2022 - present by Exact Solutions, Inc.";

uint32_t		gversion_num 		= get_version_from_string(gversion, 3);

uint32_t		gmin_partha_version 	= get_version_from_string("0.1.0", 3); 
uint32_t		gmin_shyama_version 	= get_version_from_string("0.1.0", 3); 
uint32_t		gmin_node_version 	= get_version_from_string("0.1.0", 3); 

/*
 * 
 *	Version		Date		Description
 *
 *	0.1.0		Nov 10, 2022	First Release
 *	0.1.1		Nov 17, 2022	Fix for Cloud Metadata
 */

} // namespace madhava 
} // namespace gyeeta

