//
// file : node.hpp
// in : file:///home/tim/projects/alphyn2-cr/alphyn/regex/node.hpp
//
// created by : Timothée Feuillet
// date: Thu Jun 15 2017 20:32:48 GMT-0400 (EDT)
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

#include "../tools/embed.hpp"
#include "../tools/pair.hpp"

#include "dfa.hpp"

namespace alphyn
{
  namespace regex
  {
    enum class node_id : size_t
    {
      none = 0,

      range = 10, // a range of character [ab-d0-9] or [^dg] (and actually any single character, including special ones like $ or .)

      expr = 30, // any sub expression (...)
      alternative = 31, // expr | expr

      repeat = 50, // repeat from x to y: * is 0:+infinity, + is 1:+infinity, ? is 0:1, {x, y} is x:y, {x} is x:x, can be greedy or not
    };

    /// \brief A regex node. This is in fact a NFA graph
    template<node_id NodeId, typename... Args>
    struct node
    {
    };

    template<>
    struct node<node_id::none>
    {
      template<typename... Nexts>
      static constexpr size_t match(const char*, size_t index)
      {
        return index;
      }

      using range_list = neam::ct::type_list<>;
    };

    template<typename... Ranges>
    struct node<node_id::range, Ranges...>
    {
      template<typename Next, typename... Nexts>
      static constexpr size_t match(const char* str, size_t index)
      {
          return (((str[index] >= Ranges::first::get() && str[index] <= Ranges::second::get())) || ...) ? index + (str[index] ? 1 : 0) : ~0ul;
      }

      using range_list = neam::ct::type_list<Ranges...>;
    };

    template<bool Greedy, size_t Min, size_t Max, typename Expr>
    struct node<node_id::repeat, neam::embed<Greedy>, neam::embed<Min>, neam::embed<Max>, Expr>
    {
      template<typename Next, typename... Nexts>
      static constexpr size_t match(const char* str, size_t index)
      {
        size_t ret = ~0ul;
        for (size_t j = Min; j <= Max && (j < 5 || str[index + j - 5] != '\0'); ++j)
        {
          size_t it_index = index;
          for (size_t i = 0; i < j && it_index != ~0ul; ++i)
            it_index = Expr::template match<node<node_id::none>>(str, it_index);

          if (it_index != ~0ul)
          {
            const size_t result = Next::template match<Nexts..., node<node_id::none>>(str, it_index);
            if (result != ~0ul && (result > ret || ret == ~0ul))
            {
              if (!Greedy)
                return result;
              ret = result;
            }
          }
          else
            break;
        }
        return ret;
      }

      using range_list = typename Expr::range_list;
    };

    // this node is special as it is the entry point and a subexpr
    template<typename Node, typename... Nodes>
    struct node<node_id::expr, Node, Nodes...>
    {
      template<typename... Nexts>
      static constexpr size_t match(const char* str, size_t index)
      {
        return Node::template match<Nodes..., Nexts..., node<node_id::none>>(str, index);
      }

      static constexpr size_t match(const char* str, size_t index = 0)
      {
        return Node::template match<Nodes..., node<node_id::none>>(str, index);
      }

      // generate a list containing all ranges. This list is not sorten and may contain duplicates.
      using range_list = neam::ct::list::flatten<neam::ct::list::append<neam::ct::type_list<>, typename Node::range_list, typename Nodes::range_list...>>;

      // the range list, but processed so that the range list is unique and the smallest possible but still respect node transitions
      // exemple: the regex "0\.[0-9]+e[0-9]"
      //  has a range_list as follow: <0, ., [0-9], e, [0-9]>
      //  but the clean range list looks like <0, ., [1-9], e>
      using clean_range_list = neam::ct::list::make_unique<range_list>;
    };
    template<>
    struct node<node_id::expr>
    {
      template<typename Next = node<node_id::none>, typename... Nexts>
      static constexpr size_t match(const char* str, size_t index)
      {
        return Next::template match<Nexts...>(str, index);
      }

      static constexpr size_t match(const char*, size_t index = 0)
      {
        return index;
      }

      using range_list = neam::ct::type_list<>;
      using clean_range_list = neam::ct::type_list<>;
    };

    template<typename... Exprs>
    struct node<node_id::alternative, Exprs...>
    {
      template<typename... Nexts>
      static constexpr size_t match(const char* str, size_t index = 0)
      {
        size_t longuest = ~0ul;
        size_t tmp = index;
        ((longuest = ((tmp = Exprs::template match<Nexts..., node<node_id::none>>(str, index)) != ~0ul && (longuest == ~0ul || tmp > longuest) ? tmp : longuest)), ...);
        return longuest;

      }

      using range_list = neam::ct::list::flatten<neam::ct::list::append<neam::ct::type_list<>, typename Exprs::range_list...>>;
    };
  } // namespace regex
} // namespace alphyn
