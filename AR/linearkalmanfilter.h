#ifndef LINEARKALMANFILTER_H
#define LINEARKALMANFILTER_H

#include <opencv2/video/tracking.hpp>
class LinearKalmanFilter
{
public:
    LinearKalmanFilter();
    static cv::Mat euler2rot(const cv::Mat &euler);
    static cv::Mat rot2euler(const cv::Mat &rotationMatrix);
    void fillMeasurements(const cv::Mat &translation_measured, const cv::Mat &rotation_measured);
    void updateKalmanFilter(cv::Mat &translation_estimated, cv::Mat &rotation_estimated);

private:
    cv::Mat measurements;
    cv::KalmanFilter KF;         // instantiate Kalman Filter
    int nStates = 18;            // the number of states
    int nMeasurements = 6;       // the number of measured states
    int nInputs = 0;             // the number of control actions
    double dt = 0.125;           // time between measurements (1/FPS)
    void init();

};

#endif // LINEARKALMANFILTER_H
