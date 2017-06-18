//
// file : range_utilities.hpp
// in : file:///home/tim/projects/alphyn2-cr/alphyn/regex/range_utilities.hpp
//
// created by : Timothée Feuillet
// date: Sun Jun 18 2017 19:21:20 GMT-0400 (EDT)
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

#include "../tools/pair.hpp"
#include "../tools/ct_list.hpp"
#include "../tools/embed.hpp"

namespace alphyn
{
  namespace regex
  {
    namespace range_utilities
    {
            template<typename Range1, typename... Ranges>
      struct inter_two_ranges {};
      template<char Min1, char Max1, char Min2, char Max2, typename... Ranges>
      struct inter_two_ranges<neam::ct::pair<neam::embed<Min1>, neam::embed<Max1>>, neam::ct::pair<neam::embed<Min2>, neam::embed<Max2>>, Ranges...>
      {
        static constexpr bool overlap = !(
          (Max1 < Min2) ||// The end of the first range is before the begining of the second one
          (Max2 < Min1)   // The end of the second range is before the begining of the first one
        );
        using type = typename std::conditional
        <
          overlap,
          typename inter_two_ranges
          <
            neam::ct::pair<neam::embed<char(Min1 > Min2 ? Min1 : Min2)>, neam::embed<char(Max1 < Max2 ? Max1 : Max2)>>,
            Ranges...
          >::type,
          neam::ct::pair<void, void>
        >::type;
      };

      template<typename RangeList, typename... Ranges>
      struct union_ranges_it
      {
        using type = RangeList;
      };

      template<typename Range1, typename Range2> struct are_overlapping {};
      template<char Min1, char Max1, char Min2, char Max2>
      struct are_overlapping<neam::ct::pair<neam::embed<Min1>, neam::embed<Max1>>, neam::ct::pair<neam::embed<Min2>, neam::embed<Max2>>>
      {
        static constexpr bool value = !(
          (Max1 < Min2) ||// The end of the first range is before the begining of the second one
          (Max2 < Min1)   // The end of the second range is before the begining of the first one
        );
      };

      template<typename Range, typename... Ranges>
      struct union_overlapping_ranges
      {
        using type = Range;
      };
      template<char Min1, char Max1, char Min2, char Max2, typename... Ranges>
      struct union_overlapping_ranges<neam::ct::pair<neam::embed<Min1>, neam::embed<Max1>>, neam::ct::pair<neam::embed<Min2>, neam::embed<Max2>>, Ranges...>
      : public union_overlapping_ranges<neam::ct::pair<neam::embed<char(Min1 < Min2 ? Min1 : Min2)>, neam::embed<char(Max1 > Max2 ? Max1 : Max2)>>, Ranges...>
      {
      };

      template<typename RangeList, typename Range, typename... Ranges>
      struct union_ranges_it<RangeList, Range, Ranges...>
      {
        template<typename X> using is_overlapping = are_overlapping<X, Range>;

        // retrieve overlapping ranges
        using overlapping_ranges = neam::ct::list::append<neam::ct::list::filter<RangeList, is_overlapping>, Range>;
        // remove overlapping ranges
        using non_overlapping_ranges = neam::ct::list::remove_if<RangeList, is_overlapping>;

        // compute the resulting range from the overlap
        using result_range = typename neam::ct::list::extract<overlapping_ranges>::template as<union_overlapping_ranges>::type;

        // the result
        using type = typename union_ranges_it<neam::ct::list::append<non_overlapping_ranges, result_range>, Ranges...>::type;
      };

      // yield a pair<void, void> if not everything overlaps
      template<typename... Ranges>
      using inter_ranges = inter_two_ranges<neam::ct::pair<neam::embed<0>, neam::embed<0x7F>>, Ranges...>;
      template<typename... Ranges>
      using union_ranges = typename union_ranges_it<neam::ct::type_list<>, Ranges...>::type;

      template<typename Range1, typename Range2>
      struct substract
      {
      };
      template<char Min1, char Max1>
      struct substract<neam::ct::pair<neam::embed<Min1>, neam::embed<Max1>>, neam::ct::pair<neam::embed<Min1>, neam::embed<Max1>>>
      {
        // same range: nothing to do
        using type = neam::ct::type_list<>;
      };

      enum class substract_op
      {
        split,
        move,
        remove
      };
      template<substract_op, typename Range1, typename Range2>
      struct substract_sub
      {
        using type = neam::ct::type_list<>; // remove
      };
      template<char Min1, char Max1, char Min2, char Max2>
      struct substract_sub<substract_op::move, neam::ct::pair<neam::embed<Min1>, neam::embed<Max1>>, neam::ct::pair<neam::embed<Min2>, neam::embed<Max2>>>
      {
        static constexpr bool is_left = (Min1 > Min2); // at this point, we already know that: we overlap, we are not the same, and the range isn't fully inside the other.

        using type = neam::ct::type_list<neam::ct::pair<neam::embed<is_left ? Max2 : Min1>, neam::embed<!is_left ? Min2 : Max1>>>;
      };
      template<char Min1, char Max1, char Min2, char Max2>
      struct substract_sub<substract_op::split, neam::ct::pair<neam::embed<Min1>, neam::embed<Max1>>, neam::ct::pair<neam::embed<Min2>, neam::embed<Max2>>>
      {
        using type = neam::ct::type_list<neam::ct::pair<neam::embed<Min1>, neam::embed<Min2>>, neam::ct::pair<neam::embed<Max2>, neam::embed<Max1>>>;
      };

      template<char Min1, char Max1, char Min2, char Max2>
      struct substract<neam::ct::pair<neam::embed<Min1>, neam::embed<Max1>>, neam::ct::pair<neam::embed<Min2>, neam::embed<Max2>>>
      {
        static constexpr substract_op op =
          (Min1 < Min2 && Max1 > Max2) ? substract_op::split   // 2 is located inside 1, so it's a split
          : ((Min2 <= Min1 && Max2 >= Max1) ? substract_op::remove // 1 is located inside 2, so there is nothing resuling that
             : substract_op::move); // for every other cases, (remember, we overlap !) it's a move

        using type = typename substract_sub<op, neam::ct::pair<neam::embed<Min1>, neam::embed<Max1>>, neam::ct::pair<neam::embed<Min2>, neam::embed<Max2>>>::type;
      };

      template<typename RangeList, typename... Ranges>
      struct exclude_ranges_it
      {
        using type = RangeList;
      };
      template<typename RangeList, typename Range, typename... Ranges>
      struct exclude_ranges_it<RangeList, Range, Ranges...>
      {
        template<typename X> using is_overlapping = are_overlapping<X, Range>;
        template<typename X> using substract_range = substract<X, Range>;

        // retrieve overlapping ranges
        using overlapping_ranges = neam::ct::list::filter<RangeList, is_overlapping>;
        // remove overlapping ranges
        using non_overlapping_ranges = neam::ct::list::remove_if<RangeList, is_overlapping>;

        // substract the current range from overlapping ones
        using substracted = neam::ct::list::flatten<neam::ct::list::for_each_indirect<overlapping_ranges, substract_range>>;

        using type = typename exclude_ranges_it<typename neam::ct::list::merge<non_overlapping_ranges, substracted>::template as<neam::ct::type_list>, Ranges...>::type;
      };

      // make a list of exclude ranges as inclusive ranges
      template<typename... Ranges>
      using exclude_ranges = typename exclude_ranges_it<neam::ct::type_list<neam::ct::pair<neam::embed<char(1)>, neam::embed<char(0x7F)>>>, Ranges...>::type;
    } // namespace range_utilities
  } // namespace regex
} // namespace alphyn
