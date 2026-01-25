#include "frlist.h"
#include "freya_logger.h"
#include "freya_timer.h"

/// ----------------------------------------------------------------------
/// Parser
struct Parser {
  freya::DynamicArray<ListToken> tokens;
  freya::i32 current;

  freya::HashMap<freya::String, freya::AssetType> asset_remaps;
};

static Parser s_parser;
/// Parser
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static bool iterate_assets(const freya::FilePath& base_dir, const freya::FilePath& current_path, void* user_data) {
  // Not a single file. We don't care
  
  if(freya::filepath_is_dir(current_path)) {
    return true;
  }

  // Add the path

  ListSection* section = (ListSection*)user_data;
  section->assets.emplace_back(current_path);

  // Done!
  return true;
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static bool is_eof() {
  return s_parser.tokens[s_parser.current].type == LIST_TOKEN_EOF;
}

static ListToken& token_consume() {
  if(is_eof()) {
    return s_parser.tokens[s_parser.current];
  }

  s_parser.current++;
  return s_parser.tokens[s_parser.current - 1];
}

static ListToken& token_peek_prev() {
  if(s_parser.current == 0) {
    return s_parser.tokens[0];
  }

  return s_parser.tokens[s_parser.current - 2];
}

static ListToken& token_peek_next() {
  if(is_eof()) {
    return s_parser.tokens[s_parser.current];
  }

  return s_parser.tokens[s_parser.current];
}

static void assign_section(ListContext* list, ListSection* section) {
  // Safety check

  if(token_peek_next().type != LIST_TOKEN_STRING_LITERAL) {
    FREYA_LOG_ERROR("Section declared without a name");
    return;
  }

  // Initialize a new section
  section->local_dir = list->parent_dir;

  // Assign the type of the new section
  
  ListToken& next = token_consume();
  section->type = s_parser.asset_remaps[next.literal];
}

static void assign_local(ListSection* section) {
  // Check for the literal
  
  if(token_peek_next().type != LIST_TOKEN_STRING_LITERAL) {
    FREYA_LOG_ERROR("Local variable declared without an identifier");
    return;
  } 
  
  // Consume and use the local variable
  section->local_dir = freya::filepath_append(section->local_dir, token_consume().literal); 
}

static void assign_path(ListSection* section, const ListToken& current_token) {
  freya::FilePath full_path = freya::filepath_append(section->local_dir, current_token.literal);

  // We need all of the inner asset paths, so iterate through them

  if(freya::filepath_is_dir(full_path)) {
    freya::filesystem_directory_recurse_iterate(full_path, iterate_assets, section);
    return;
  }

  // Otherwise, just add the path regularly
  section->assets.emplace_back(full_path); 
}

static bool parser_start(ListContext* list) {
  ListSection* section = nullptr;

  while(!is_eof()) {
    ListToken token = token_consume(); 

    switch(token.type) {
      case LIST_TOKEN_SECTION: {
        // Add a new section to the context
        
        list->sections.push_back(ListSection{});
        section = &list->sections[list->sections.size() - 1];
         
        assign_section(list, section);
      } break;
      case LIST_TOKEN_LOCAL:
        assign_local(section);
        break;
      case LIST_TOKEN_STRING_LITERAL:
        assign_path(section, token);
        break;
      case LIST_TOKEN_COMMENT:
      case LIST_TOKEN_EOF:
        break;
      default:
        FREYA_LOG_ERROR("Invalid token found");
        return false;
    }
  }

  return true;
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// List parser functions

bool list_parser_init(const freya::DynamicArray<ListToken>& tokens, ListContext& out_list) {
  FREYA_PROFILE_FUNCTION();

  // Nothing was given!

  if(tokens.empty()) {
    FREYA_LOG_ERROR("Empty tokens array given to parser!");
    return false;
  }
  
  // Defining the name to asset type mappings

  s_parser.asset_remaps["TEXTURE"] = freya::ASSET_TYPE_TEXTURE;
  s_parser.asset_remaps["texture"] = freya::ASSET_TYPE_TEXTURE;

  s_parser.asset_remaps["SHADER"] = freya::ASSET_TYPE_SHADER;
  s_parser.asset_remaps["shader"] = freya::ASSET_TYPE_SHADER;

  s_parser.asset_remaps["FONT"] = freya::ASSET_TYPE_FONT;
  s_parser.asset_remaps["font"] = freya::ASSET_TYPE_FONT;

  s_parser.asset_remaps["AUDIO"] = freya::ASSET_TYPE_AUDIO_BUFFER;
  s_parser.asset_remaps["audio"] = freya::ASSET_TYPE_AUDIO_BUFFER;

  // Parser init

  s_parser.current = 0; 
  s_parser.tokens  = tokens;

  // Done!
  return parser_start(&out_list);
}

/// List parser functions
/// ----------------------------------------------------------------------
