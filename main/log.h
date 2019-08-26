#pragma once
#include <stdio.h>

#define LOGD(format, ...) printf("[DEBUG][%s][%s][%d]: " format "\n", __FILE__, __FUNCTION__,\
                            __LINE__, ##__VA_ARGS__)
