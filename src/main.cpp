#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

class VideoRecorder {
private:
    cv::VideoCapture cap;
    cv::VideoWriter writer;
    std::vector<int64_t> timestamps;
    std::string output_path;
    std::string json_path;
    bool is_recording;
    int frame_count;
    
    // Get current timestamp in nanoseconds since Unix epoch
    int64_t getCurrentTimestampNs() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
    
    // Extract directory and filename without extension from full path
    std::string getJsonPath(const std::string& mp4_path) {
        std::filesystem::path path(mp4_path);
        std::filesystem::path directory = path.parent_path();
        std::string filename = path.stem().string();
        return (directory / (filename + ".json")).string();
    }
    
public:
    VideoRecorder() : is_recording(false), frame_count(0) {}
    
    bool initialize(const std::string& output_file, int camera_id = 0) {
        output_path = output_file;
        json_path = getJsonPath(output_file);
        
        // Open camera
        cap.open(camera_id);
        if (!cap.isOpened()) {
            std::cerr << "Error: Cannot open camera " << camera_id << std::endl;
            return false;
        }
        
        // Get camera properties
        int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
        int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
        double fps = cap.get(cv::CAP_PROP_FPS);
        
        // Set a default FPS if camera doesn't provide it
        if (fps <= 0) {
            fps = 30.0;
        }
        
        std::cout << "Camera initialized:" << std::endl;
        std::cout << "  Resolution: " << frame_width << "x" << frame_height << std::endl;
        std::cout << "  FPS: " << fps << std::endl;
        
        // Create output directory if it doesn't exist
        std::filesystem::path output_dir = std::filesystem::path(output_path).parent_path();
        if (!output_dir.empty()) {
            std::filesystem::create_directories(output_dir);
        }
        
        // Initialize video writer
        // Using MP4V codec for better compatibility
        int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
        writer.open(output_path, fourcc, fps, cv::Size(frame_width, frame_height), true);
        
        if (!writer.isOpened()) {
            std::cerr << "Error: Cannot open video writer for " << output_path << std::endl;
            return false;
        }
        
        std::cout << "Video writer initialized for: " << output_path << std::endl;
        std::cout << "Timestamps will be saved to: " << json_path << std::endl;
        
        return true;
    }
    
    void startRecording() {
        if (!cap.isOpened() || !writer.isOpened()) {
            std::cerr << "Error: Camera or video writer not initialized" << std::endl;
            return;
        }
        
        is_recording = true;
        timestamps.clear();
        frame_count = 0;
        
        std::cout << "\nStarting recording..." << std::endl;
        std::cout << "Press 'q' to stop recording" << std::endl;
        
        cv::Mat frame;
        while (is_recording) {
            // Capture frame
            cap >> frame;
            if (frame.empty()) {
                std::cerr << "Warning: Empty frame captured" << std::endl;
                continue;
            }
            
            // Get timestamp before writing
            int64_t timestamp = getCurrentTimestampNs();
            
            // Write frame to video
            writer.write(frame);
            
            // Store timestamp
            timestamps.push_back(timestamp);
            frame_count++;
            
            // Display frame (optional)
            cv::imshow("Recording - Press 'q' to stop", frame);
            
            // Check for user input
            char key = cv::waitKey(1) & 0xFF;
            if (key == 'q' || key == 'Q') {
                break;
            }
            
            // Print progress every 30 frames
            if (frame_count % 30 == 0) {
                std::cout << "Recorded " << frame_count << " frames" << std::endl;
            }
        }
        
        stopRecording();
    }
    
    void stopRecording() {
        is_recording = false;
        
        // Release resources
        if (writer.isOpened()) {
            writer.release();
        }
        cv::destroyAllWindows();
        
        // Save timestamps to JSON file
        saveTimestampsToJson();
        
        std::cout << "\nRecording finished!" << std::endl;
        std::cout << "Total frames recorded: " << frame_count << std::endl;
        std::cout << "Video saved to: " << output_path << std::endl;
        std::cout << "Timestamps saved to: " << json_path << std::endl;
    }
    
    void saveTimestampsToJson() {
        std::ofstream json_file(json_path);
        if (!json_file.is_open()) {
            std::cerr << "Error: Cannot create JSON file " << json_path << std::endl;
            return;
        }
        
        json_file << "[\n";
        
        for (size_t i = 0; i < timestamps.size(); ++i) {
            json_file << "  { \"frame\": " << i << ", \"timestamp_ns\": " << timestamps[i] << " }";
            if (i < timestamps.size() - 1) {
                json_file << ",";
            }
            json_file << "\n";
        }
        
        json_file << "]\n";
        
        json_file.close();
    }
    
    ~VideoRecorder() {
        if (cap.isOpened()) {
            cap.release();
        }
        if (writer.isOpened()) {
            writer.release();
        }
        cv::destroyAllWindows();
    }
};

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <output_file.mp4> [camera_id]" << std::endl;
    std::cout << "  output_file.mp4: Path where the MP4 video will be saved" << std::endl;
    std::cout << "  camera_id: Camera index (default: 0)" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  " << program_name << " recording.mp4" << std::endl;
    std::cout << "  " << program_name << " /path/to/my_video.mp4 1" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return -1;
    }
    
    std::string output_file = argv[1];
    int camera_id = 0;
    
    // Check if output file has .mp4 extension
    if (output_file.length() < 4 || output_file.substr(output_file.length() - 4) != ".mp4") {
        std::cerr << "Error: Output file must have .mp4 extension" << std::endl;
        return -1;
    }
    
    // Parse camera ID if provided
    if (argc > 2) {
        try {
            camera_id = std::stoi(argv[2]);
        } catch (const std::exception&) {
            std::cerr << "Error: Invalid camera ID. Using default camera (0)" << std::endl;
            camera_id = 0;
        }
    }
    
    std::cout << "RGB Recorder with Epoch Timestamps" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "Output file: " << output_file << std::endl;
    std::cout << "Camera ID: " << camera_id << std::endl;
    
    VideoRecorder recorder;
    
    if (!recorder.initialize(output_file, camera_id)) {
        std::cerr << "Failed to initialize video recorder" << std::endl;
        return -1;
    }
    
    recorder.startRecording();
    
    return 0;
}
