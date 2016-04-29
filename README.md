# ofxRayComposer
openFrameworks addon to control show-lasers using the USB interface by RayComposer.


Introduction
------------
C++ openFrameworks addon for interfacing with the <a href="http://www.raycomposer.de/en/ilda-dac-interfaces/raycomposer-usb/">ILDA DAC interface by RayComposer</a>. Similar to the [ofxEatherdream](https://github.com/memo/ofxEtherdream) the addon is designed to be used with [ofxIlda](https://github.com/memo/ofxIlda).  
Written by [Jonas Fehr](www.jonasfehr.ch) and is based on the API by Robin Adams ([RayComposer](http://www.raycomposer.de/)).

- <a href = "https://github.com/RayComposer/raycomposer-api">RayComposer API</a>
- <a href = "http://www.raycomposer.de/apidoc/index.html">API Documentation</a>

Installation
------------
1. Copy to your openFrameworks/addons folder or use <code>git clone</code>

2. copy following lines into the <code>Run Script</code> under <code>Build Phases</code> in Xcode. It will copy the files needed at the right place and redirect a path.  
`# Copy libraries for the RayComposer USB dongle.`  
`rsync -aved ../../../addons/ofxRayComposer/api-lib/osx_x86_64/librcdev.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/";  `
`rsync -aved ../../../addons/ofxRayComposer/api-lib/osx_x86_64/librcdev.1.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/";  `
`rsync -aved ../../../addons/ofxRayComposer/api-lib/osx_x86_64/librcdev.1.4.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/";  `
`rsync -aved ../../../addons/ofxRayComposer/api-lib/osx_x86_64/librcdev.1.4.0.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/";  `
`rsync -aved ../../../addons/ofxRayComposer/api-lib/osx_x86_64/lib/libusb-1.0.0.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/lib/";  `
`# redirect` 
`install_name_tool -change @executable_path/lib/librcdev.1.dylib @executable_path/../Frameworks/librcdev.1.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/$PRODUCT_NAME";`
3. To avoid linker problems when compile your code, drag the file `librcdev.1.4.0.dylib` from `../ofxRayComposer/api-lib/osx_x86_64/` into your project folder (fx. src)

Example
------------
the example is an adaptation of the example provided with ofxIlda using this DAC instead of Eatherdream.

Dependencies
------------
the addon is designed to be used with:

- ofxIlda ([https://github.com/memo/ofxIlda](https://github.com/memo/ofxIlda))

Compatibility
------------
tested on  
- openFrameworks 0.9.3  
- MacbookPro mid 2012 (2,6 GHz Intel Core i7 | 8 GB RAM)  
- OSX El Capitan 10.11.4  
- Xcode 7.3
  
Known issues
------------
none

Version history
------------

### v0.1
- initial version

Licence
-------
The c++ code in this repository is available under the [MIT License](https://en.wikipedia.org/wiki/MIT_License).

The use of the API is free of charge as long as it's used to drive the RayComposer hardware. (C) 2015 RayComposer - Robin Adams

