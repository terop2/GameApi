#!/bin/bash
rm GameApi-sourcecode-v27.tar.gz
tar czvf GameApi-sourcecode-v27.tar.gz --exclude="assets2" --exclude="android-objs" --exclude="objs" --exclude="chai" --exclude="editor/sanmiguel3_a.ds" --exclude="editor/gameapi-builder_1.0-27" --exclude="display/tmp" --exclude="editor/GameApi-Builder.AppDir" --exclude="editor/gameapi-builder_1.0-25" --exclude="editor/appimage-builder-cache" --exclude="editor/*.ds" --exclude="editor/.AppDir" --exclude="editor/gameapi-builder_1.0-26" --exclude="editor/*.AppImage" --exclude="editor/AppDir" --exclude="editor/tmp" --exclude="*.o" --exclude="*.wasm" --exclude="*.bc" --exclude="*.pkg" --exclude="web_page/tmp2" --exclude="*.wasm.br" --exclude="*.dwp" --exclude="#*#" --exclude="*~" --exclude="*.ogg" *
scp GameApi-sourcecode-v27.tar.gz terop@ssh.meshpage.org:/home/terop/meshpage.org/