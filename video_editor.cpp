#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
using namespace cv;
using namespace std;

/**
 * Overlay a transparent logo on top-right corner of a frame.
 */
void overlayLogo(Mat &frame, const Mat &logo) {
    if (logo.empty()) return;

    int x = frame.cols - logo.cols - 10; // padding 10px
    int y = 10;

    if (x < 0 || y < 0) return;

    Mat roi = frame(Rect(x, y, logo.cols, logo.rows));

    for (int r = 0; r < logo.rows; r++) {
        for (int c = 0; c < logo.cols; c++) {
            Vec4b pixel = logo.at<Vec4b>(r, c); // RGBA pixel
            if (pixel[3] > 0) { // alpha > 0
                roi.at<Vec3b>(r, c) = Vec3b(pixel[0], pixel[1], pixel[2]);
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 5) {
        cerr << "Usage: ./video_editor <video1> <video2> <logo> <output>" << endl;
        return -1;
    }

    string video1Path = argv[1];
    string video2Path = argv[2];
    string logoPath   = argv[3];
    string outputPath = argv[4];

    // Open videos
    VideoCapture cap1(video1Path);
    VideoCapture cap2(video2Path);

    if (!cap1.isOpened() || !cap2.isOpened()) {
        cerr << "Error: Could not open input videos!" << endl;
        return -1;
    }

    // Read logo
    Mat logo = imread(logoPath, IMREAD_UNCHANGED);
    if (logo.empty()) {
        cerr << "Error: Could not load logo image!" << endl;
        return -1;
    }

    int width = static_cast<int>(cap1.get(CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap1.get(CAP_PROP_FRAME_HEIGHT));
    double fps = cap1.get(CAP_PROP_FPS);

    // Create output writer (no ffmpeg needed, just OpenCV's backend)
    VideoWriter writer(outputPath, VideoWriter::fourcc('a','v','c','1'), fps, Size(width, height));
    if (!writer.isOpened()) {
        cerr << "Error: Could not create output video!" << endl;
        return -1;
    }

    // Time (seconds) to split video1
    double splitTime = 5.0;
    int splitFrame = static_cast<int>(fps * splitTime);

    Mat frame;
    int frameCount = 0;

    cout << "Processing first part of video1..." << endl;

    // Part 1: Write frames from video1 until splitTime
    while (cap1.read(frame)) {
        if (frameCount >= splitFrame) break;
        overlayLogo(frame, logo);
        writer.write(frame);
        frameCount++;
    }

    cout << "Processing video2..." << endl;

    // Part 2: Insert entire video2
    while (cap2.read(frame)) {
        resize(frame, frame, Size(width, height));
        overlayLogo(frame, logo);
        writer.write(frame);
    }

    cout << "Processing remaining part of video1..." << endl;

    // Part 3: Continue with remaining frames of video1
    while (cap1.read(frame)) {
        overlayLogo(frame, logo);
        writer.write(frame);
    }

    cap1.release();
    cap2.release();
    writer.release();

    cout << "Video editing completed: " << outputPath << endl;
    return 0;
}
