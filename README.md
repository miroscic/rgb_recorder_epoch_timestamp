# RGB Recorder with Epoch Timestamps

A cross-platform C++ application that records video from a camera using OpenCV and saves frame timestamps in Unix epoch nanoseconds to a JSON file.

## Features

- Records video from camera to MP4 format
- Cross-platform support (Windows and Linux)
- Saves frame timestamps in nanoseconds since Unix epoch
- Outputs timestamps to JSON file alongside the video
- Command-line interface with customizable output path
- Real-time preview during recording

## Prerequisites

### Dependencies
- OpenCV 4.x or higher
- CMake 3.12 or higher
- C++17 compatible compiler

### Windows
- Visual Studio 2019 or newer
- OpenCV (can be installed via vcpkg or manually)

### Linux
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake
sudo apt install libopencv-dev

# CentOS/RHEL/Fedora
sudo yum install gcc-c++ cmake
sudo yum install opencv-devel
```

## Building

1. Clone the repository:
```bash
git clone <repository-url>
cd rgb_recorder_epoch_timestamp
```

2. Build:
```bash
cmake -Bbuild
```

3. Compile:
```bash
cmake --build build --config Release
```

## Usage

```bash
./rgb_recorder <output_file.mp4> [camera_id]
```

### Parameters
- `output_file.mp4`: Path where the MP4 video will be saved (must have .mp4 extension)
- `camera_id`: Optional camera index (default: 0)

### Examples

```bash
# Record to current directory
./rgb_recorder recording.mp4

# Record to specific path
./rgb_recorder /path/to/my_video.mp4

# Use camera 1 instead of default camera 0
./rgb_recorder recording.mp4 1
```

### Windows Examples
```cmd
# Record to current directory
rgb_recorder.exe recording.mp4

# Record to specific path
rgb_recorder.exe C:\Videos\my_video.mp4

# Use camera 1
rgb_recorder.exe recording.mp4 1
```

## Controls

- Press `q` or `Q` to stop recording and save the video

## Output Files

The application creates two files:

1. **Video file**: The recorded MP4 video at the specified path
2. **Timestamp file**: A JSON file with the same name as the video file, containing an array of objects with:
   - `frame`: Frame number (starting from 0)
   - `timestamp_ns`: Timestamp in nanoseconds since Unix epoch

### Example JSON Output
```json
[
  { "frame": 0, "timestamp_ns": 1642694400123456789 },
  { "frame": 1, "timestamp_ns": 1642694400156789123 },
  { "frame": 2, "timestamp_ns": 1642694400190123456 },
  { "frame": 3, "timestamp_ns": 1642694400223456789 },
  ...
]
```

## Timestamp Precision

Timestamps are captured using `std::chrono::system_clock` and represent nanoseconds since Unix epoch (January 1, 1970, 00:00:00 UTC). The actual precision depends on the system's clock resolution.

## Supported Cameras

### Built-in/USB Cameras
- **Laptop built-in webcams** (usually camera ID 0)
- **USB webcams** (Logitech, Microsoft, etc.)
- **USB capture cards** for external video sources
- **Industrial USB cameras** (UVC compatible)

### Camera IDs
- `0`: Default camera (usually built-in webcam)
- `1, 2, 3...`: Additional cameras in order of detection
- Use different IDs to test which camera you want to use

### iPhone as Camera

To use an iPhone as a camera source, you have several options:

#### Option 1: USB Connection (Recommended)
1. **Install iPhone camera software:**
   - **Windows**: Use apps like "EpocCam", "iVCam", or "Camo"
   - **macOS**: Use "Continuity Camera" (built-in) or third-party apps

2. **Connect iPhone via USB cable**

3. **The iPhone should appear as a camera device** (usually camera ID 1 or 2)

#### Option 2: Wireless Connection
1. **Install apps like:**
   - **EpocCam** (free/paid versions available)
   - **iVCam** 
   - **DroidCam** (also works with iPhone)

2. **Connect iPhone and computer to same WiFi network**

3. **iPhone will appear as a virtual camera device**

#### Option 3: RTMP/Network Streaming
For advanced users, you can modify the code to accept RTMP streams from iPhone camera apps.

### Camera Testing
To test which cameras are available:
```bash
# Try different camera IDs
./rgb_recorder test.mp4 0  # Built-in camera
./rgb_recorder test.mp4 1  # External/iPhone camera
./rgb_recorder test.mp4 2  # Additional camera
```

The application will show an error if the camera ID doesn't exist, so you can easily test which cameras are available on your system.

## Troubleshooting

### Camera Not Found
- Ensure your camera is connected and not being used by another application
- Try different camera IDs (0, 1, 2, etc.)
- Check camera permissions on your system

### Build Issues
- Ensure OpenCV is properly installed and can be found by CMake
- Check that your compiler supports C++17
- On Windows, make sure OpenCV is in your PATH or set OpenCV_DIR

### Performance Issues
- Lower resolution cameras will perform better
- Ensure sufficient disk space for video recording
- Close other applications that might be using the camera

## License

This project is open source and available under the [MIT License](LICENSE).