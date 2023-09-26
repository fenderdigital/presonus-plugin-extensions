# PreSonus Plug-In Extensions

Studio One supports audio plug-ins in VST2, VST3 and Audio Unit format. These technologies are maintained by their respective owners and the appropriate SDKs are available separately. In addition to the basic plug-in API implementation Studio One supports host-specific extensions to the original SDKs.

- Support for Sound Variations (Studio One 5.2 and later)
- Support for Instrument Extensions (Studio One 5.2 and later)
- Query key switches from VST2 instrument plug-ins (Studio One 4.5 and later)
- Notifications for cloned plug-in instances in low-latency processing mode (Studio One 3.5.2 and later)
- Remote control channel volume, mute, solo, etc. from plug-in (Studio One 3.2.1 and later)
- Gain reduction reporting to the host application (Studio One 3 and later)
- High DPI scaling notifications for plug-in GUIs on Windows (Studio One 3 and later)
- Access context information like channel names, colors, etc. (Studio One 2.6 and later)
- Per-parameter commands for audio plug-ins related to automation and mapping to control surfaces
- Support for embedding plug-in views into the single-window environment of Studio One
- Support for sizable plug-in editors in VST2 using a similar mimic to VST3
- Support for direct plug-in view renderering to render targets (Studio One 6.5 and later)
- Support for native Wayland plug-in views (currently being implemented)

Feel free to integrate these enhancements in your plug-in or adapt them for your host.

[PreSonus Plug-in Extensions Documentation](https://github.com/fenderdigital/presonus-plugin-extensions/releases/latest/download/PluginExtension.pdf)

[Technical Note on System DPI Scaling for Plug-ins in Studio One](https://presonus.software/developer/technical-note-system-dpi-scaling.pdf)

> [!IMPORTANT]
> DISCLAIMER: PreSonus Plug-In Extensions are host-specific extensions of existing proprietary technologies, provided to the community on an AS IS basis. They are not part of any official 3rd-party SDK and PreSonus is not affiliated with the owner of the underlying technology in any way.
