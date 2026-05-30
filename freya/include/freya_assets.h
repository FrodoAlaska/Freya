#pragma once

#include "freya_file.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Assets consts

/// The currently valid version of any `.frpkg` file
const u8 FRPKG_VALID_VERSION  = 5;

/// A value to indicate an invalid asset group.
const i32 ASSET_GROUP_INVALID = -1;

/// A value to indicate an invalid asset ID.
const i16 ASSET_ID_INVALID    = -1;

/// The ID of the group associated with the rasset cache.
const i32 ASSET_CACHE_ID      = 0;

/// Assets consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AssetType
enum AssetType {
  ASSET_TYPE_INVALID = -1,
  
  ASSET_TYPE_BUFFER = 0, 
  ASSET_TYPE_TEXTURE, 
  ASSET_TYPE_SHADER,
  ASSET_TYPE_FONT,
  ASSET_TYPE_AUDIO_BUFFER,
  ASSET_TYPE_UI_CONFIG,
  ASSET_TYPE_LUA,

  ASSET_TYPES_MAX,
};
/// AssetType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AssetGroupID 
struct AssetGroupID {
  /// Constructors

  public:
    AssetGroupID() {}
    AssetGroupID(const i32 id) :_id(id) {}
  
  /// Getters (yuck!)

  public:
    inline const i32 get_id() const {return _id;}
  
  /// Operators

  public:
    //
    // Eqality operators
    // 

    inline bool operator==(const i32& rhs) {return _id == rhs;}
    inline bool operator!=(const i32& rhs) {return _id != rhs;}
    
    inline bool operator==(const i32& rhs) const {return _id == rhs;}
    inline bool operator!=(const i32& rhs) const {return _id != rhs;}

  /// Private fields

  private:
    i32 _id = ASSET_GROUP_INVALID;
};
/// AssetGroupID 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AssetID
struct AssetID {
  /// Constructors

  public:
    AssetID() {}

    AssetID(const AssetType type, const AssetGroupID& group_id, const i16 id) 
      :_type(type), _id(id), _group(group_id)
    {}
  
  /// Getters (yuck!)

  public:
    inline const AssetType get_type() const {return _type;}
    inline const i16 get_id() const {return _id;}
    inline const AssetGroupID get_group() const {return _group;}
    inline const i32 get_group_id() const {return _group.get_id();}

  /// Operators

  public:
    //
    // Eqality operators
    // 

    inline bool operator==(const i16& rhs) {return _id == rhs;}
    inline bool operator!=(const i16& rhs) {return _id != rhs;}
    
    inline bool operator==(const AssetType& rhs) {return _type == rhs;}
    inline bool operator!=(const AssetType& rhs) {return _type != rhs;}
   
    //
    // Comparison operators 
    //

    inline bool operator>(const i16& rhs) {return _id > rhs;}
    inline bool operator<(const i16& rhs) {return _id < rhs;}
    
    inline bool operator>=(const i16& rhs) {return _id >= rhs;}
    inline bool operator<=(const i16& rhs) {return _id <= rhs;}
    
    inline bool operator==(const i16& rhs) const {return _id == rhs;}
    inline bool operator!=(const i16& rhs) const {return _id != rhs;}
    
    inline bool operator==(const AssetType& rhs) const {return _type == rhs;}
    inline bool operator!=(const AssetType& rhs) const {return _type != rhs;}
    
    inline bool operator>(const i16& rhs) const {return _id > rhs;}
    inline bool operator<(const i16& rhs) const {return _id < rhs;}
    
    inline bool operator>=(const i16& rhs) const {return _id >= rhs;}
    inline bool operator<=(const i16& rhs) const {return _id <= rhs;}

  /// Private fields

  private:
    AssetType _type     = ASSET_TYPE_INVALID; 
    i16 _id             = ASSET_ID_INVALID;
    AssetGroupID _group = ASSET_GROUP_INVALID;
};
/// AssetID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AssetGroup 
struct AssetGroup {
  String name;
  FilePath frpkg_path;

  AssetGroupID id;

  DynamicArray<sg_buffer> buffers;
  DynamicArray<Texture> textures;
  DynamicArray<sg_shader> shaders;
  DynamicArray<AudioBufferID> audio_buffers;
  
  DynamicArray<Font*> fonts;
  DynamicArray<UIConfig> ui_configs;
  DynamicArray<lua_State*> lua_states;
  
  HashMap<String, AssetID> named_ids;

  ///
  /// @NOTE/@TEMP:
  ///
  /// We have 6 watchers here for all the assets that are actually 
  /// read from the disk, as opposed to the assets that are created 
  /// on the CPU. Currently, the 6 are: textures, fonts, shaders, audio buffers, ui configs, and lua files.
  ///
  Array<FileWatcher*, 6> watchers; // on the wall... no? ASOIAF?
};
/// AssetGroup 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Asset manager functions

/// Initialize the global asset manager.
FREYA_API void asset_manager_init();

/// Shutdown the global asset manager, reclaming any memory in the process 
/// and destroying the cache asset group.
FREYA_API void asset_manager_shutdown();

/// Retrieve a reference to an `AssetGroup` object using the given `group_id`.
FREYA_API AssetGroup& asset_manager_get_group(const AssetGroupID& group_id);

/// Asset manager functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AssetGroupID functions

/// Create a new (and empty) asset group with a `name`.
FREYA_API AssetGroupID asset_group_create(const String& name);

/// Unload all of the assets in the given `group_id`, but do not 
/// destroy the `group_id`.
FREYA_API void asset_group_clear(const AssetGroupID& group_id);

/// Destroy and unload all of the assets in the given `group_id`.
FREYA_API void asset_group_destroy(const AssetGroupID& group_id);

/// Clear and reload all of the assets of the given `group_id` from 
/// the `FRPKG` file at the path initally given in `asset_group_load_package`.
///
/// @NOTE: It is advised to only use this function in dev-only builds. 
/// It is very slow, since it needs to unload all the assets and load them again. 
FREYA_API void asset_group_reload(const AssetGroupID& group_id);

/// Build all of the intermediary formats listed in the `.frlist` file at `list_path` 
/// into a `FRPKG` file and place it at `output_path`. 
///
/// @NOTE: It is advised to only use this function in dev-only builds. 
/// It is very slow, since it needs to convert all the intermediary formats into binary formats 
/// and then write them on disk. 
FREYA_API bool asset_group_build(const AssetGroupID& group_id, const FilePath& list_path, const FilePath& output_path);

/// Push a new `sg_buffer` into `group_id`, using all the information found in `buff_desc`,
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_buffer(const AssetGroupID& group_id, const sg_buffer_desc& buff_desc);

/// Push a new `Texture` into `group_id`, using all the information found in `image_desc`, and `sampler_desc`,
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_texture(const AssetGroupID& group_id, 
                                           const sg_image_desc& image_desc, 
                                           const sg_sampler_desc& sampler_desc);

/// Push a new `sg_shader` into `group_id`, using all the information found in `shader_desc`,
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_shader(const AssetGroupID& group_id, const sg_shader_desc& shader_desc);

/// Push a new `Font` into `group_id`, using `font_data` and `name` (name of the font),
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_font(const AssetGroupID& group_id, const DynamicArray<u8>& font_data, const String& name);

/// Push a new `AudioBufferID` into `group_id`, using all the information found in `audio_desc`,
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_audio_buffer(const AssetGroupID& group_id, const AudioBufferDesc& audio_desc);

/// Push a new `UIConfig` into `group_id`, using the given `html_source`,
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_ui_config(const AssetGroupID& group_id, const String& html_source);

/// Push a new `sol::state` into `group_id`, using the given `lua_source`,
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_lua_state(const AssetGroupID& group_id, const String& lua_source);

/// Load a `FRPKG` file at `frpkg_path` and push all of the assts into the given `group_id`. 
///
/// @NOTE: See `asset_group_create` for more information about internal paths.
FREYA_API bool asset_group_load_package(const AssetGroupID& group_id, const FilePath& frpkg_path);

/// Get a valid `AssetID` from `group_id`, using the given `asset_name` to identify the asset. 
///
/// @NOTE: This function will return an invalid `AssetID` if the given `asset_name` does not 
/// exist in `group_id`. The name of the asset is derived from its file stem (i.e `texture.png` -> `texture`).
FREYA_API const AssetID& asset_group_get_id(const AssetGroupID& group_id, const String& asset_name);

/// Retrieve a `sg_buffer`, using `id`.
///
/// @NOTE: This function will assert if the given `id` is either: 
///   1 - is invalid and was never created before, 
///   2 - the internal group ID is invalid,
///   3 - or the internal type does not match this asset.
FREYA_API sg_buffer asset_group_get_buffer(const AssetID& id);

/// Retrieve a `Texture`, using `id`.
///
/// @NOTE: This function will assert if the given `id` is either: 
///   1 - is invalid and was never created before, 
///   2 - the internal group ID is invalid,
///   3 - or the internal type does not match this asset.
FREYA_API Texture& asset_group_get_texture(const AssetID& id);

/// Retrieve a `sg_shader`, using `id`.
///
/// @NOTE: This function will assert if the given `id` is either: 
///   1 - is invalid and was never created before, 
///   2 - the internal group ID is invalid,
///   3 - or the internal type does not match this asset.
FREYA_API sg_shader asset_group_get_shader(const AssetID& id);

/// Retrieve a `Font`, using `id`.
///
/// @NOTE: This function will assert if the given `id` is either: 
///   1 - is invalid and was never created before, 
///   2 - the internal group ID is invalid,
///   3 - or the internal type does not match this asset.
FREYA_API Font* asset_group_get_font(const AssetID& id);

/// Retrieve a `AudioBufferID`, using `id`.
///
/// @NOTE: This function will assert if the given `id` is either: 
///   1 - is invalid and was never created before, 
///   2 - the internal group ID is invalid,
///   3 - or the internal type does not match this asset.
FREYA_API const AudioBufferID& asset_group_get_audio_buffer(const AssetID& id);

/// Retrieve a `UIConfig`, using `id`.
///
/// @NOTE: This function will assert if the given `id` is either: 
///   1 - is invalid and was never created before, 
///   2 - the internal group ID is invalid,
///   3 - or the internal type does not match this asset.
FREYA_API UIConfig& asset_group_get_ui_config(const AssetID& id);

/// Retrieve a `lua_State*`, using `id`.
///
/// @NOTE: This function will assert if the given `id` is either: 
///   1 - is invalid and was never created before, 
///   2 - the internal group ID is invalid,
///   3 - or the internal type does not match this asset.
FREYA_API lua_State* asset_group_get_lua_state(const AssetID& id);

/// AssetGroupID functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////
