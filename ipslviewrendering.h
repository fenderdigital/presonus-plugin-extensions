//************************************************************************************************
//
// PreSonus Plug-In Extensions
// Written and placed in the PUBLIC DOMAIN by PreSonus Software Ltd.
//
// Filename    : ipslviewrendering.h
// Created by  : PreSonus Software Ltd., 09/2023
// Description : Plug-in View Rendering Interfaces
//
//************************************************************************************************
/*
	DISCLAIMER:
	PreSonus Plug-In Extensions are host-specific extensions of existing proprietary technologies,
	provided to the community on an AS IS basis. They are not part of any official 3rd party SDK and
	PreSonus is not affiliated with the owner of the underlying technology in any way.
*/
//************************************************************************************************

#ifndef _ipslviewrendering_h
#define _ipslviewrendering_h

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/falignpush.h"

namespace Steinberg {
struct ViewRect; }

namespace Presonus {

/**	Platform type for IPlugView::attached() when using IPlugViewRendering interface,
	parent argument is null in this case. */
const Steinberg::FIDString kPlatformTypePlugViewRendering = "IPlugViewRendering";

/** Identifer for RGBA pixel format. */
constexpr Steinberg::int32 kPixelFormatRGBA = 'RGBA';

/** Coordinate unit physical pixels. */
constexpr Steinberg::int32 kCoordinateUnitPhysicalPixels = 0;

/** Coordinate unit scalable points, i.e. pixels = points x scaling factor */
constexpr Steinberg::int32 kCoordinateUnitScalablePoints = 1;

//************************************************************************************************
// BitmapPixelBuffer
//************************************************************************************************

struct BitmapPixelBuffer
{
	Steinberg::int32 width = 0;					///< bitmap width
	Steinberg::int32 height = 0;				///< bitmap height
	Steinberg::int32 format = kPixelFormatRGBA;	///< pixel format
	Steinberg::int32 rowBytes = 0;				///< offset between scanlines in bytes (can be negative if image is bottom-up!)
	void* scan0 = nullptr;						///< address of first scanline
};

//************************************************************************************************
// IBitmapAccessor
//************************************************************************************************

struct IBitmapAccessor: Steinberg::FUnknown
{
	/** Lock pixel buffer. */
	virtual Steinberg::tresult PLUGIN_API lockPixelBuffer (BitmapPixelBuffer* buffer) = 0;

	/** Unlock pixel buffer. */
	virtual Steinberg::tresult PLUGIN_API unlockPixelBuffer (BitmapPixelBuffer* buffer) = 0;

	static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (IBitmapAccessor, 0x1c4b3ab0, 0x76384cb2, 0x8adafd1b, 0xdd198055)

//************************************************************************************************
// BitmapLockScope
//************************************************************************************************

struct BitmapLockScope
{
	BitmapLockScope (IBitmapAccessor* accessor)
	: accessor (accessor),
	  result (Steinberg::kResultFalse)
	{
		if(accessor)
			result = accessor->lockPixelBuffer (&data);
	}

	~BitmapLockScope ()
	{
		if(accessor && result == Steinberg::kResultOk)
			accessor->unlockPixelBuffer (&data);
	}

	IBitmapAccessor* accessor;
	BitmapPixelBuffer data;
	Steinberg::tresult result;
};

//************************************************************************************************
// IPlugViewCoordinateUnitSupport
/** Interface to query coordinate unit, to be implemented by the VST3 IPlugView class.
	@ingroup viewExt */
//************************************************************************************************

struct IPlugViewCoordinateUnitSupport: Steinberg::FUnknown
{
	/**	Check which coordinate unit is used by the plug-in view, i.e. kCoordinateUnitPhysicalPixels
		or kCoordinateUnitScalablePoints. This decision affects all coordinates for rendering
		(IPlugViewRendering and IPlugRenderingFrame) and mouse input (IPlugViewMouseInput). */
	virtual Steinberg::int32 PLUGIN_API getCoordinateUnit () = 0;

	static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (IPlugViewCoordinateUnitSupport, 0xeae3ebb, 0xb301468a, 0xa127bd34, 0x8fab0824)

//************************************************************************************************
// IPlugViewRendering
/** Support for plug-in view renderering, to be implemented by the VST3 IPlugView class.
	@ingroup viewExt */
//************************************************************************************************

struct IPlugViewRendering: Steinberg::FUnknown
{
	/** Check if given rendering type and format is supported.
		For bitmaps, this is IBitmapAccessor::iid and kPixelFormatRGBA. */
	virtual Steinberg::tresult PLUGIN_API isRenderingTypeSupported (Steinberg::TUID type, Steinberg::int32 format) = 0;

	/**	Render plug-in view to given target.
		Rendering target can be a bitmap (IBitmapAccessor). */
	virtual Steinberg::tresult PLUGIN_API render (Steinberg::FUnknown* target, Steinberg::ViewRect* updateRect) = 0;

	static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (IPlugViewRendering, 0x215519ce, 0xb4de449f, 0x9572b7f2, 0x4a004a8f)

//************************************************************************************************
// IPlugRenderingFrame
/** Callback interface when view rendering is used. Implemented by host as extension to IPlugFrame.
	@ingroup viewExt */
//************************************************************************************************

struct IPlugRenderingFrame: Steinberg::FUnknown
{
	/** Invalidate given rectangle. */
	virtual Steinberg::tresult PLUGIN_API invalidateViewRect (Steinberg::ViewRect* dirtyRect) = 0;

	static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (IPlugRenderingFrame, 0x68956019, 0x4b964921, 0x9c249f6a, 0xbcff47c6)

//************************************************************************************************
// PlugViewMouseEvent
//************************************************************************************************

struct PlugViewMouseEvent
{
	enum Type
	{
		kMouseDown,
		kMouseUp,
		kMouseEnter,
		kMouseMove,
		kMouseLeave
	};

	enum Button
	{
		kLeftButton,
		kMiddleButton,
		kRightButton
	};

	Steinberg::int32 type = kMouseDown;
	Steinberg::int16 button = kLeftButton;
	Steinberg::int16 modifiers = 0;
	Steinberg::int32 x = 0;
	Steinberg::int32 y = 0;
};

//************************************************************************************************
// IPlugViewMouseInput
/**	Support for mouse input from host to plug-in view, to be implemented by the VST3 IPlugView class.
	Can be combined with rendering via IPlugViewRendering interface.
	@ingroup viewExt */
//************************************************************************************************

struct IPlugViewMouseInput: Steinberg::FUnknown
{
	/** Handling of mouse events. */
	virtual Steinberg::tresult PLUGIN_API onMouseEvent (PlugViewMouseEvent* mouseEvent) = 0;

	static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (IPlugViewMouseInput, 0xc13c4ea4, 0x868e4af7, 0x9614d52c, 0x7cd07b47)

} // namespace Presonus

#include "pluginterfaces/base/falignpop.h"

#endif // _ipslviewrendering_h