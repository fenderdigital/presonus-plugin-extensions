//************************************************************************************************
//
// PreSonus Plug-In Extensions
// Written and placed in the PUBLIC DOMAIN by PreSonus Software Ltd.
//
// Filename    : ipslspeakerinfo.h
// Created by  : PreSonus Software Ltd., 04/2024
// Description : Speaker Arrangement Support Info Interface
//
//************************************************************************************************
/*
	DISCLAIMER:
	PreSonus Plug-In Extensions are host-specific extensions of existing proprietary technologies,
	provided to the community on an AS IS basis. They are not part of any official 3rd party SDK and
	PreSonus is not affiliated with the owner of the underlying technology in any way.
*/
//************************************************************************************************

#ifndef _ipslspeakerinfo_h
#define _ipslspeakerinfo_h

#include "pluginterfaces/vst/vsttypes.h"
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/falignpush.h"

namespace Presonus {

/** @defgroup speakerInfo Speaker Arrangement Support Info */

//************************************************************************************************
// ISpeakerArrangementInfo
/**	Interface to query information about speaker arrangement support.	
	- Implemented by plug-in as extension of Steinberg::Vst::IComponent 
		
	@ingroup speakerInfo */
//************************************************************************************************

struct ISpeakerSupportInfo: Steinberg::FUnknown
{	
	/** Report if the speaker arrangement is supported on the a specific bus */
	virtual Steinberg::TBool PLUGIN_API isArrangementSupportedOnBus (Steinberg::Vst::SpeakerArrangement arr, Steinberg::Vst::BusDirection dir, Steinberg::int32 index) = 0;

    static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (ISpeakerSupportInfo, 0x7342e0eb, 0x8f5641de, 0xa5f7c503, 0x8e2ec3ef)

//************************************************************************************************
// ISpeakerArrangementHostInfo
/**	Interface to query information about speaker arrangement support.	
	- Implemented by host as extension of Steinberg::Vst::IComponentHandler. 
		
	@ingroup speakerInfo */
//************************************************************************************************

struct ISpeakerSupportHostInfo: Steinberg::FUnknown
{	
	/** Report if the given speaker arrangement is supported by host. */
	virtual Steinberg::TBool PLUGIN_API isSpeakerArrangementSupported (Steinberg::Vst::SpeakerArrangement arr) = 0;

    static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (ISpeakerSupportHostInfo, 0x3327e14a, 0x055e4d27, 0x9a0f6b4a, 0x36316e7b)

} // namespace Presonus

#include "pluginterfaces/base/falignpop.h"

#endif // _ipslspeakerinfo_h