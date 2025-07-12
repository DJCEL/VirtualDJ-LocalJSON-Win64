![logo](https://github.com/djcel/VirtualDJ-localJSON-Win64/blob/main/website.JPG?raw=true "")
# VirtualDJ-LocalJSON-Win64
A VirtualDJ Online Source plugin for Windows 64-bit

It uses the [JsonCpp](https://github.com/open-source-parsers/jsoncpp) package. We advise you to use the static library. You can download and install JsonCpp using the [vcpkg](https://github.com/Microsoft/vcpkg/) dependency manager:

```powershell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install jsoncpp:x64-windows-static
