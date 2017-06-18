//
// file : regex.hpp
// in : file:///home/tim/projects/alphyn2-cr/alphyn/regex/regex.hpp
//
// created by : Timothée Feuillet
// date: Thu Jun 15 2017 20:14:16 GMT-0400 (EDT)
//
//
// Copyright (c) 2017 Timothée Feuillet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include <cstddef>

#include "../tools/ct_string.hpp"

#include "node.hpp"
#include "parser.hpp"

namespace alphyn
{
  /// \brief The ultimate way to compile a regex is to let the compiler compile it
  namespace regex
  {
    template<const char Str[]>
    class regex
    {
      private:
        static constexpr size_t len = neam::ct::strlen(Str);

        
    };
  } // namespace regex
} // namespace alphyn
