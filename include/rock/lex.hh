#ifndef rock_lex_hh
#define rock_lex_hh

#include <vector>

namespace rock {

/**
 * On success, the lex function will return a vector
 * of tokens ending in EOF.
 * On failure, the lex function will reutrn a vector
 * of tokens ending in ERROR.
 * The 'value' field of the ERROR token will
 * contain more information on what kind of error
 * was encountered.
 */
std::vector<Token> lex(const File&);

}

#endif//rock_lex_hh
