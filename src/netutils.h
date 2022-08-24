#pragma once

#include <uv.h>

#include "byte_buffer.h"

void write(uv_stream_t*, const ByteBuffer&);
void splitIP(const std::string&, std::string&, int&);