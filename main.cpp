#include "iostream"
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

static const int POINT_QUANTITY = 100;
static const int SIN_WAVE_FREQUENCY_REDUCER = 1;
static const int SIN_WAVE_FREQUENCY_INCREASER = 2;
static const int START_HORIZONTAL_OFFSET = 20;
static const int END_HORIZONTAL_OFFSET = 20;
static const double BACKGROUND_SIZE_REDUCER = 1.4;

static const int DRONE_SIZE_REDUCER = 2;

int main()
{
    Mat robo_path = imread("sources/background/grass.png");
    resize(
            robo_path,
            robo_path,
            Size(lround(robo_path.cols / BACKGROUND_SIZE_REDUCER), lround(robo_path.rows / BACKGROUND_SIZE_REDUCER)),
            INTER_AREA);
    Mat robo_path_sin = robo_path.clone();
    Mat robo_path_drone = robo_path.clone();
    Mat robo_path_final = robo_path.clone();

    Mat drone = imread("sources/robot/drone.jpg");
    rotate(drone, drone, ROTATE_90_COUNTERCLOCKWISE);
    resize(drone,
           drone,
           Size(lround(drone.cols / DRONE_SIZE_REDUCER), lround(drone.rows / DRONE_SIZE_REDUCER)),
           INTER_AREA);
    drone.convertTo(drone, -1, 2, 2);

    int vertical_image_position[POINT_QUANTITY] = {0};

    int start_horizontal_position = START_HORIZONTAL_OFFSET;
    int end_horizontal_position = robo_path_sin.cols - drone.cols;
    int horizontal_step = lround((end_horizontal_position - start_horizontal_position) / POINT_QUANTITY);
    int sin_amplitude = lround((robo_path.rows - drone.rows) / 2);

    for (int step_quantity = 0; step_quantity < POINT_QUANTITY; step_quantity++)
    {
        vertical_image_position[step_quantity] = lround(robo_path_sin.rows / 2 +
                                                        sin_amplitude * sin(
                                                                (step_quantity * CV_PI * SIN_WAVE_FREQUENCY_INCREASER) /
                                                                (POINT_QUANTITY * SIN_WAVE_FREQUENCY_REDUCER)
                                                        )
        );
    }

    imshow("robo_path", robo_path);
    waitKey();

    Mat drone_gray;
    Mat drone_thresholded(drone.rows, drone.cols, 5);
    Mat drone_bitwised_or;
    Mat drone_thresholded_not;
    Mat drone_fr;
    Mat roi_final;

    while (waitKey() != 27)
    {
        int horizontal_image_position = start_horizontal_position;

        for (int step_quantity = 0; step_quantity < POINT_QUANTITY; step_quantity++)
        {
            Rect roi(Point(horizontal_image_position, vertical_image_position[step_quantity] - drone.rows / 2),
                     Size(drone.cols, drone.rows));
            Mat robo_path_drone_roi = robo_path_final(roi);

            cvtColor(drone, drone_gray, COLOR_RGB2GRAY);

            threshold(drone_gray, drone_thresholded, 254, 255, THRESH_BINARY);

            bitwise_or(robo_path_drone_roi, robo_path_drone_roi, drone_bitwised_or, drone_thresholded);

            bitwise_not(drone_gray, drone_thresholded_not);

            bitwise_and(drone, drone, drone_fr, drone_thresholded_not);

            add(drone_bitwised_or, drone_fr, roi_final);

            robo_path_final = robo_path_sin.clone();

            roi_final.copyTo(robo_path_final(roi));

            line(robo_path_sin,
                 Point(horizontal_image_position, vertical_image_position[step_quantity]),
                 Point(horizontal_image_position + horizontal_step, vertical_image_position[step_quantity + 1]),
                 CV_RGB(0, 255, 0),
                 5,
                 LINE_8,
                 0);

            imshow("robo_path", robo_path_final);
            waitKey(7);

            if (horizontal_image_position > (lround(end_horizontal_position / 2) - horizontal_step) &&
                horizontal_image_position < (lround(end_horizontal_position / 2) + horizontal_step))
            {
                imwrite("sources/workResult/theHalfOfTheRoboPath.jpg", robo_path_final);
            }

            robo_path_final = robo_path_sin.clone();

            horizontal_image_position += horizontal_step;
        }

        robo_path_sin = robo_path.clone();
        robo_path_final = robo_path.clone();
    }

    return 0;
}