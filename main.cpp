#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <cstring>
#include <sstream>
#include "include/webview.h"
#include "downloader.cpp"

// Global variables
webview_t g_w = nullptr;
YouTubeDownloader* g_downloader = nullptr;
std::atomic<bool> g_isDownloading{false};

// Update progress
void updateProgress(int percent) {
    std::string js = "if(window.updateProgress) window.updateProgress(" + std::to_string(percent) + ");";
    webview_eval(g_w, js.c_str());
}

// Update status
void updateStatus(const std::string& msg) {
    // Escape single quotes and backslashes
    std::string escaped = msg;
    size_t pos = 0;
    while ((pos = escaped.find("\\", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\\\");
        pos += 2;
    }
    pos = 0;
    while ((pos = escaped.find("'", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\'");
        pos += 2;
    }
    std::string js = "if(window.updateStatus) window.updateStatus('" + escaped + "');";
    webview_eval(g_w, js.c_str());
}

// Enable button
void enableButton() {
    std::string js = "if(window.enableButton) window.enableButton();";
    webview_eval(g_w, js.c_str());
}

// Download thread
void downloadThread(const std::string& url) {
    g_isDownloading = true;
    updateStatus("Downloading: " + url);
    updateProgress(30);
    
    updateStatus("Converting to MP3...");
    updateProgress(60);
    
    bool success = g_downloader->downloadAsMp3(url, "");
    
    if (success) {
        updateProgress(100);
        updateStatus("✅ Download complete! Check downloads folder.");
    } else {
        updateProgress(0);
        updateStatus("❌ Download failed! Check if yt-dlp is installed.");
    }
    
    g_isDownloading = false;
    enableButton();
}

// Bind callback
void onDownload(const char* id, const char* req, void*) {
    std::cout << "onDownload called with: " << req << std::endl;
    
    if (g_isDownloading) {
        updateStatus("Download already in progress!");
        return;
    }
    
    std::string request(req);
    std::string url;
    
    // The request format is: ["{\"url\":\"https://...\"}"]
    // We need to extract the URL between the escaped quotes
    
    // First, find the pattern \"url\":\"
    size_t urlPattern = request.find("\\\"url\\\":\\\"");
    if (urlPattern != std::string::npos) {
        size_t urlStart = urlPattern + 10; // Length of \"url\":\"
        size_t urlEnd = request.find("\\\"", urlStart);
        if (urlEnd != std::string::npos) {
            url = request.substr(urlStart, urlEnd - urlStart);
        }
    }
    
    // If that didn't work, try simpler approach
    if (url.empty()) {
        // Look for https:// or http://
        size_t httpPos = request.find("https://");
        if (httpPos == std::string::npos) {
            httpPos = request.find("http://");
        }
        if (httpPos != std::string::npos) {
            size_t urlEnd = request.find("\\\"", httpPos);
            if (urlEnd == std::string::npos) {
                urlEnd = request.find("\"", httpPos);
            }
            if (urlEnd == std::string::npos) {
                urlEnd = request.length();
            }
            url = request.substr(httpPos, urlEnd - httpPos);
        }
    }
    
    std::cout << "Extracted URL: '" << url << "'" << std::endl;
    
    if (url.empty()) {
        updateStatus("Failed to extract URL! Please try again.");
        enableButton();
        return;
    }
    
    // Clean up URL - remove any trailing quotes or backslashes
    while (!url.empty() && (url.back() == '"' || url.back() == '\\')) {
        url.pop_back();
    }
    
    std::cout << "Cleaned URL: '" << url << "'" << std::endl;
    
    updateProgress(0);
    updateStatus("Starting download...");
    
    std::thread(downloadThread, url).detach();
}

int main() {
    std::cout << "Initializing YouTube MP3 Downloader..." << std::endl;
    
    g_downloader = new YouTubeDownloader();
    g_w = webview_create(true, nullptr);
    
    if (!g_w) {
        std::cerr << "Failed to create webview!" << std::endl;
        return 1;
    }
    
    webview_set_title(g_w, "YouTube MP3 Downloader");
    webview_set_size(g_w, 600, 500, WEBVIEW_HINT_NONE);
    webview_bind(g_w, "download", onDownload, nullptr);
    
    // Get the directory where the executable is
    char exePath[1024];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath)-1);
    std::string exeDir;
    if (len != -1) {
        exePath[len] = '\0';
        std::string path(exePath);
        exeDir = path.substr(0, path.find_last_of('/'));
    } else {
        exeDir = ".";
    }
    
    std::string htmlPath = "file://" + exeDir + "/index.html";
    std::cout << "Loading: " << htmlPath << std::endl;
    webview_navigate(g_w, htmlPath.c_str());
    
    webview_run(g_w);
    webview_destroy(g_w);
    
    delete g_downloader;
    std::cout << "Application closed." << std::endl;
    return 0;
}