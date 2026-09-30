
#include <TargetConditionals.h>
#if TARGET_OS_IOS == 1
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

#define IPLUG_AUVIEWCONTROLLER IPlugAUViewController_vSynth108
#define IPLUG_AUAUDIOUNIT IPlugAUAudioUnit_vSynth108
#import <Synth108AU/IPlugAUViewController.h>
#import <Synth108AU/IPlugAUAudioUnit.h>

//! Project version number for Synth108AU.
FOUNDATION_EXPORT double Synth108AUVersionNumber;

//! Project version string for Synth108AU.
FOUNDATION_EXPORT const unsigned char Synth108AUVersionString[];

@class IPlugAUViewController_vSynth108;
