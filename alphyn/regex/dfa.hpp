//
// file : dfa.hpp
// in : file:///home/tim/projects/alphyn2-cr/alphyn/regex/dfa.hpp
//
// created by : Timothée Feuillet
// date: Fri Jun 16 2017 23:16:20 GMT-0400 (EDT)
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

#include <array>

#include "../tools/ct_list.hpp"

namespace alphyn
{
  namespace regex
  {
    using dfa_result = std::array<size_t, 2>;
    static constexpr dfa_result dfa_no_result = dfa_result{{~0ul, ~0ul}};

    /// \brief An edge. A special case is fa_edge<>: it accept any character
    template<size_t DFANodeIndex, char... Transitions>
    struct fa_edge
    {
      static constexpr bool is_matching(char a)
      {
        return (sizeof...(Transitions) == 0) || ((a == Transitions) || ...);
      }

      // assumes that is_matching has been called prior to this one and that it returned true
      template<typename DFANodeList>
      static constexpr dfa_result match(const char *str, size_t index)
      {
        return neam::ct::list::get_type<DFANodeList, DFANodeIndex>::match(str, index + 1);
      }
    };

    /// \brief A [ND]FA Node
    /// \tparam MatchingID The ID returnedif something matched. 0 if not matching
    /// If Matching is not 0 and there isn't any transition possible, it will match with {string-index, matching-id}
    template<size_t MatchingID, typename... Edges>
    struct fa_node
    {
      template<typename DFANodeList>
      static constexpr dfa_result match(const char *str, size_t index)
      {
        dfa_result ret = dfa_no_result;
        // iterate over the edges and get call match over the first matching one
        ((Edges::is_matching(str, index) ? ((ret = Edges::template match<DFANodeList>(str, index), true)) : false) || ...) || ((ret = (MatchingID ? dfa_result{{index, MatchingID}} : dfa_no_result)), false);
        return ret;
      }

      /// \brief Change the matchin ID. If NewMatchingID is 0, keep the existing ID.
      template<size_t NewMatchingID>
      using set_matching_id = fa_node<NewMatchingID ? (MatchingID ? NewMatchingID : 0) : MatchingID, Edges...>;
    };

    /// \brief Mostly NFA2DFA conversion and DFA merging utilities
    namespace utilities
    {
      
    } // namespace utilities
  } // namespace regex
} // namespace alphyn
