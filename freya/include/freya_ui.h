#pragma once

#include "freya_file.h"
#include "freya_assets.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// UIAnchor
enum UIAnchor {
  /// Anchor a UI element to the top-left of the screen.
  UI_ANCHOR_TOP_LEFT, 

  /// Anchor a UI element to the top-center of the screen.
  UI_ANCHOR_TOP_CENTER, 

  /// Anchor a UI element to the top-right of the screen.
  UI_ANCHOR_TOP_RIGHT, 

  /// Anchor a UI element to the center-left of the screen.
  UI_ANCHOR_CENTER_LEFT, 

  /// Anchor a UI element to the center of the screen.
  UI_ANCHOR_CENTER, 

  /// Anchor a UI element to the center-right of the screen.
  UI_ANCHOR_CENTER_RIGHT, 

  /// Anchor a UI element to the bottom-left of the screen.
  UI_ANCHOR_BOTTOM_LEFT, 

  /// Anchor a UI element to the bottom-center of the screen.
  UI_ANCHOR_BOTTOM_CENTER, 

  /// Anchor a UI element to the bottom-right of the screen.
  UI_ANCHOR_BOTTOM_RIGHT, 
};
/// UIAnchor
///---------------------------------------------------------------------------------------------------------------------
/// ----------------------------------------------------------------------
/// UITextDesc
struct UITextDesc {
  /// The string to be set on the text UI element.
  String string;

  /// The ID of the font to be used later.
  AssetID font_id;

  /// The total size of the font of the text UI element.
  f32 font_size;

  /// The anchor point of the text UI element.
  UIAnchor anchor;

  /// The bounds of the canvas that the text will 
  /// be placed on. The UI element will calculate 
  /// its anchor point based on this.
  Vec2 canvas_bounds;
  
  /// The blur amount to be applied to the text UI element.
  ///
  /// @NOTE: This is set to `0.0f` by default.
  f32 blur_effect = 0.0f;
  
  /// The spacing to be applied to the text UI element.
  ///
  /// @NOTE: This is set to `1.0f` by default.
  f32 spacing     = 1.0f;

  /// The extra offset to be applied to the text UI element.
  ///
  /// @NOTE: This is set to `Vec2(0.0f, 0.0f)` by default.
  Vec2 offset     = Vec2(0.0f);

  /// The color of the text UI element.
  ///
  /// @NOTE: This is set to `Vec4(1.0f, 1.0f, 1.0f, 1.0f)` by default.
  Vec4 color      = Vec4(1.0f);
};
/// UITextDesc
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIText
struct UIText {
  UIAnchor anchor;
  int align = 0;

  Vec2 position, offset, bounds;
  Vec2 canvas_bounds;

  String string;
  Font* font;

  f32 size, blur, spacing;

  Vec4 color;
  bool is_active;
};
/// UIText
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIText functions

/// Fill in the information of `out_text` using the given `desc`.
FREYA_API void ui_text_create(UIText& out_text, const UITextDesc& desc);

/// Using the `anchor` member, place the given `text` according to 
/// the `canvas_bounds` in the correct position.
FREYA_API void ui_text_place(UIText& text);

/// UIText functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////
