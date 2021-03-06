/*
 * rest.c
 *
 * Copyright 2016 DeviceHive
 *
 * Author: Nikolay Khabarov
 *
 */

#include "uploadable_api.h"
#include "dhsettings.h"
#include "uploadable_page.h"

#include <c_types.h>
#include <osapi.h>
#include <ets_forward.h>

HTTP_RESPONSE_STATUS ICACHE_FLASH_ATTR uploadable_api_handle(const char *path, const char *key,
		HTTP_CONTENT *content_in, HTTP_ANSWER *answer) {
	static const char flash[] = "/flash/page/";
	answer->content.len = 0;
	if(os_strncmp(path, flash, sizeof(flash) - 1) == 0) {
		if(dhsettings_get_devicehive_key()[0]) {
			if(key == 0) {
				return HRCS_UNAUTHORIZED;
			}
			if(os_strcmp(key, dhsettings_get_devicehive_key())) {
				return HRCS_UNAUTHORIZED;
			}
		}
		const char *p = &path[sizeof(flash) - 1];
		UP_STATUS res = UP_STATUS_WRONG_CALL;
		if(os_strcmp(p, "begin") == 0) {
			if(content_in->len == 0)
				res = uploadable_page_begin();
		} else if(os_strcmp(p, "finish") == 0) {
			if(content_in->len == 0)
				res = uploadable_page_finish();
		} else if(os_strcmp(p, "put") == 0) {
			if(content_in->len)
				res = uploadable_page_put(content_in->data, content_in->len);
		} else {
			return HRCS_NOT_FOUND;
		}
		switch(res) {
			case UP_STATUS_OK:
				return HRCS_ANSWERED_PLAIN;
			case UP_STATUS_INTERNAL_ERROR:
				return HRCS_INTERNAL_ERROR;
			case UP_STATUS_WRONG_CALL:
				answer->ok = 0;
				return HRCS_ANSWERED_PLAIN;
			case UP_STATUS_OVERFLOW:
				return HRCS_TOO_MANY_REQUESTS;
		}
		return HRCS_ANSWERED_PLAIN;
	}
	return HRCS_NOT_FOUND;
}
