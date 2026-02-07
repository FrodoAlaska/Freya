#pragma once

#include "freya_file.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

// Forward declarations

struct Font;
struct ShaderContext;
struct Animation;

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
  ASSET_TYPE_SHADER_CONTEXT,
  ASSET_TYPE_FONT,
  ASSET_TYPE_AUDIO_BUFFER,
  ASSET_TYPE_UI_STYLE,

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
      :_type(type), _group(group_id), _id(id)
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
/// Asset manager functions

/// Initialize the global asset manager.
FREYA_API void asset_manager_init();

/// Shutdown the global asset manager, reclaming any memory in the process 
/// and destroying the cache asset group.
FREYA_API void asset_manager_shutdown();

/// Asset manager functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AssetGroupID functions

/// Create a new (and empty) asset group with a `name`.
FREYA_API AssetGroupID asset_group_create(const String& name);

/// Destroy and unload all of the assets in the given `group_id`.
FREYA_API void asset_group_destroy(const AssetGroupID& group_id);

/// Build all of the intermediary formats listed in the `.frlist` file at `list_path` 
/// into a `FRPKG` file and place it at `output_path`. 
///
/// @NOTE: See `asset_group_create` for more information about internal paths.
FREYA_API bool asset_group_build(const AssetGroupID& group_id, const FilePath& list_path, const FilePath& output_path);

/// Push a new `GfxBuffer` into `group_id`, using all the information found in `buff_desc`,
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_buffer(const AssetGroupID& group_id, const GfxBufferDesc& buff_desc);

/// Push a new `GfxTexture` into `group_id`, using all the information found in `tex_desc`,
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_texture(const AssetGroupID& group_id, const GfxTextureDesc& tex_desc);

/// Push a new `GfxShader` into `group_id`, using all the information found in `shader_desc`,
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_shader(const AssetGroupID& group_id, const GfxShaderDesc& shader_desc);

/// Push a new `ShaderContext` into `group_id`, using the previously-created shader `shader_id`,
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_shader_context(const AssetGroupID& group_id, const AssetID& shader_id);

/// Push a new `ShaderContext` into `group_id`, creating a new `GfxShader` with the information 
/// found in `shader_desc`, returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_shader_context(const AssetGroupID& group_id, const GfxShaderDesc& shader_desc);

/// Push a new `Font` into `group_id`, using `font_data` and `name` (name of the font),
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_font(const AssetGroupID& group_id, const DynamicArray<u8>& font_data, const String& name);

/// Push a new `AudioBufferID` into `group_id`, using all the information found in `audio_desc`,
/// returning a valid `AssetID` to be used later.
FREYA_API AssetID asset_group_push_audio_buffer(const AssetGroupID& group_id, const AudioBufferDesc& audio_desc);

/// Load a `FRPKG` file at `frpkg_path` and push all of the assts into the given `group_id`. 
///
/// @NOTE: See `asset_group_create` for more information about internal paths.
FREYA_API bool asset_group_load_package(const AssetGroupID& group_id, const FilePath& frpkg_path);

/// Get a valid `AssetID` from `group_id`, using the given `asset_name` to identify the asset. 
///
/// @NOTE: This function will return an invalid `AssetID` if the given `asset_name` does not 
/// exist in `group_id`. The name of the asset is derived from its file stem (i.e `texture.png` -> `texture`).
FREYA_API const AssetID& asset_group_get_id(const AssetGroupID& group_id, const String& asset_name);

/// Retrieve a `GfxBuffer`, using `id`.
///
/// @NOTE: This function will assert if the given `id` is either: 
///   1 - is invalid and was never created before, 
///   2 - the internal group ID is invalid,
///   3 - or the internal type does not match this asset.
FREYA_API GfxBuffer* asset_group_get_buffer(const AssetID& id);

/// Retrieve a `GfxTexture`, using `id`.
///
/// @NOTE: This function will assert if the given `id` is either: 
///   1 - is invalid and was never created before, 
///   2 - the internal group ID is invalid,
///   3 - or the internal type does not match this asset.
FREYA_API GfxTexture* asset_group_get_texture(const AssetID& id);

/// Retrieve a `GfxShader`, using `id`.
///
/// @NOTE: This function will assert if the given `id` is either: 
///   1 - is invalid and was never created before, 
///   2 - the internal group ID is invalid,
///   3 - or the internal type does not match this asset.
FREYA_API GfxShader* asset_group_get_shader(const AssetID& id);

/// Retrieve a `ShaderContext`, using `id`.
///
/// @NOTE: This function will assert if the given `id` is either: 
///   1 - is invalid and was never created before, 
///   2 - the internal group ID is invalid,
///   3 - or the internal type does not match this asset.
FREYA_API ShaderContext* asset_group_get_shader_context(const AssetID& id);

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

/// AssetGroupID functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////
