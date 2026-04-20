## Installation backend

### Debian/Ubuntu
sudo apt update
sudo apt install cmake gcc ffmpeg

### Arch Linux
sudo pacman -S cmake gcc yt-dlp ffmpeg

> download https://github.com/yt-dlp/yt-dlp en https://ffmpeg.org/download.html (package installer OS) for your OS

> make sure partition has execute rights

> update CMakeLists.txt

## Build with CMake (Linux)

rm -rf build && mkdir build && cd build && cmake ..;
make && cd ..;


## Build zith MinGW Makefiles (Windows)

> Download Webview 2 (https://www.nuget.org/packages/Microsoft.Web.WebView2/)

> Download FFmpeg [online](https://ffmpeg.org/download.html#build-windows) 

rm -r -fo build; mkdir build;cd build;cmake .. -G "MinGW Makefiles";
make ; cd ..;


## Usage Example:

./downloader 

> Use the GUI: compatible with (2026): Youtube, khinsider,PeerTube,Dailymotion,Tiktok,...