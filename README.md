## Installation backend

### Debian/Ubuntu
sudo apt update
sudo apt install cmake gcc ffmpeg

### Arch Linux
sudo pacman -S cmake gcc yt-dlp ffmpeg

> download https://github.com/yt-dlp/yt-dlp  for your OS

> make sure partition has execute rights

> update CMakeLists.txt

## Build wit CMake

rm -rf build && mkdir build && cd build && cmake ..;
make && cd ..;

## Usage Examples:

bash

# Basic download

./downloader 

> Use the GUI: compatible with (2026): Youtube, khinsider,PeerTube,dailymotion.com,Tiktok,...