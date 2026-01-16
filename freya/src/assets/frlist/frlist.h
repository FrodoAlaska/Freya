#pragma once

#include "freya_assets.h"

/// ----------------------------------------------------------------------
/// ListTokenType
enum ListTokenType {
  LIST_TOKEN_SECTION, 
  LIST_TOKEN_LOCAL,
  LIST_TOKEN_COMMENT,
  LIST_TOKEN_STRING_LITERAL,
  LIST_TOKEN_EOF,
};
/// ListTokenType
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ListSection
struct ListSection {
  freya::AssetType type;
  freya::FilePath local_dir;

  freya::DynamicArray<freya::FilePath> assets;
};
/// ListSection
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ListContext 
struct ListContext {
  freya::DynamicArray<ListSection> sections; 
  freya::FilePath parent_dir; 
};
/// ListContext 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ListToken 
struct ListToken {
  ListTokenType type; 
  
  freya::String literal;
  freya::i32 line;
};
/// ListToken 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// List lexer functions

bool list_lexer_init(const freya::FilePath& list_path, freya::DynamicArray<ListToken>& out_tokens);

/// List lexer functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// List parser functions

bool list_parser_init(const freya::DynamicArray<ListToken>& tokens, ListContext& out_list);

/// List parser functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// FrList functions 

void frlist_load(ListContext& out_list, const freya::FilePath& path);

/// FrList context functions 
/// ----------------------------------------------------------------------
