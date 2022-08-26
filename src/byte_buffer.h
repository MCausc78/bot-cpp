/*
 * All Rights Reserved
 *
 * Copyright (c) 2022 JNNGL
 *
 * Unauthorized copying and/or distributing of this software in source
 * and/or binary forms, via any medium is strictly prohibited
 *
 * Proprietary and confidential
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>

struct ItemStack;

class ByteBuffer {
 public:
  ByteBuffer();
  explicit ByteBuffer(size_t);
  explicit ByteBuffer(const std::vector<unsigned char>&);
  ByteBuffer(const unsigned char*, size_t);

 public:
  unsigned char readByte();
  void writeByte(unsigned char);

  short readShort();
  void writeShort(short);

  int readInt();
  void writeInt(int);

  long long readLong();
  void writeLong(long long);

  std::string readString();
  void writeString(std::string);

  int readVarInt();
  void writeVarInt(int);

  long long readVarLong();
  void writeVarLong(long long);

  float readFloat();
  void writeFloat(float);

  double readDouble();
  void writeDouble(double);

  std::vector<unsigned char> readBytes(size_t);
  void writeBytes(std::vector<unsigned char>);
  void writeBytes(const unsigned char*, size_t);
  void writeBytes(ByteBuffer&, size_t);
  void writeBytes(ByteBuffer&);

  void ensureWritableBytes(size_t);
  [[nodiscard]] size_t readableBytes() const;
  [[nodiscard]] size_t length() const;

 public:
  size_t readerIdx = 0;
  size_t writerIdx = 0;

  std::vector<unsigned char> data;
};