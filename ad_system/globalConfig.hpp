#pragma once
#include "common/globalHelper.hpp"

class globalConfig
{
public:
    globalConfig(const string &filename) {
		load(filename);
	}
    ~globalConfig() {}

	// [SERVER]
	string service_port;
	int service_thread_num;
	int enable_print_request;
	
	// [COMMON]
	int Default;//TODO

private:
#define GET_VALUE(section, value) globalHelper::getValue(cfg, section, #value, value)
	void load(const string &cfg) {
		// [SERVER]
		ASSERT_TRUE(GET_VALUE("SERVER", service_port));
		ASSERT_TRUE(GET_VALUE("SERVER", service_thread_num));
		ASSERT_TRUE(GET_VALUE("SERVER", enable_print_request));
		// [COMMON]
		ASSERT_TRUE(GET_VALUE("COMMON", Default));
	}
#undef GET_VALUE

};

