//************************************************************************************************
//
// PreSonus Plug-In Extensions
// Written and placed in the PUBLIC DOMAIN by PreSonus Software Ltd.
//
// Filename    : ipslsoundvariation.h
// Created by  : PreSonus Software Ltd., 10/2020
// Description : Plug-in Sound Variation Interface
//
//************************************************************************************************
/*
	DISCLAIMER:
	PreSonus Plug-In Extensions are host-specific extensions of existing proprietary technologies,
	provided to the community on an AS IS basis. They are not part of any official 3rd party SDK and
	PreSonus is not affiliated with the owner of the underlying technology in any way.
*/
//************************************************************************************************

#ifndef _ipslsoundvariation_h
#define _ipslsoundvariation_h

#include "pluginterfaces/vst/vsttypes.h"
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/falignpush.h"

namespace Presonus {

/** \defgroup soundVariations Sound Variations 

The following interfaces allow to report available sound variations of a synth.
 
Sound Variations are alterations of a loaded sound - also known as articulations. 
Usually they are realized as sample layers and often enabled with a key switch note.
The main purpose of this interface extension is to 
- Support complex event sequences that enable variations.
- Report variation names, colors and how variations are organized in folders
- Report which is the active variation
- Report the name of the sound preset the current variations belong to

Overview:
- The main entry point for this feature is ISoundVariationController provided by the plug-in. 
- For each unit the plug-in provides a ISoundVariationInfo (addressed via bus and channel index).
- The host will can query variations via ISoundVariationInfo::getVariationList. 
- The plug-in must report changes to the list with ISoundVariationObserver::onSoundVariationsChanged
- The host will activate variations either by sending the reported sound activation sequence or using a special Event (optional - if the plug-in supports this)
*/

typedef Steinberg::int16 Pitch;
typedef Steinberg::int16 CCNumber;
typedef Steinberg::int16 CCValue;
typedef Steinberg::int32 VariationID;
typedef Steinberg::uint32 ScoreSymbolID;

//************************************************************************************************
// SoundActivationSequenceItem
/** Sound Activation Sequence Item. 
	One item of a sound variation activation sequence 

	@ingroup soundVariations */
//************************************************************************************************

struct SoundActivationSequenceItem 
{ 
	enum ItemTypes
	{
	/** Note event: Host should send a note-on followed by note off. 
	  * The note off is either ignored in the plug-in or it terminates momentary sound variations. */
		kNoteEvent         = 0, ///< is Note
	
	/**  Single note-on event. */
		kNoteOnEvent       = 1,			///< is Note
		
	/**  Single note-off event. */
		kNoteOffEvent      = 2,			///< is Note 
		
	/** Controller event */
		kControlEvent      = 3,			///< is Controller

	/** Program change */
		kProgramChange     = 4			///< is ProgramChange
	};

	struct Note
	{
		Pitch pitch;
		float velocity;

		Note (Pitch p = 0, float v = 1.f)
		: pitch (p), velocity (v) 
		{}
	};
		
	struct Controller
	{
		CCNumber number;
		CCValue value;

		Controller (CCNumber n, CCValue v)
		: number (n), value (v)
		{}
	}; 

	struct ProgramChange
	{
		CCValue value;

		ProgramChange (CCValue v)
		: value (v)
		{}
	}; 

	int type;
	union 
	{ 
		Note note;
		Controller controller;
		ProgramChange programChange;
		Steinberg::int32 placeHoder[16];
	};

	SoundActivationSequenceItem (): type (kNoteEvent), note (0)
	{}
};

//************************************************************************************************
// SoundActivationSequence
/** Sound Activation Sequence. List of events that activate a sound variation in the plug-in. 
    In most cases an activation sequence has only one item, for example a simple key switch. 
	If your instrument provides a multi-level activation system that needs more than one event to
	activate a variation, the activation sequence should contain all events that will trigger it.
	All events will be sent to the instrument in sequence in order to activate a variation.
	On the other hand, if there are multiple ways to trigger a variation, the activation sequence 
	should present one way only. 

	@ingroup soundVariations */
//************************************************************************************************

struct SoundActivationSequence
{ 
	static const int kMaxItems = 8;
	static int getMaxItems () { return kMaxItems; }

	int count; 
	SoundActivationSequenceItem items[kMaxItems]; 
	
	SoundActivationSequence (): count (0) {}
	
	/** Remove all. */
	void clear () { count = 0; }

	/** Add sound activation sequence item  */
	void addItem (const SoundActivationSequenceItem& item)
	{
		if(count < kMaxItems)
			items[count++] = item;	
	}

	/** Add a note event (@see SoundActivationSequenceItem)  */
	void addNote (const SoundActivationSequenceItem::Note& note, int type = SoundActivationSequenceItem::kNoteEvent)
	{
		if(count < kMaxItems)
		{
			items[count].type = type;
			items[count++].note = note;
		}
	}
	
	/** Add a controller event.
	  * A VST3 plug-in needs to support IMidiMapping interface and will receive MIDI-CC as parameter changes */
	void addController (const SoundActivationSequenceItem::Controller& controller)
	{
		if(count < kMaxItems)
		{
			items[count].type = SoundActivationSequenceItem::kControlEvent;
			items[count++].controller = controller;
		}
	}

	/** Add a program change event. */
	void addProgramChange (CCValue value)
	{
		if(count < kMaxItems)
		{
			items[count].type = SoundActivationSequenceItem::kProgramChange;
			items[count++].programChange = SoundActivationSequenceItem::ProgramChange (value);
		}
	}
};

//************************************************************************************************
// ScoreSymbolList
/** Score Symbol List.    
	Combination of notation symbols associated with a sound variation. Plug-in can suggest a unique
	score symbol comination that should trigger the variation. 

	@ingroup soundVariations */
//************************************************************************************************

struct ScoreSymbolList
{ 
	static const int kMaxItems = 4;
	static int getMaxItems () { return kMaxItems; }

	int count;
	ScoreSymbolID symbols[kMaxItems]; 

	ScoreSymbolList (): count (0) {}
	void clear () { count = 0; }

	void addSymbol (ScoreSymbolID symbol)
	{
		if(count < kMaxItems)
			symbols[count++] = symbol;
	}
};

//************************************************************************************************
// SoundVariationData
/** Sound Variation. 
	Used with ISoundVariationList::addVariation to report a single variation

	@ingroup soundVariations */
//************************************************************************************************

struct SoundVariationData
{				
	/** Each variation needs an id which must be unique in the reported list. 
        This ID is used to address variations. It can be implemented as a simple counter as long as it meets the following rules
		  - The id must be the same each time the sound variations are queried.
		  - The id must be the same each time a sound is loaded.
		  - If the synth gui allows to move or edit a variation, the ids of all reported variations must not change.
 	    
		The id will be stored with trigger events of host documents. 
		They are also used to transmit sound variation activation requests via Vst3SoundVariationEvent / Vst2SoundVariationEvent */
	VariationID identifier;         
	
	/** Displayed name of the variation. */
	Steinberg::Vst::String128 title;

	/** Specify how the variation is activated. 
	  * Alternatively the plug-in can support Sound Variation Events.
  	    If the variation is triggered with a simple key switch, this should be always reported as activation sequence, even if Sound Variation Events are supported */

	SoundActivationSequence activationSequence; 

	Steinberg::Vst::ColorSpec color; ///< optional - set to 0 if not provided
	Pitch triggerPitch;              ///< optional - synth can suggest a default key switch, set to -1 if not provided. If the variation is triggered with a simple key switch, set the triggerPitch accordingly
	ScoreSymbolList scoreSymbols;    ///< optional - @see ScoreSymbolList
	Steinberg::int32 flags;          ///< optional - @see Flags

	enum Flags
	{
	/** Previous sound variation is re-enabled when this variation terminates. 
	  * Activation sequence can only be a note (with length) - and / or kTerminateTypeId must be supported */
		kIsMomentary = 1 << 0,
	
	/** Main or default variation which is active on loading the sound preset. 
	  * No more than one variation should have this flag. */	
		kIsDefault = 1 << 1        
	};

	SoundVariationData (VariationID id = -1): identifier (id), color (0), triggerPitch (-1), flags (0) { title[0] = 0; }
};

//************************************************************************************************
// SoundVariationFolderData
/** Sound Variation Folder. 
	Used with ISoundVariationList::begin folder to report nested variations 

	@ingroup soundVariations */
//************************************************************************************************

struct SoundVariationFolderData
{
	/** Displayed name of the folder */
	Steinberg::Vst::String128 title; 

	/** Color - opional (set to 0 if not available) */
	Steinberg::Vst::ColorSpec color; 

	Steinberg::int32 flags;          ///< optional - @see Flags

	enum Flags
	{
	/** Prepend the title of this folder to display variation names */
		kAddTitleToVariations = 1 << 0	
	};

	SoundVariationFolderData (): color (0),  flags (0) { title[0] = 0; }
};

//************************************************************************************************
// SoundVariationPresetInfo
/** Info about the loaded sound preset. 
	The host uses this to
	- display the preset name together with the sound variation list.
	- automatically store/restore additional data with the sound variations  

	@ingroup soundVariations */
//************************************************************************************************

struct SoundVariationPresetInfo
{
	/** Displayed name of the preset */
	Steinberg::Vst::String128 name; 

	/** Internal qualifier to resolve name clashes (optional). 
	  * Not intended to be displayed, but should be valid for file systems
	  * so the host can use it to create folders and organize sound variation map presets. */
	Steinberg::Vst::String128 path; 
};

//************************************************************************************************
// ISoundVariationList
/** Callback interface passed to ISoundVariationInfo::getVariationList.
	The host implements this to retrieve the variations provided by the plug-in.  

	@ingroup soundVariations */
//************************************************************************************************

struct ISoundVariationList
{
	/** Append variation to the list. This can be done on top-level or inside folders. */	
	virtual void PLUGIN_API addVariation (const SoundVariationData& var) = 0;

	/** Optional. Begin an new folder. All following sound variation will be added to the current folder. */
	virtual void PLUGIN_API beginFolder (const SoundVariationFolderData& folderData) = 0;
	
	/** Each started folder needs to be closed again. */
	virtual void PLUGIN_API endFolder () = 0;
	
	/** Tell the host which sound preset the reported variations belong to. \see SoundVariationPresetInfo */
	virtual void PLUGIN_API setPresetInfo (const SoundVariationPresetInfo& info) = 0;
};

//************************************************************************************************
// ISoundVariationObserver
/** Observer interface implemented by the host. 
	Please note that this interface should not be used inside audio processing calls.

	@ingroup soundVariations */
//************************************************************************************************

struct ISoundVariationObserver
{
	/** Values used for ISoundVariationObserver::onSoundVariationsChanged */
	enum ChangeMessage
	{
		/** Notify the host that a new sound preset was loaded. 
		  * The host should call ISoundVariationInfo::getVariationList and discard any cached variation data.	*/
		kPresetChanged = 0,         

		/** Notify the host that the variation list of a loaded sound preset have been edited. 
		  * The host should call ISoundVariationInfo::getVariationList to retrieve modifications of the variation list. 
		  * Cached variation data should be adjusted. */		
		kVariationListModified = 1, 
		                            
		/** Notify the host that the active variation has changed. 
		  * The host should call ISoundVariationInfo::getActiveVariation in return. */
		kActiveVariationChanged = 2
	};

	/** Notify the host that the sound variation info has changed. 
	    This method should be called in the main thread if possible.
	    \param changeMessage is one value of enum ChangeMessage. */
	virtual void PLUGIN_API onSoundVariationsChanged (Steinberg::int32 changeMessage) = 0;
};

//************************************************************************************************
// ISoundVariationInfo
/**	Report sound variations for a synth unit (busIndex + channel). 
	Implemented by plug-in. 

	@ingroup soundVariations*/
//************************************************************************************************

struct ISoundVariationInfo: Steinberg::FUnknown
{	
	/** Set interface to report changes of sound variation list. 
	  * The plug-in should store the pointer and report the cases defined by ISoundVariationObserver::ChangeMessage to the host */
	virtual Steinberg::tresult PLUGIN_API setVariationObserver (ISoundVariationObserver* handler) = 0;
	
	/** Get the current sound variations. The plug-in should fill the given varition list.
	  *  \return kResultTrue on success */
	virtual Steinberg::tresult PLUGIN_API getVariationList (ISoundVariationList& list) = 0;

	/** Get the currently active sound variation. 
 	  *  \return kResultTrue on success */
	virtual Steinberg::tresult PLUGIN_API getActiveVariation (VariationID& variation) = 0;
	
	static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (ISoundVariationInfo, 0xe59066c0, 0x41d940bc, 0x8f88cbb9, 0xa337e20a)

//************************************************************************************************
// ISoundVariationController
/**	Main interface for sound variation reporting. 
	Extension to Steinberg::Vst::IEditController. 

	@ingroup soundVariations */
//************************************************************************************************

struct ISoundVariationController: Steinberg::FUnknown
{	
	/** Get the variation info for the synth unit addressed by busIndex + channel.
	    Note for VST3: If the plug-in supports multiple event inputs, Steinberg::Vst::IUnitInfo::getUnitByBus needs to be supported.  */	
	virtual ISoundVariationInfo* PLUGIN_API getSoundVariationInfo (Steinberg::int32 busIndex, Steinberg::int16 channel) = 0;

	// optional:

	/** Return true if the plug-in can handle events of type Vst3SoundVariationEvent / Vst2SoundVariationEvent */
	virtual Steinberg::TBool PLUGIN_API isSoundVariationEventSupported () = 0;

	/** Return true if the plug-in supports disableKeySwitches */
	virtual Steinberg::TBool PLUGIN_API isDisableKeySwitchesSupported () = 0;

	/** Enable a mode that ignores all activation sequence events and only handle Vst3SoundVariationEvent / Vst2SoundVariationEvent */
	virtual Steinberg::tresult PLUGIN_API disableKeySwitches (Steinberg::TBool state) = 0;
	
	/** Return true if the disableKeySwitches mode is active */
	virtual Steinberg::TBool PLUGIN_API areKeySwitchesDisabled () = 0;

	static const Steinberg::FUID iid;
};

DECLARE_CLASS_IID (ISoundVariationController, 0x3abdfc3e, 0x4b964a66, 0xafcd86f10, 0x0d554023)

//************************************************************************************************
// Vst3SoundVariationEvent (VST3)
/** Event sent to a synth in order to activate a sound variation.
	Events of this type are sent only if ISoundVariationController::isSoundVariationEventSupported returns true.
	A Steinberg::Vst::Event which has the type SoundVariationEvent::kTypeId must be reinterpret-casted to
	Presonus::SoundVariationEvent and the according sound variation should be activated.

	see also Steinberg::Vst::Event
	
	@ingroup soundVariations */
//************************************************************************************************

struct Vst3SoundVariationEvent 
{
	static const Steinberg::int16 kTypeId = 'VE';          ///< Activate sound variation
	static const Steinberg::int16 kTerminateTypeId = 'VT'; ///< Terminate momentary sound variation

	Steinberg::int32 busIndex;                  ///< event bus index
	Steinberg::int32 sampleOffset;              ///< sample frames related to the current block start sample position
	Steinberg::Vst::TQuarterNotes ppqPosition;  ///< position in project
	Steinberg::uint16 flags;                    ///< combination of \ref EventFlags - and kTerminateMomentary
	Steinberg::uint16 type;                     ///< Vst3SoundVariationEvent::kType
	Steinberg::int32 channel;                   ///< channel index in event bus (like NoteOnEvent)
	VariationID variationId;                    ///< id of variation to be activated
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// VST 2 specific
//////////////////////////////////////////////////////////////////////////////////////////////////

/** VST2 opcode passed to AEffectDispatcherProc to get ISoundVariationController instance.\n 
	Host call looks like this:
	
	@code
		if(effect->dispatch (effect, effCanDo, 0, 0, (void*)Presonus::PlugCanDos::canDoGetSoundVariationController) == 1)
		{
			Presonus::ISoundVariationController* soundVariationController = 0; 
			effect->dispatcher (effect, effVendorSpecific, Presonus::kVendorID, Presonus::kGetSoundVariationController, &soundVariationController)); 
			// ...
		}
	@endcode	

	Plug-In side looks like this:

	@code
		VstIntPtr MySynth::dispatcher (VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
		{
			if(opcode == effVendorSpecific && index == Presonus::kVendorID && value == Presonus::kGetSoundVariationController)
			{
				Presonus::ISoundVariationController** result = (Presonus::ISoundVariationController**)ptr;
				if(soundVariationController == 0)
					soundVariationController = new SoundVariationController;
		
				*result = soundVariationController;
				return 1;
			}
			return AudioEffectX::dispatcher (opcode, index, value, ptr, opt);
		}
	@endcode	

	@ingroup soundVariations
*/

static const int kGetSoundVariationController = 'GSVC';

namespace PlugCanDos
{
	/** Check if plug-in can provide a ISoundVariationController. */
	static const char* canDoGetSoundVariationController = "getSoundVariationController";
}

//************************************************************************************************
// Vst2SoundVariationEvent
/** VST2 Sound Activation Event.
	Event sent to a synth in order to activate a sound variation. 
	See also VstEvent 

	@ingroup soundVariations */
//************************************************************************************************

struct Vst2SoundVariationEvent    
{
	static const Steinberg::int32 kTypeId = 'PSVE';          ///< Activate sound variation
	static const Steinberg::int32 kTerminateTypeId = 'PSVT'; ///< Terminate momentary sound variation

	Steinberg::int32 type;        ///< Vst2SoundVariationEvent::kTypeId or Vst2SoundVariationEvent::kTerminateTypeId
	Steinberg::int32 byteSize;    ///< sizeof(Vst2SoundVariationEvent) - 2 * sizeof(VstInt32);
	Steinberg::int32 deltaFrames; ///< sample frames related to the current block start sample position
	Steinberg::int32 flags;       ///< @see VstMidiEventFlags
	Steinberg::int32 channel;     ///< MIDI channel
	VariationID variationId;      ///< id of variation to be activated	
};

//************************************************************************************************
// SymbolID
/** Symbol definitions.	@ingroup soundVariations  */
//************************************************************************************************

struct SymbolID
{
	static const ScoreSymbolID kStaccato          = 'stac'; 
	static const ScoreSymbolID kStaccatissimo     = 'stis';
	static const ScoreSymbolID kTenuto            = 'tenu';
	static const ScoreSymbolID kAccent            = 'acce'; 
	static const ScoreSymbolID kStrongAccent      = 'marc'; 
	static const ScoreSymbolID kForceFP           = 'fpno';    
	static const ScoreSymbolID kForceFFP          = 'ffpn';        
	static const ScoreSymbolID kForceFZ           = 'fzdo';
	static const ScoreSymbolID kForceFFZ          = 'ffzo';
	static const ScoreSymbolID kForceSF           = 'sfdo';          
	static const ScoreSymbolID kForceSFF          = 'sffo';
	static const ScoreSymbolID kForceSFZ          = 'sfzo';
	static const ScoreSymbolID kForceSFFZ         = 'sffz';
	static const ScoreSymbolID kForceSFP          = 'sfpo';
	static const ScoreSymbolID kForceSFFP         = 'sffp';
	static const ScoreSymbolID kMezzoStaccato             = 'mzsc'; ///< kStaccato | kTenuto
	static const ScoreSymbolID kAccentTenuto              = 'actn'; ///< kAccent | kTenuto
	static const ScoreSymbolID kAccentStaccato            = 'acst'; ///< kAccent | kStaccato,
	static const ScoreSymbolID kAccentStaccatissimo       = 'acso'; ///< kAccent | kStaccatissimo
	static const ScoreSymbolID kStrongAccentTenuto        = 'mrtn'; ///< kStrongAccent | kTenuto
	static const ScoreSymbolID kStrongAccentStaccato      = 'mrst'; ///< kStrongAccent | kStaccato
	static const ScoreSymbolID kStrongAccentStaccatissimo = 'mrso'; ///< kStrongAccent | kStaccatissimo
	static const ScoreSymbolID kTremolo1          = 'trm1';  ///< tremolo one slash (note repeated)
	static const ScoreSymbolID kTremolo2          = 'trm2';  ///< tremolo 2 slashes
	static const ScoreSymbolID kTremolo3          = 'trm3';  ///< tremolo 3 slashes
	static const ScoreSymbolID kIntervalTremolo1  = 'itr1';  ///< tremolo with next note / fingered tremolo
	static const ScoreSymbolID kIntervalTremolo2  = 'itr2';
	static const ScoreSymbolID kIntervalTremolo3  = 'itr3';					   
	static const ScoreSymbolID kArpeggioNormal    = 'arpN';    
	static const ScoreSymbolID kArpeggioUp        = 'arpU';    
	static const ScoreSymbolID kArpeggioDown      = 'arpD';    					   
	static const ScoreSymbolID kGlissando         = 'glss';
	static const ScoreSymbolID kPortamento        = 'port';	
	static const ScoreSymbolID kSlur              = 'slur'; ///< legato
	static const ScoreSymbolID kTrillHalftone     = 'trHT';
	static const ScoreSymbolID kTrillWholetone    = 'trWT'; 
	static const ScoreSymbolID kVibrato           = 'vibr'; 
	static const ScoreSymbolID kWideVibrato       = 'wvib';
	static const ScoreSymbolID kCircle            = 'circ'; ///< o meaning depends on instrument: open / harmonic (flageolett)
	static const ScoreSymbolID kPlus              = 'plus'; ///< + (closed)					 
	static const ScoreSymbolID kLaissezVibrer     = 'lvib'; ///< l.v.
	static const ScoreSymbolID kConSordino        = 'sord'; ///< Con Sordino https://en.wikipedia.org/wiki/Mute_(music)
	static const ScoreSymbolID kSenzaSordino      = 'ssor'; ///< revert kConSordino
	static const ScoreSymbolID kArco              = 'arco';
	static const ScoreSymbolID kPizzicato         = 'pizz';
	static const ScoreSymbolID kColLegno          = 'legn'; ///< "mit dem Holz" - reverted bow
	static const ScoreSymbolID kSulPonticello     = 'pont'; ///< "am steg"
    static const ScoreSymbolID kSulTasto          = 'tast'; ///< flautando "nahe am Griffbrett"
	static const ScoreSymbolID kBehindBridge      = 'bhnd'; 
	static const ScoreSymbolID kDownBow           = 'dnbw';
	static const ScoreSymbolID kUpBow             = 'upbw';
	static const ScoreSymbolID kBartokPizzicato   = 'snap'; ///< circle with line crossing the top
	static const ScoreSymbolID kPedalDown         = 'pddn'; 
	static const ScoreSymbolID kPedalUp           = 'pdup';
	static const ScoreSymbolID kHammerOn          = 'hmon';
	static const ScoreSymbolID kPullOff           = 'plof';
	static const ScoreSymbolID kGuitarTap         = 'gtap';
};

} // namespace Presonus

#include "pluginterfaces/base/falignpop.h"

#endif // _ipslsoundvariation_h
