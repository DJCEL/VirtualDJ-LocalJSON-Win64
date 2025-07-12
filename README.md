![logo](https://github.com/djcel/VirtualDJ-localJSON-Win64/blob/main/website.JPG?raw=true "")
# VirtualDJ-LocalJSON-Win64
A VirtualDJ Online Source plugin for Windows 64-bit

It uses the 'jsoncpp' package downloaded with Vcpkg (For more information about this package, please read https://github.com/open-source-parsers/jsoncpp ). 

Use the static library:

You can download and install JsonCpp using the [vcpkg](https://github.com/Microsoft/vcpkg/) dependency manager:

git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install jsoncpp:x64-windows-static
