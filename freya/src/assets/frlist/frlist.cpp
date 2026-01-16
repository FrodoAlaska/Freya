#include "frlist.h"

/// ----------------------------------------------------------------------
/// FrList functions 

void frlist_load(ListContext& out_list, const freya::FilePath& path) {
  // Lex

  freya::DynamicArray<ListToken> tokens;
  if(!list_lexer_init(path, tokens)) {
    return;
  }

  // Parse

  if(!list_parser_init(tokens, out_list)) {
    return;
  }
}

/// FrList context functions 
/// ----------------------------------------------------------------------
