
#include <alphyn/alphyn.hpp>
#include <alphyn/tools/ct_string.hpp>

static constexpr neam::string_t str1 = "[coucou]+";
using regex1 = alphyn::regex::parse<str1>;
static constexpr neam::string_t str2 = "a+(b*|c?)+a*$";
using regex2 = alphyn::regex::parse<str2>;

int main(int ac, char**av)
{
  return ac > 1 && regex1::match(av[1]) != ~0ul && regex2::match(av[1]) != ~0ul;
}
