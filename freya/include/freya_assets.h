#pragma once

#include "freya_gfx.h"
#include "freya_file.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

// Forward declarations

struct ShaderContext;
struct Animation;

///---------------------------------------------------------------------------------------------------------------------
/// Assets consts

/// The currently valid version of any `.frpkg` file
const u8 FRPKG_VERSION        = 1;

/// A value to indicate an invalid asset group.
const i32 ASSET_GROUP_INVALID = -1;

/// A value to indicate an invalid asset ID.
const i16 ASSET_ID_INVALID    = -1;

/// The ID of the group associated with the rasset cache.
const i16 ASSET_CACHE_ID      = 0;

/// Assets consts
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
/// AssetType
enum AssetType {
  ASSET_TYPE_INVALID = -1,
  
  ASSET_TYPE_BUFFER = 0, 
  ASSET_TYPE_TEXTURE, 
  ASSET_TYPE_SHADER,
  ASSET_TYPE_SHADER_CONTEXT,
  ASSET_TYPE_ANIMATION,
  ASSET_TYPE_FONT,
  ASSET_TYPE_AUDIO_BUFFER,

  ASSET_TYPES_MAX,
};
/// AssetType
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
    inline const i16 get_id() const {return _type;}
    inline const AssetGroupID& get_group() const {return _type;}

  /// Operators

  public:
    inline bool operator==(const i16& rhs) {return _id == rhs;}
    inline bool operator!=(const i16& rhs) {return _id != rhs;}
    
    inline bool operator==(const AssetType& rhs) {return _type == rhs;}
    inline bool operator!=(const AssetType& rhs) {return _type != rhs;}
    
    inline bool operator>(const i16& rhs) {return _id > rhs;}
    inline bool operator<(const i16& rhs) {return _id < rhs;}
    
    inline bool operator>=(const i16& rhs) {return _id >= rhs;}
    inline bool operator<=(const i16& rhs) {return _id <= rhs;}

  /// Private fields

  private:
    AssetType _type     = ASSET_TYPE_INVALID; 
    i16 _id             = ASSET_ID_INVALID;
    AssetGroupID _group = ASSET_GROUP_INVALID;
};
/// AssetID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AssetGroupID functions

FREYA_API AssetGroupID asset_group_create(const String& name);

FREYA_API void asset_group_destroy(const AssetGroupID& group_id);

FREYA_API bool asset_group_build(const AssetGroupID& group_id, const FilePath& list_path, const FilePath& output_path);

FREYA_API bool asset_group_load_package(const AssetGroupID& group_id, const FilePath& frpkg_path);

FREYA_API const AssetID& asset_group_get_id(const AssetGroupID& group_id, const String& asset_name);

/// AssetGroupID functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////
