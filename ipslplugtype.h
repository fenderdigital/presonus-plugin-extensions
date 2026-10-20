//************************************************************************************************
//
// PreSonus Plug-In Extensions
// Written and placed in the PUBLIC DOMAIN by PreSonus Software Ltd.
//
// Filename    : ipslplugtype.h
// Created by  : PreSonus Software Ltd., 04/2024
// Description : Special Purpose Plug-In Categories
//
//************************************************************************************************
/*
	DISCLAIMER:
	PreSonus Plug-In Extensions are host-specific extensions of existing proprietary technologies,
	provided to the community on an AS IS basis. They are not part of any official 3rd party SDK and
	PreSonus is not affiliated with the owner of the underlying technology in any way.
*/
//************************************************************************************************

#ifndef _ipslplugtype_h
#define _ipslplugtype_h

/** Plug-in Category "Media Content Provider".

A plug-in of this kind is added to the application's media browser if it has a registered plug-in class ID. 

- The VST processor class must be specified in the following way:
  - the Steinberg::PClassInfo2::category must be kMediaContentProviderClass (instead of kVstAudioEffectClass)
  - the Steinberg::PClassInfo2::cid should be a class ID defined by the host application vendor.

- The plug-in is supposed to have one audio output only and no inputs.
  - The output of the plug-in is fed directly into the "Audition" channel (usually the Main output).
  - Note: The host application might not provide a separate gain control for the plug-in.

- There will be one instance per document. This is to make sure that there is always a one-to-one relation between document and plug-in.
  - A plug-in instance will be created automatically when a document is created or loaded. Initially it will be inactive. setActive / setProcessing are not called.    
  - If the user opens the corresponding browser tab, the plug-in will be activated (setActive (true), setProcessing (true)) and the edit view is created.
  - If the user opens another tab or closes the browser, the plug-in will be deactivated (setActive (false), setProcessing (false)). 
	Also the edit view is removed as if you would close the normal instrument editor window.  
  - The plug-in instance has the same lifecycle as a document. Closing a document will destroy the plug-in instance.

- The plug-in GUI is supposed to be sizable to fit into the browser.

- The application stores the latest state of the plug-in in a default preset file which is used when a new document is created. 
  This is optional and is only performed if the plug-in is marked dirty. It is marked dirty when it sends 
  IComponentHandler::performEdit messages or IComponentHandler::restartComponent (kParamValuesChanged).
*/

#ifndef kMediaContentProviderClass
#define kMediaContentProviderClass "Media Content Provider"
#endif

#endif // _ipslplugtype_h