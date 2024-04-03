//************************************************************************************************
//
// PreSonus Plug-In Extensions
// Written and placed in the PUBLIC DOMAIN by PreSonus Software Ltd.
//
// Filename    : ipslwaylandframe.h
// Created by  : PreSonus Software Ltd., 09/2023
// Description : Wayland Frame Interfaces
//
//************************************************************************************************
/*
	DISCLAIMER:
	PreSonus Plug-In Extensions are host-specific extensions of existing proprietary technologies,
	provided to the community on an AS IS basis. They are not part of any official 3rd party SDK and
	PreSonus is not affiliated with the owner of the underlying technology in any way.
*/
//************************************************************************************************

#ifndef _ipslwaylandframe_h
#define _ipslwaylandframe_h

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/falignpush.h"

struct wl_display;
struct wl_surface;
struct xdg_surface;
struct xdg_toplevel;

namespace Steinberg {
struct ViewRect; }

namespace Presonus {

/** \defgroup waylandFrame Wayland Frame
 *
 * The following interfaces allow to query information about the host plug-in frame when running in a Wayland session.
 * 
 * A native Wayland host application is both a Wayland client and a Wayland compositor. 
 * The host application connects to the system compositor and creates application windows etc. using this compositor connection.
 * A plug-in does not connect to the system compositor, but connects to the host application by calling IWaylandHost::openWaylandConnection().
 * The host application provides the IWaylandHost interface via IPluginFactory3::setHostContext. The plug-in may also query this interface using an IPlugFrame object.
 * The plug-in must not perform blocking reads using functions like wl_display_roundtrip or wl_display_dispatch.
 * Instead, the Steinberg::Linux::IRunLoop interface should be used to used to register an event handler using the file descriptor returned by wl_display_get_fd.
 * 
 * When opening a plug-in window, the host calls IPlugView::attached() with a null pointer.
 * In order to create the frame surface and additional windows (dialogs, menus, tooltips etc.), the plug-in can use the IWaylandFrame interface, which is implemented by the host's IPlugFrame object.
 * The plug-in creates a wl_surface and must assign the wl_subsurface role using the wl_surface pointer returned by IWaylandFrame::getWaylandSurface. The plug-in is responsible for resizing the subsurface accordingly.
 * In order to create popup windows, the plug-in can use IWaylandFrame::getParentSurface() to query an xdg_surface, which can in turn be used as a parent in xdg_surface_get_popup.
 * Likewise, the plug-in can use IWaylandFrame::getParentToplevel() to query an xdg_toplevel, which can be used in xdg_toplevel_set_parent.
 */

//************************************************************************************************
// Platform UI Types
//************************************************************************************************

/** The host application is a Wayland compositor.
 * The parent parameter in IPlugView::attached() is a null pointer.
 * The plug-in should query information about the host frame using the IWaylandFrame interface, which is implemented by the host's IPlugFrame object.
 * The plug-in should create a wl_surface and attach it to the parent surface as a wl_subsurface.
 * The plug-in should not connect to the system compositor to do so, but use IWaylandHost::openWaylandConnection().
 * @ingroup waylandFrame
 */
const Steinberg::FIDString kPlatformTypeWaylandSurfaceID = "WaylandSurfaceID"; ///< Wayland Surface ID.

//************************************************************************************************
// IWaylandHost
/** Wayland host interface.
	Implemented as a singleton in the host application.
	@ingroup waylandFrame */
//************************************************************************************************

struct IWaylandHost: Steinberg::FUnknown
{
	/** Open a Wayland connection to the host. */
	virtual wl_display* PLUGIN_API openWaylandConnection () = 0;

	/** Close a previously created connection. */
	virtual Steinberg::tresult PLUGIN_API closeWaylandConnection (wl_display* display) = 0;

	static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (IWaylandHost, 0x5E9582EE, 0x86594652, 0xB213678E, 0x7F1A705E)

//************************************************************************************************
// IWaylandFrame
/** Interface to query additional information about the host plug-in frame in a Wayland session.
	To be implemented by the VST3 IPlugFrame class.
	@ingroup waylandFrame */
//************************************************************************************************

struct IWaylandFrame: Steinberg::FUnknown
{
	/** Get the parent Wayland surface.
	 * The plug-in must not change the state of the parent surface.
	 */
	virtual wl_surface* PLUGIN_API getWaylandSurface (wl_display* display) = 0;

	/** Get the parent XDG surface for creating popup windows.
	 * If the parent surface is not an xdg_surface, 
	 *   this returns the first xdg_surface that can be found in the surface hierarchy,
	 *   starting the search with the parent surface.
	 * The plug-in must not change the state of the parent surface.
	 * The size and position of the parent surface, relative to the top left corner of 
	 *   the plug-in surface, is returned in parentSize.
	 */
	virtual xdg_surface* PLUGIN_API getParentSurface (Steinberg::ViewRect& parentSize, wl_display* display) = 0;

	/** Get the XDG toplevel surface containing the plug-in frame.
	 * The plug-in must not change the state of the returned xdg_toplevel. 
	 */
	virtual xdg_toplevel* PLUGIN_API getParentToplevel (wl_display* display) = 0;
	
	static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (IWaylandFrame, 0x809FAEC6, 0x231C4FFA, 0x98ED046C, 0x6E9E2003) 

} // namespace Presonus

#include "pluginterfaces/base/falignpop.h"

#endif // _ipslwaylandframe_h