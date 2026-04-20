
#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <array>      // Added for std::array
#include <cstdio>     // Added for FILE, popen, pclose
#include <algorithm>

class YouTubeDownloader {
private:
    std::string ytdlpPath;
    std::string ffmpegPath;
    std::string outputDir;

    std::string execCommand(const std::string& cmd) {
        std::array<char, 128> buffer;
        std::string result;
        
        // Use popen to execute command and read output
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        
        pclose(pipe);
        return result;
    }

public:
    YouTubeDownloader(const std::string& ytdlp = "./yt-dlp", 
                     const std::string& ffmpeg = "ffmpeg",
                     const std::string& output = "./downloads") 
        : ytdlpPath(ytdlp), ffmpegPath(ffmpeg), outputDir(output) {
        // Fix paths for Windows
        #ifdef _WIN32
            // If yt-dlp.exe is in current directory
            if (std::filesystem::exists("./yt-dlp.exe")) {
                ytdlpPath = ".\\yt-dlp.exe";
            }
            // Convert forward slashes to backslashes for Windows
            std::replace(outputDir.begin(), outputDir.end(), '/', '\\');
        #endif
        // Create output directory if it doesn't exist
        try {
            std::filesystem::create_directories(outputDir);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not create directory: " << e.what() << std::endl;
        }
    }

    bool downloadAsMp3(const std::string& url, const std::string& customName = "") {
        try {
            #ifdef _WIN32
                std::string outputTemplate = outputDir + "/%(title)s.%(ext)s";
                std::replace(outputTemplate.begin(), outputTemplate.end(), '/', '\\');
                if (!customName.empty()) {
                    outputTemplate = outputDir + "\\" + customName + ".%(ext)s";
                }
            #else
                std::string outputTemplate = outputDir + "/%(title)s.%(ext)s";
                if (!customName.empty()) {
                    outputTemplate = outputDir + "/" + customName + ".%(ext)s";
                }
            #endif

            // Build yt-dlp command for audio extraction
            std::string command = ytdlpPath + " " + url + 
                                 " -x --audio-format mp3 " +
                                 "--audio-quality 0 " +
                                 "-o \"" + outputTemplate + "\" " +
                                 "--force-overwrites " +
                                 "--no-playlist";

            std::cout << "Downloading: " << url << std::endl;
            std::cout << "Command: " << command << std::endl;
            
            std::string result = execCommand(command);
            
            if (result.find("ERROR") != std::string::npos) {
                std::cerr << "Download failed: " << result << std::endl;
                return false;
            }
            
            std::cout << "Download complete!" << std::endl;
            std::cout << result << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    bool downloadBestQuality(const std::string& url) {
        try {
            std::string outputTemplate = outputDir + "/%(title)s.%(ext)s";
            
            std::string command = ytdlpPath + " " + url + 
                                 " -f bestaudio " +
                                 "--extract-audio " +
                                 "--audio-format mp3 " +
                                 "--audio-quality 0 " +
                                 "-o \"" + outputTemplate + "\" " +
                                 "--no-playlist";

            std::cout << "Downloading best quality audio..." << std::endl;
            std::cout << "Command: " << command << std::endl;
            
            std::string result = execCommand(command);
            
            if (result.find("ERROR") != std::string::npos) {
                std::cerr << "Download failed: " << result << std::endl;
                return false;
            }
            
            std::cout << "Download complete!" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    void listFormats(const std::string& url) {
        try {
            std::string command = ytdlpPath + " -F " + url;
            std::cout << "Available formats:\n" << execCommand(command) << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    
    bool checkDependencies() {
#ifdef _WIN32
        // Windows version - check if files exist
        std::string ytdlpCheck = ytdlpPath+ ".exe";
        
        // Check if yt-dlp.exe exists
        bool ytdlpFound = std::filesystem::exists(ytdlpCheck) || 
                         std::filesystem::exists(ytdlpPath) ||
                         !execCommand("where yt-dlp 2>nul").empty();
        
        if (!ytdlpFound) {
            std::cerr << "Error: Make sure yt-dlp.exe is in the current directory or PATH\n";
            std::cerr << "Download from: https://github.com/yt-dlp/yt-dlp/releases\n";
            return false;
        }
        
        // Check if ffmpeg is installed system-wide (in PATH)
        std::string checkFfmpeg = "where ffmpeg 2>nul";
        std::string ffmpegResult = execCommand(checkFfmpeg);
        
        if (ffmpegResult.empty()) {
            std::cerr << "Error: ffmpeg is not installed or not in PATH!\n";
            std::cerr << "Installation options:\n";
            std::cerr << "  1. Run as Admin: winget install ffmpeg\n";
            std::cerr << "  2. Download from: https://ffmpeg.org/download.html\n";
            std::cerr << "  3. Add to PATH: C:\\ffmpeg\\bin\n";
            return false;
        }
        
        std::cout << "Dependencies found!\n";
        return true;
        
#else
        // Check if yt-dlp is installed
        std::string checkYtdlp = "test -x " + ytdlpPath + " && echo 'Found'";
        std::string ytdlpResult = execCommand(checkYtdlp);
        
        if (ytdlpResult.empty()) {
            std::cerr << "Error: yt-dlp is not installed or not in PATH\n";
            std::cerr << "Install it: pip install yt-dlp\n";
            return false;
        }
        
        // Check if ffmpeg is installed
        std::string checkFfmpeg = "which " + ffmpegPath + " 2>/dev/null || where " + ffmpegPath + " 2>nul";
        std::string ffmpegResult = execCommand(checkFfmpeg);
        
        if (ffmpegResult.empty()) {
            std::cerr << "Error: ffmpeg is not installed or not in PATH\n";
            std::cerr << "Install it: sudo apt install ffmpeg (Linux) or brew install ffmpeg (Mac)\n";
            return false;
        }
        
        std::cout << "Dependencies found:\n";
        std::cout << "yt-dlp: " << ytdlpResult;
        std::cout << "ffmpeg: " << ffmpegResult;
        return true;
#endif
    }
};