//************************************************************************************************
//
// PreSonus Plug-In Extensions
// Written and placed in the PUBLIC DOMAIN by PreSonus Software Ltd.
//
// Filename    : ipslinstrumentcontroller.h
// Created by  : PreSonus Software Ltd., 10/2020
// Description : Plug-in Instrument Extension Interface
//
//************************************************************************************************
/*
	DISCLAIMER:
	PreSonus Plug-In Extensions are host-specific extensions of existing proprietary technologies,
	provided to the community on an AS IS basis. They are not part of any official 3rd party SDK and
	PreSonus is not affiliated with the owner of the underlying technology in any way.
*/
//************************************************************************************************

#ifndef _ipslinstrumentcontroller_h
#define _ipslinstrumentcontroller_h

#include "pluginterfaces/vst/vsttypes.h"
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/falignpush.h"

namespace Presonus {

/** @defgroup instrument Instrument Extensions 

The following interfaces allow to support extended features of a synth plug-in.
-# Report Key Assignment. Used in the host to display the function and name of key on a musical keyboard
-# Report which editor is used best with this instrument (Piano / Drum)
-# Report or set the "Middle C" 
*/

//************************************************************************************************
// KeyAssignment
/** Assignment of a single Key / Pitch / Note	
	@ingroup instrument */
//************************************************************************************************

struct KeyAssignment
{				
	Steinberg::int16 midiPitch;        ///< the pitch
	Steinberg::int32 type;             ///< assignment type \see Type
	Steinberg::Vst::String128 title;   ///< optional
	Steinberg::Vst::ColorSpec color;   ///< optional

	enum Type
	{				
		kSustainable = 0,  ///< sound starts with note on, ends with note off
		kOneShot = 1,      ///< sound starts with note on, note off does not end it, sound always plays for a defined time
		kFunction = 2      ///< the pitch is assingned to a function (like a keyswitch)		
	};

	KeyAssignment (Steinberg::int16 pitch, Type _type): midiPitch (pitch), type (_type), color (0) { title[0] = 0; }
};

//************************************************************************************************
// IKeyAssignmentReceiver
/** Callback interface for retrieving key assignemtns
	The host implements this to retrieve key assignments from the plug-in.
	
	@ingroup instrument */
//************************************************************************************************

struct IKeyAssignmentReceiver
{
	/** Add info for keys/pitches (in display order). */
	virtual void PLUGIN_API addKeyAssignment (const KeyAssignment& info) = 0;
};

//************************************************************************************************
// IInstrumentObserver
/** Observer interface implemented by the host. Notify the host that instrument data has changed. 	
	
	@ingroup instrument */
//************************************************************************************************

struct IInstrumentObserver
{
	/** Values used for IInstrumentObserver::onInstrumentChanged */
	enum ChangeMessage
	{
		/** Notify the host that key assignment info changed. 
		  * The host should call IInstrumentController::getKeyAssignment and discard any cached data.	*/
		kKeyAssignmentChanged = 1, 
		                            
		/** Notify the host that the instrument type changed. 
		  * The host should call IInstrumentController::isDrumInstrument in return.	*/
		kIsDrumInstrumentChanged = 2,

		/** Notify the host that the middle C settings changed. 
		  * The host should call IInstrumentController::getMiddleCValue in return.	*/
		kMiddleCChanged = 3
	};

	/** Notify the host that instrument data has changed. This method must be called in the main thread. 
	    If the change value is only valid for a certain synth unit, busIndex and channel should be used to specify this.*/	
	virtual void PLUGIN_API onInstrumentInfoChanged (Steinberg::int32 changeMessage, Steinberg::int32 busIndex = -1, Steinberg::int16 channel = -1) = 0;
};

//************************************************************************************************
// IInstrumentController
/**	Extension to Steinberg::Vst::IEditController. 
	Implemented by plug-in.
	
	@ingroup instrument */
//************************************************************************************************

struct IInstrumentController: Steinberg::FUnknown
{	
	enum Feature
	{
		kReportKeyAssignment = 1,    ///< getKeyAssignment is supported
		kReportDrumInstrument = 2,   ///< isDrumInstrument is supported
		kReportMiddleC = 3,          ///< middle C can be queried
		kModifyMiddleC = 4           ///< middle C can be set via  setMiddleCValue 
	};

	/** Check if a certain instrument feature is supported. 
	  * The plug-in should store the pointer and report whenever data changes */
	virtual Steinberg::TBool PLUGIN_API isInstrumentFeatureSupported (Steinberg::int32 which) = 0;			

	/** Receive callback interface to report instrument changes. 
	  * The plug-in should store the pointer and report whenever data changes */
	virtual Steinberg::tresult PLUGIN_API setInstrumentObserver (IInstrumentObserver* observer) = 0;
	
	/** Get the current key assignment for a synth unit (busIndex + channel). 
	  *  \return kResultTrue on success */
	virtual Steinberg::tresult PLUGIN_API getKeyAssignment (IKeyAssignmentReceiver& result, Steinberg::int32 busIndex, Steinberg::int16 channel) = 0;
	
	/** Suggest to use the drum editor for editing. */
	virtual Steinberg::TBool PLUGIN_API isDrumInstrument (Steinberg::int32 busIndex, Steinberg::int16 channel) = 0;

	/** Return the current middle C midi pitch. \return kResultTrue on success, kNotImplemented if not implemented, kResultFalse otherwise */
	virtual Steinberg::tresult PLUGIN_API getMiddleCValue (int& pitch) = 0;
	
	/** Set middle C midi pitch. \return kResultTrue on success, kNotImplemented if not implemented, kResultFalse otherwise */
	virtual Steinberg::tresult PLUGIN_API setMiddleCValue (int pitch) = 0;

	static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (IInstrumentController, 0xd2ce9317, 0xf24942c9, 0x9742e82d, 0xb10ccc52)

//************************************************************************************************
// DefaultInstrumentController
/**	Mix-in class implementing optional methods as default	
	@ingroup instrument */
//************************************************************************************************

class DefaultInstrumentController: public IInstrumentController
{
public:	
	Steinberg::tresult PLUGIN_API getKeyAssignment (IKeyAssignmentReceiver& result, Steinberg::int32 busIndex, Steinberg::int16 channel)
	{ return Steinberg::kNotImplemented; }
		
	Steinberg::TBool PLUGIN_API isDrumInstrument (Steinberg::int32 busIndex, Steinberg::int16 channel)
	{ return false; }

	Steinberg::tresult PLUGIN_API getMiddleCValue (int& pitch)
	{ return Steinberg::kNotImplemented; }
	
	Steinberg::tresult PLUGIN_API setMiddleCValue (int pitch)
	{ return Steinberg::kNotImplemented; }
};

} // namespace Presonus

#include "pluginterfaces/base/falignpop.h"

#endif // _ipslinstrumentcontroller_h
