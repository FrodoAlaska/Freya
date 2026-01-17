#include "frlist.h"

/// ----------------------------------------------------------------------
/// FrList functions 

bool frlist_load(ListContext& out_list, const freya::FilePath& path) {
  // Assing the parent directory of the list
  out_list.parent_dir = freya::filepath_parent_path(path);
  
  // Lex

  freya::DynamicArray<ListToken> tokens;
  if(!list_lexer_init(path, tokens)) {
    return false;
  }

  // Parse

  if(!list_parser_init(tokens, out_list)) {
    return false;
  }

  // Done!
  return true;
}

/// FrList context functions 
/// ----------------------------------------------------------------------
