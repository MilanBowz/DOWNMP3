## Installation backend

### Debian/Ubuntu
sudo apt update
sudo apt install cmake gcc ffmpeg

### Arch Linux
sudo pacman -S yt-dlp ffmpeg
#make sure partition has execute rights

## Build wit CMake

rm -rf build && mkdir build && cd build && cmake ..;
make && cd ..;

## Usage Examples:

bash
# Basic download
./downloader "https://www.youtube.com/watch?v=dQw4w9WgXcQ"

# Download with custom name
./downloader "https://www.youtube.com/watch?v=dQw4w9WgXcQ" my_song