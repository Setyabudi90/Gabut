#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <regex>
#include <cstdlib>
#include <filesystem>
#include <uuid/uuid.h>
#include <curl/curl.h>

namespace fs = std::filesystem;

class Downloader {
private:
    std::mutex logMutex;

    static size_t writeData(void* ptr, size_t size, size_t nmemb, FILE* stream) {
        return fwrite(ptr, size, nmemb, stream);
    }

    std::string generateUUID() {
        uuid_t uuid;
        uuid_generate_random(uuid);
        char uuidStr[37];
        uuid_unparse(uuid, uuidStr);
        return std::string(uuidStr);
    }

    std::string ensureDir(const std::string& dirPath) {
        fs::path path(dirPath);
        if (!fs::exists(path)) {
            fs::create_directories(path);
            log("Buat folder: " + path.string(), "INFO");
        }
        return path.string() + "/";
    }

    void log(const std::string& msg, const std::string& prefix = "INFO") {
    std::lock_guard<std::mutex> lock(logMutex);

    if (prefix == "OK")
        std::cout << "\033[1;32m[✔] " << msg << "\033[0m\n";
    else if (prefix == "ERR")
        std::cerr << "\033[1;31m[✘] " << msg << "\033[0m\n"; 
    else if (prefix == "YT")
        std::cout << "\033[1;35m[YT] " << msg << "\033[0m\n";
    else if (prefix == "PROGRESS")
        std::cout << "\033[1;34m[⇣] " << msg << "\033[0m\n";
    else if (prefix == "WARN")
        std::cout << "\033[1;33m[!] " << msg << "\033[0m\n"; 
    else if (prefix == "SYSTEM")
        std::cout << "\033[1;36m[*] " << msg << "\033[0m\n"; 
    else
        std::cout << "\033[1;34m[*] " << msg << "\033[0m\n"; 
}

public:
    void showHelp(const std::string& name) {
        std::cout << "\033[1;36mUsage:\033[0m\n"
                  << "  " << name << " --video <url> [res] [path]\n"
                  << "  " << name << " --audio <url> [path]\n"
                  << "  " << name << " --image <url> [path]\n"
                  << "  " << name << " --help\n";
    }

    void downloadVideo(const std::string& url, const std::string& quality, const std::string& savePath) {
        std::string res = quality.empty() ? "720" : quality;
        std::string path = ensureDir(savePath.empty() ? "downloaded" : savePath);
        log("Savepath confirmed " + path, "System");
        std::string filename = path + generateUUID() + ".mp4";

        log("Download video " + url, "YT");
        std::string cmd = 
    "yt-dlp -f \"bv*[height<=" + res + "]+ba/b[height<=" + res + "]\" "
    "--merge-output-format mp4 "
    "-o \"" + filename + "\" \"" + url + "\"";
        FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        log("Failed to open yt-dlp process", "ERR");
        return;
    }

    char buffer[512];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        if (line.find("[download]") != std::string::npos ||
            line.find("Destination") != std::string::npos ||
            line.find("Merging") != std::string::npos) {
            log(line, "WARN");
        }
    }

    int exitCode = pclose(pipe);
    std::cout << exitCode;
    if (exitCode == 0) log("Video disimpan ke " + filename, "OK");
    else log("Gagal download video", "ERR");
    }
    
    void showWelcome() {
    std::cout << "\033[1;35m"; 
    std::cout << R"(
 _   _                          _____             _             
| \ | | ___  _ __   ___ _ __  | ____|_ __   __ _| |_ ___  _ __ 
|  \| |/ _ \| '_ \ / _ \ '__| |  _| | '_ \ / _` | __/ _ \| '__|
| |\  | (_) | | | |  __/ |    | |___| | | | (_| | || (_) | |   
|_| \_|\___/|_| |_|\___|_|    |_____|_| |_|\__,_|\__\___/|_|   
    )" << '\n';
    std::cout << "\033[1;36m";
    std::cout << "        Snatching media from the grid — fast, clean, cyber.\n";
    std::cout << "        Created by Inggrit Setya Budi | Gen Z Energy Activated ⚡\n\n";
    std::cout << "\033[0m"; 
}

    void downloadAudio(const std::string& url, const std::string& savePath) {
        std::string path = ensureDir(savePath.empty() ? "downloaded" : savePath);
        std::string filename = path + generateUUID() + ".mp3";
        log("Savepath confirmed " + path, "System");
        log("Download audio " + url, "YT");
        std::string cmd = "yt-dlp -x --audio-format mp3 -o \"" + filename + "\" \"" + url + "\"";

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        log("Failed to open yt_dlp process", "ERR");
        return;
    }

    char buffer[512];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        if (line.find("[download]") != std::string::npos ||
            line.find("Destination") != std::string::npos ||
            line.find("Extracting") != std::string::npos) {
            log(line, "WARN");
        }
    }

    int exitCode = pclose(pipe);
    if (exitCode == 0) log("Audio disimpan ke " + filename, "OK");
    else log("Gagal download audio", "ERR");
    }

    void downloadImage(const std::string& url, const std::string& savePath) {
        std::string path = ensureDir(savePath.empty() ? "downloaded" : savePath);
        std::string filename = path + generateUUID() + ".jpg";

        log("Download gambar " + url);
        CURL* curl = curl_easy_init();
        if (!curl) {
            log("Init CURL gagal", "ERR");
            return;
        }

        FILE* fp = fopen(filename.c_str(), "wb");
        if (!fp) {
            log("Gagal buka file: " + filename, "ERR");
            curl_easy_cleanup(curl);
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        CURLcode res = curl_easy_perform(curl);
        fclose(fp);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) log("Gambar disimpan ke " + filename, "OK");
        else log("Gagal download gambar: " + std::string(curl_easy_strerror(res)), "ERR");
    }

    void run(int argc, char* argv[]) {
      showWelcome();
    if (argc < 3 || std::string(argv[1]) == "--help") {
        showHelp(argv[0]);
        return;
    }

    std::string mode = argv[1];
    std::string url = argv[2];

    if (mode == "--video") {
    if (argc < 5) {
        log("All params are required, please see --help", "ERR");
        return;
    }
       std::string quality = (argc > 3) ? argv[3] : "";
        std::string path = (argc > 4) ? argv[4] : argv[3];
        downloadVideo(url, quality, path);
    } else if (mode == "--audio") {
        std::string path = (argc > 3) ? argv[3] : "";
        downloadAudio(url, path);

    } else if (mode == "--image") {
        std::string path = (argc > 3) ? argv[3] : "";
        downloadImage(url, path);

    } else {
        log("Mode nggak valid. Gunakan --help buat liat cara pakai", "ERR");
    }
}
};

int main(int argc, char* argv[]) {
    Downloader d;
    d.run(argc, argv);
    return 0;
}