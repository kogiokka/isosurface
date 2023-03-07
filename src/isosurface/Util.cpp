#include "Util.hpp"

std::vector<std::string>
util::str::Split(std::string const& target, std::string const& pattern)
{
  using namespace std;
  regex re(pattern);
  vector<std::string> tokens;

  tokens.insert(end(tokens), sregex_token_iterator(begin(target), end(target), re, -1), sregex_token_iterator());

  return tokens;
}

bool
util::str::StartsWith(std::string const& target, std::string const& prefix)
{
  return (target.substr(0, prefix.size()) == prefix);
}

std::string
util::str::TrimCR(std::string string)
{
  if (string.back() == '\r') {
    string.pop_back();
  }
  return string;
}
