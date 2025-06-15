<h1 align="center">⚡ CYBERFETCH - CLI Downloader for YouTube & Images</h1>

<p align="center">
  <img src="https://img.shields.io/badge/yt--dl-powered-blueviolet?style=flat-square" />
  <img src="https://img.shields.io/badge/termux--friendly-00b894?style=flat-square" />
  <img src="https://img.shields.io/badge/linux--ready-4caf50?style=flat-square" />
  <img src="https://img.shields.io/badge/cyberpunk-CLI-fc5c65?style=flat-square" />
</p>

> 🚀 A terminal-based tool to download videos, audios, and images with aesthetic logs, smooth UX, and custom output handling.

---

## ✨ Features

- 🎥 Download YouTube **videos** at custom resolution
- 🎵 Extract YouTube **audio** directly to MP3
- 🖼️ Download **images** from any valid URL
- 🔧 Smart file naming with **UUID**
- 📂 Auto-create output folders if not exist
- 🌈 Colored terminal logs for readability
- 🎨 Cyberpunk-style UX with clean CLI design

---

## 💻 Platform Support

| Platform | Status       | Notes                                |
|----------|--------------|--------------------------------------|
| ✅ Linux | Full support | Build-ready with g++ & curl          |
| ✅ Termux (Android) | Full support | CLI optimized, tested on latest Termux |

---

## ⚙️ Requirements

Install these dependencies first:

### Termux:

```bash
pkg update
pkg install yt-dlp ffmpeg curl libuuid clang
```

### Linux (Debian/Ubuntu)
```bash
sudo apt update
sudo apt install yt-dlp ffmpeg libcurl4-openssl-dev uuid-dev g++
```

### 🛠️ Build Instructions

Clone and compile:

```bash
g++ downloader.cpp -o main_d -lcurl -luuid -pthread -std=c++17
```
> Output binary will be main_d
