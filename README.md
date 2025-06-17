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
g++ main.cpp -o main_d -lcurl -luuid -pthread -std=c++17
```
> Output binary will be main_d


🚀 Usage

```bash
./main_d --video <url> [resolution] [path]
./main_d --audio <url> [path]
./main_d --image <url> [path]
./main_d --help
```

---

🎥 Download Video

```bash
./main_d --video https://youtu.be/example 720 videos/
```

resolution: 1080, 720, 480 etc. (required)

path (optional): folder to save (default: downloaded/)



---

🎧 Download Audio

```bash
./main_d --audio https://youtu.be/example music/
```

Extracts .mp3 from YouTube

UUID-based filename



---

🖼️ Download Image

```bash
./main_d --image https://example.com/image.jpg pics/
```

Downloads and saves as .jpg

Auto folder creation if needed



---

🌈 Log Types & Colors

Prefix	Color	Description

```bash
[*]	Blue	Info
[✔]	Green	Success
[✘]	Red	Error
[YT]	Magenta	YouTube logs
[SYS]	Cyan	System (folder/files)
```

> Terminal logs designed for clarity and cyberpunk feels ✨




---

🔧 Sample Output

```bash
[SYS] Created folder: downloaded/
[YT] Downloading video https://youtu.be/abc123
[✔] Video saved to downloaded/6aa1-42d0-9dbf.mp4
```

---

---

## 👤 Developer Info

| Field        | Detail                            |
|--------------|------------------------------------|
| 👨‍💻 Author    | **Inggrit Setya Budi**            |
| 🧠 Role       | Developer & Designer of CLI Tool  |
| 🛠️ Language  | C++17                              |
| ⚙️ Binary     | `main_d`                          |
| 🎨 Style      | Gen Z × Cyberpunk Terminal Vibes  |
| 🌐 Platform   | Termux & Linux                    |
| 📦 Project    | CYBERFETCH - Smart Downloader CLI |

> Always building tools to make CLI life smoother.  
> Proudly coded in a terminal, probably late at night 💻🌙
