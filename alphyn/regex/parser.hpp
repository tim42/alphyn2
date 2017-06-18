//
// file : parser.hpp
// in : file:///home/tim/projects/alphyn2-cr/alphyn/regex/parser.hpp
//
// created by : Timothée Feuillet
// date: Fri Jun 16 2017 18:04:02 GMT-0400 (EDT)
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

#include "../tools/ct_list.hpp"
#include "../tools/embed.hpp"

#include "node.hpp"
#include "range_utilities.hpp"

namespace alphyn
{
  namespace regex
  {
    template<char X, const char Str[], size_t Index, typename NodeList>
    struct parser_at
    {
      using node_list = neam::ct::list::append<NodeList, node<node_id::range, neam::ct::pair<neam::embed<X>, neam::embed<X>>>>;
      static constexpr size_t next_index = Index + 1;
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct parser_at<'.', Str, Index, NodeList>
    {
      using node_list = neam::ct::list::append<NodeList, node<node_id::range, neam::ct::pair<neam::embed<char(1)>, neam::embed<char(0x7F)>>>>;
      static constexpr size_t next_index = Index + 1;
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct parser_at<'$', Str, Index, NodeList>
    {
      using node_list = neam::ct::list::append<NodeList, node<node_id::range, neam::ct::pair<neam::embed<char(0)>, neam::embed<char(0)>>>>;
      static constexpr size_t next_index = Index + 1;
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct parser_at<'\\', Str, Index, NodeList>
    {
      static_assert(Str[Index + 1], "Invalid trailing backslash ( '\\' ) token");
      using node_list = neam::ct::list::append<NodeList, node<node_id::range, neam::ct::pair<neam::embed<char(Str[Index + 1])>, neam::embed<char(Str[Index + 1])>>>>;
      static constexpr size_t next_index = Index + 2;
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct parser_at<'?', Str, Index, NodeList>
    {
      static_assert(Index != 0 && neam::ct::list::size<NodeList> > 0, "Invalid starting '?' token");

      // replace the last node by a repeat
      using node_list = neam::ct::list::append
      <
        neam::ct::list::pop_back<NodeList>,
        node
        <
          node_id::repeat, neam::embed<true>, neam::embed<0ul>, neam::embed<1ul>,
          neam::ct::list::get_type<NodeList, neam::ct::list::size<NodeList> - 1>
        >
      >;
      static constexpr size_t next_index = Index + 1;
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct parser_at<'+', Str, Index, NodeList>
    {
      static_assert(Index != 0 && neam::ct::list::size<NodeList> > 0, "Invalid starting '+' token");

      // replace the last node by a repeat
      using node_list = neam::ct::list::append
      <
        neam::ct::list::pop_back<NodeList>,
        node
        <
          node_id::repeat, neam::embed<true>, neam::embed<size_t(1ul)>, neam::embed<size_t(~0ul)>,
          neam::ct::list::get_type<NodeList, neam::ct::list::size<NodeList> - 1>
        >
      >;
      static constexpr size_t next_index = Index + 1;
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct parser_at<'*', Str, Index, NodeList>
    {
      static_assert(Index != 0 && neam::ct::list::size<NodeList> > 0, "Invalid starting '*' token");

      // replace the last node by a repeat
      using node_list = neam::ct::list::append
      <
        neam::ct::list::pop_back<NodeList>,
        node
        <
          node_id::repeat, neam::embed<true>, neam::embed<size_t(0ul)>, neam::embed<size_t(~0ul)>,
          neam::ct::list::get_type<NodeList, neam::ct::list::size<NodeList> - 1>
        >
      >;
      static constexpr size_t next_index = Index + 1;
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct parser_at<')', Str, Index, NodeList>
    {
      using node_list = NodeList;
      static constexpr size_t next_index = Index;
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct parser_at<'\0', Str, Index, NodeList>
    {
      using node_list = NodeList;
      static constexpr size_t next_index = Index;
    };

    template<typename PairList, char X>
    struct parser_bracket_list_merge_last
    {
      using ret = neam::ct::list::append<neam::ct::list::pop_back<PairList>, neam::ct::pair<typename neam::ct::list::get_type<PairList, neam::ct::list::size<PairList> - 1>::first, neam::embed<X>>>;
    };
    template<char X>
    struct parser_bracket_list_merge_last<neam::ct::type_list<>, X>
    {
      using ret = neam::ct::type_list<neam::ct::pair<neam::embed<X>, neam::embed<X>>>;
    };

    template<char X, const char Str[], size_t Index, size_t Shift, bool Exclude, bool LastIncomplete, typename PairList>
    struct parser_bracket_list
    : public parser_bracket_list
    <
      Str[Index + Shift + 1], Str, Index, Shift + 1, Exclude, Str[Index + Shift + 1] == '-' && !LastIncomplete,
      typename std::conditional
      <
        LastIncomplete,
        typename parser_bracket_list_merge_last<PairList, X>::ret,
        neam::ct::list::append<PairList, neam::ct::pair<neam::embed<X>, neam::embed<X>>>
      >::type
    >
    {
    };

    template<const char Str[], size_t Index, size_t Shift, bool Exclude, typename PairList>
    struct parser_bracket_list<'-', Str, Index, Shift, Exclude, true, PairList>
    : public parser_bracket_list
    <
      Str[Index + Shift + 1], Str, Index, Shift + 1, Exclude, true,
      typename std::conditional
      <
        Str[Index + Shift + 1] != ']',
        PairList,
        neam::ct::list::append<PairList, neam::ct::pair<neam::embed<'-'>, neam::embed<'-'>>>
      >::type
    >
    {
    };

    template<const char Str[], size_t Index, size_t Shift, bool Exclude, bool LastIncomplete, typename PairList>
    struct parser_bracket_list<']', Str, Index, Shift, Exclude, LastIncomplete, PairList>
    {
      template<typename... P>
      using range_node_tpl = node<node_id::range, P...>;

      using unionized_ranges = typename neam::ct::list::extract<PairList>::template as<range_utilities::union_ranges>;
      using ranges = typename std::conditional
      <
        Exclude,
        typename neam::ct::list::extract<unionized_ranges>::template as<range_utilities::exclude_ranges>,
        unionized_ranges
      >::type;

      static constexpr size_t next_index = Index + Shift + 1;
      using range_node = typename neam::ct::list::extract<ranges>::template as<range_node_tpl>;
    };
    template<const char Str[], size_t Index, size_t Shift, bool Exclude, bool LastIncomplete, typename PairList>
    struct parser_bracket_list<'\0', Str, Index, Shift, Exclude, LastIncomplete, PairList>
    {
      static_assert(Index + Shift < 0, "Missing ']'");
    };

    template<const char Str[], size_t Index, size_t Shift, bool Exclude, bool LastIncomplete, typename PairList>
    struct parser_bracket_list<'\\', Str, Index, Shift, Exclude, LastIncomplete, PairList>
    : public parser_bracket_list
    <
      Str[Index + Shift + 2], Str, Index, Shift + 2, Exclude, Str[Index + Shift + 2] == '-' && !LastIncomplete,
      typename std::conditional
      <
        LastIncomplete,
        typename parser_bracket_list_merge_last<PairList, Str[Index + Shift + 1]>::ret,
        neam::ct::list::append<PairList, neam::ct::pair<neam::embed<Str[Index + Shift + 1]>, neam::embed<Str[Index + Shift + 1]>>>
      >::type
    >
    {
    };

    // handle the [^ that excludes things, following ^ occurrences are treated as a normal character
    template<const char Str[], size_t Index>
    struct parser_bracket_list<'^', Str, Index, 0, false, false, neam::ct::type_list<>>
    : public parser_bracket_list<Str[Index + 1], Str, Index, 1, true, false, neam::ct::type_list<>>
    {
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct parser_at<'[', Str, Index, NodeList>
    {
      using range = parser_bracket_list<Str[Index + 1], Str, Index + 1, 0, false, false, neam::ct::type_list<>>;
      using node_list = neam::ct::list::append<NodeList, typename range::range_node>;
      static constexpr size_t next_index = range::next_index;
    };

    template<const char Str[], size_t Index, typename NodeList, bool Done = false>
    struct expression_parser
    : public expression_parser
    <
      Str,
      parser_at<Str[Index], Str, Index, NodeList>::next_index,
      typename parser_at<Str[Index], Str, Index, NodeList>::node_list,
      parser_at<Str[Index], Str, Index, NodeList>::next_index == Index
    >
    {
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct expression_parser<Str, Index, NodeList, true>
    {
      template<typename... E>
      using node_expr = node<node_id::expr, E...>;

      using expression = typename neam::ct::list::extract<NodeList>::template as<node_expr>;
      static constexpr size_t next_index = Index;
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct parser_at<'(', Str, Index, NodeList>
    {
      using expr = expression_parser<Str, Index + 1, neam::ct::type_list<>>;
      using node_list = neam::ct::list::append<NodeList, typename expr::expression>;
      static constexpr size_t next_index = expr::next_index + 1;

      static_assert(Str[expr::next_index] == ')', "Missing closing ')'");
    };

    template<const char Str[], size_t Index, typename NodeList>
    struct parser_at<'|', Str, Index, NodeList>
    {
      template<typename... E>
      using node_expr = node<node_id::expr, E...>;

      using expression1 = typename neam::ct::list::extract<NodeList>::template as<node_expr>;
      using expression2 = expression_parser<Str, Index + 1, neam::ct::type_list<>>;

      using node_list = neam::ct::type_list<node<node_id::alternative, expression1, typename expression2::expression>>;

      static constexpr size_t next_index = expression2::next_index;
    };

    /// \brief Parse the regular expression
    template<const char Str[], size_t Index = 0>
    using parse = typename expression_parser<Str, Index, neam::ct::type_list<>>::expression;

// #ifndef NDEBUG
    static constexpr neam::string_t str1 = "a+(b*|c?)+a*$";
    static constexpr neam::string_t str2 = "a+(b*|c?)+a*";

    static_assert(parse<str1>::match("a") != ~0ul);
    static_assert(parse<str1>::match("ab") == 2);
    static_assert(parse<str1>::match("ab") == 2);
    static_assert(parse<str1>::match("aa") == 2);
    static_assert(parse<str1>::match("aaaa") == 4);
    static_assert(parse<str1>::match("aaca") == 4);
    static_assert(parse<str1>::match("abca") == 4);
    static_assert(parse<str1>::match("abaa") == 4);
    static_assert(parse<str1>::match("abbb") == 4);
    static_assert(parse<str1>::match("acbb") == 4);
    static_assert(parse<str1>::match("acba") == 4);
    static_assert(parse<str1>::match("acca") == 4);
    static_assert(parse<str1>::match("accc") == 4);
    static_assert(parse<str1>::match("abcb") == 4);

    static_assert(parse<str1>::match("bcb") == ~0ul);
    static_assert(parse<str1>::match("c") == ~0ul);

    static_assert(parse<str1>::match("abcaca") == ~0ul);
    static_assert(parse<str2>::match("abcaca") != ~0ul);


    static constexpr neam::string_t str3 = "[ acd-g\\\\k-m-x-]+$";
    static constexpr neam::string_t str4 = "[^acd-g\\\\k-m-x-]+$";

    static_assert(parse<str3>::match("aceace\\acle-ace") != ~0ul);
    static_assert(parse<str3>::match("b") == ~0ul);
    static_assert(parse<str3>::match("n") == ~0ul);
    static_assert(parse<str3>::match("n") == ~0ul);

    static_assert(parse<str4>::match("aceace\\acle-ace") == ~0ul);
    static_assert(parse<str4>::match("b") != ~0ul);
    static_assert(parse<str4>::match("n") != ~0ul);
    static_assert(parse<str4>::match("n") != ~0ul);

// #endif
  } // namespace regex
} // namespace alphyn
