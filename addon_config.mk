meta:
	ADDON_NAME = ofxRayComposer
	ADDON_DESCRIPTION = addon for the ILDA-DAC USB dongle by RayComposer to be used with ofxIlda
	ADDON_AUTHOR = Jonas Fehr
	ADDON_TAGS = "Laser" "ILDA" "RayComposer" "ofxIlda"
	ADDON_URL = https://github.com/jonasfehr/ofxRayComposer

	ADDON_INCLUDES += libs
	ADDON_INCLUDES += src

vs:
	#win32
	#ADDON_DLLS_TO_COPY += api-lib/win_x86/rcdev32.dll
	#ADDON_LIBS += api-lib/win_x86/rcdev32.lib
	#x64
	ADDON_DLLS_TO_COPY += api-lib/win_x86_64/rcdev64.dll
	ADDON_LIBS += api-lib/win_x86_64/rcdev64.lib

osx:
	ADDON_FRAMEWORKS = $(PATH_OF_ADDON)/api-lib/osx_x86-64/librcdev.1.6.0.dylib	
