//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
#pragma once

/////////////////////////////////////////////////////////////////////
// Preprocessor settings

// Include a manifest adding modern windows visual styles. (optional)
#define MANIFEST_VISUALSTYLES
// Use original language (English U.S.) for "Mode Selection" dialog resource. (optional)
#define MODEDIALOG_ORIGLANG


/////////////////////////////////////////////////////////////////////
// General use
//
#define IDM_STATIC						-1
#define IDC_STATIC						-1


/////////////////////////////////////////////////////////////////////
// Used by openlrr.rc
//
#define IDI_OPENLRR						104		// 2057 : ICON_GROUP : (OpenLRR main icon)
#define IDR_MAINFRAME					128
#define IDI_SMALL						129
#define IDS_APP_TITLE                   130
#define IDD_ABOUTBOX					131
#define IDC_DEBUGSYSMENU				132

#define IDMI_FILE						0
#define IDMI_OPTIONS					1
#define IDMI_DEBUG						2
#define IDMI_CHEATS						3
#define IDMI_HELP						4

// &File
#define IDM_TOGGLEMENU					601
#define IDM_TOGGLECONSOLE				602
// --------------
#define IDM_EXIT						603


// &Help
#define IDM_ABOUT						651


// &Options
#define IDM_MUSICON						501
#define IDM_SOUNDON						502
#define IDM_HELPWINDOW					503
#define IDM_AUTOGAMESPEED				504
#define IDM_DUALMOUSE					505
#define IDM_LOSEFOCUSANDPAUSE		    506
// --------------
// &Options > Building Arrow (Radio Group)
#define IDM_SELECTPLACEARROW_NEVER		587
#define IDM_SELECTPLACEARROW_SOLIDONLY	588
#define IDM_SELECTPLACEARROW_ALWAYS		589

#define IDM_SHOWOBJINFO					511
#define IDM_RENDERPANELS				512
#define IDM_TOOLTIPSOUND				513
// --------------
#define IDM_RELOAD_KEYBINDS				580
// &Options > Logging
#define IDM_LOGGING_DEBUG				581
#define IDM_LOGGING_TRACE				582
#define IDM_LOGGING_INFO				583
#define IDM_LOGGING_WARNING				584
#define IDM_LOGGING_FATAL				585
// &Options > Scale > Window (Radio Group)
#define IDM_WINDOWSCALE_X1				521
#define IDM_WINDOWSCALE_X2				522
#define IDM_WINDOWSCALE_X3				523
#define IDM_WINDOWSCALE_X4				524
// &Options > Scale > Radar Map (Radio Group)
#define IDM_RADARMAPSCALE_X1			526
#define IDM_RADARMAPSCALE_X2			527
#define IDM_RADARMAPSCALE_X3			528
#define IDM_RADARMAPSCALE_X4			529
// &Options > Cursor Visibility (Radio Group)
#define IDM_CURSOR_NEVER				531
#define IDM_CURSOR_TITLEBAR				532
#define IDM_CURSOR_ALWAYS				533
// &Options > Cursor Clipping (Radio Group)
#define IDM_CURSORCLIPPING_OFF			535
#define IDM_CURSORCLIPPING_MENU			536
#define IDM_CURSORCLIPPING_GAMEAREA		537
// &Options > Icon (Radio Group)
#define IDM_ICON_NONE					541
#define IDM_ICON_NATIVE					542
#define IDM_ICON_OPENLRR				543
#define IDM_ICON_GOLD					544
#define IDM_ICON_TEAL					545
#define IDM_ICON_TEALRR					546
// --------------
// &Options > Graphics
#define IDM_LIGHTEFFECTS				551
#define IDM_DETAILON					552
#define IDM_DYNAMICPM					553
#define IDM_TOPDOWNFOG					554
// &Options > Graphics > Quality (Radio Group)
#define IDM_QUALITY_WIREFRAME			561
#define IDM_QUALITY_UNLITFLAT			562
#define IDM_QUALITY_FLAT				563
#define IDM_QUALITY_GOURAUD				564
#define IDM_QUALITY_PHONG				565
// ------
#define IDM_GRAPHICS_BLEND				571
#define IDM_GRAPHICS_DITHER				572
#define IDM_GRAPHICS_FILTER				573
#define IDM_GRAPHICS_LINEARMIPMAP		574
#define IDM_GRAPHICS_MIPMAP				575
#define IDM_GRAPHICS_SORT				576
#define IDM_GRAPHICS_ALPHAMODULATION	577


// &Debug
//#define IDM_DEBUGENABLED				301
#define IDM_CHEATSENABLED				302
#define IDM_PROGRAMMER					303
#define IDM_DEBUGMODE					304
#define IDM_DEBUGCOMPLETE				305
#define IDM_LEVELSOPEN					306
#define IDM_TESTERCALL					307
// --------------
#define IDM_ALLOWDEBUGKEYS				311
#define IDM_ALLOWEDITMODE				312
#define IDM_SHOWDEBUGTOOLTIPS			313
// --------------
// &Debug > Routing Paths
#define IDM_ROUTING_ENABLED				350
#define IDM_ROUTING_COMPLETEDPATHS		351
#define IDM_ROUTING_CURVEPATHS		    352
// &Debug > Routing Paths > Auto Add (Radio Group)
#define IDM_ROUTING_AUTO_NONE			353
#define IDM_ROUTING_AUTO_TRACKED		354
#define IDM_ROUTING_AUTO_ALLFRIENDLY	355
#define IDM_ROUTING_AUTO_ALL			356

#define IDM_ROUTING_ADDSELECTED			357
#define IDM_ROUTING_ADDALLFRIENDLY		358
#define IDM_ROUTING_ADDALL				359
#define IDM_ROUTING_REMOVESELECTED		360
#define IDM_ROUTING_REMOVEALL			361
// --------------
// &Debug > Rendering
#define IDM_RENDERING_DRAW				365
#define IDM_RENDERING_IMAGES			366
#define IDM_RENDERING_FONTS				367
// --------------
// &Debug > Experimental
#define IDM_BLOCKFADE					321
#define IDM_DDRAWCLEAR					322
#define IDM_WOBBLYWORLD					323
// &Debug > Info
#define IDM_FPSMONITOR					331
#define IDM_MEMORYMONITOR				332
// --------------
#define IDM_DUMPMODE					341
#define IDM_FREEZE						342
#define IDM_ADVANCE_1FRAME				343
#define IDM_ADVANCE_1SECOND				344


// &Cheats
#define IDM_NONERPS						401
#define IDM_UNLOCKCAMERA				402
#define IDM_UNLOCKBUILD					403
#define IDM_NOBUILDCOSTS				404
#define IDM_NOCONSTRUCTIONBARRIERS		405
#define IDM_BUILDWITHOUTPATHS			406
#define IDM_BUILDANYROUGHNESS			407
#define IDM_FPNOCLIP					408
#define IDM_FPCONTROLS					409
// --------------
#define IDM_NOPOWERCONSUMPTION			411
#define IDM_NOOXYGENCONSUMPTION			412
#define IDM_SUPERTOOLSTORE				413
#define IDM_QUICKREINFORCE				414
#define IDM_PEACEFUL					415
// --------------
#define IDM_SURVEYLEVEL					419
// --------------
// &Cheats > Config
#define IDM_NOROCKFALL					420
#define IDM_ALLOWRENAME					421
#define IDM_DISABLEENDTELEPORT			422
#define IDM_GENERATESPIDERS				423
#define IDM_NOAUTOEAT					424
#define IDM_NOFALLINS					425
#define IDM_NOMULTISELECT				426
#define IDM_SAFECAVERNS					427
#define IDM_SEETHROUGHWALLS				428

// &Cheats > Resources
#define IDM_ADDCRYSTALS					450
#define IDM_ADDORE						451
#define IDM_ADDSTUDS					452
#define IDM_ADDOXYGEN					453
// --------------
#define IDM_SUBCRYSTALS					460
#define IDM_SUBORE						461
#define IDM_SUBSTUDS					462
#define IDM_SUBOXYGEN					463

// &Cheats > Units
#define IDM_SELECTEDLEVELUP				470
#define IDM_SELECTEDHEAL				471
#define IDM_SELECTEDTRAIN				472
#define IDM_SELECTEDEQUIP				473
#define IDM_SELECTEDMAXIMIZE			474
// --------------
#define IDM_ALLLEVELUP					480
#define IDM_ALLHEAL						481
#define IDM_ALLTRAIN					482
#define IDM_ALLEQUIP					483
#define IDM_ALLMAXIMIZE					484
// --------------
#define IDM_KILLCREATURES				491



#define ID_Menu							32771
#define ID_SUBMENUOPEN_SUBMENUITEM		32772
#define ID_SUBMENUOPEN_SUBMENUITEM32773	32773


/////////////////////////////////////////////////////////////////////
// Used by legorr.rc
//
#define IDI_LEGORR						113		// 2057 : ICON_GROUP : (LegoRR.exe main icon)

// Exclusive to beta lego*.rc builds
#define IDD_WARNDIALOG					102


/////////////////////////////////////////////////////////////////////
// Used by init.rc
//
#define IDD_MODEDIALOG					101		// 1033 : DIALOG : "Mode Selection"
#define IDC_DRIVER						1000	// IDD_MODEDIALOG : LISTBOX : "Driver"
#define IDC_DEVICE						1002	// IDD_MODEDIALOG : LISTBOX : "Device"
#define IDC_MODE						1003	// IDD_MODEDIALOG : LISTBOX : "Screen Mode" | "Window Size"
#define IDC_FULLSCREEN					1004	// IDD_MODEDIALOG : AUTORADIOBUTTON : "Full Screen"
#define IDC_WINDOW						1005	// IDD_MODEDIALOG : AUTORADIOBUTTON : "Window"
#define IDC_DEVICELISTTEXT				IDC_STATIC	// IDD_MODEDIALOG : LTEXT : "Device"
#define IDC_DEVICELISTTEXT				IDC_STATIC	// IDD_MODEDIALOG : LTEXT : "Driver"
#define IDC_MODELISTTEXT				1006	// IDD_MODEDIALOG : RTEXT : "Screen Mode" | "Window Size"


/////////////////////////////////////////////////////////////////////
// Used by clgen.rc
//
// Subtract 100 from all 200's IDs to get real CLGen values.
// (prevents conflicts with init.rc)
#define IDD_CLGEN_PRESETDIALOG			201		// 2057 : DIALOG : "Video Card selector"
#define IDI_CLGEN						203		// 2057 : ICON_GROUP : (CLGen.exe main icon)
#define IDC_CLGEN_SELECT				1101	// IDD_CLGEN_PRESETDIALOG : COMBOBOX : "Select Video Card type:"
#define IDC_CLGEN_SELECTTEXT			1106	// IDD_CLGEN_PRESETDIALOG : LTEXT : "Select:"


/////////////////////////////////////////////////////////////////////
// Other
//
#define IDI_CDROM						204		// icon used by the CDROM


/////////////////////////////////////////////////////////////////////
// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NO_MFC                     1
#define _APS_NEXT_RESOURCE_VALUE        308
#define _APS_NEXT_COMMAND_VALUE         32774
#define _APS_NEXT_CONTROL_VALUE         1007
#define _APS_NEXT_SYMED_VALUE           308
#endif
#endif
