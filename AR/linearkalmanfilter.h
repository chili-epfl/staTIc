#ifndef LINEARKALMANFILTER_H
#define LINEARKALMANFILTER_H
/*
    Kalman Filetr from OpenCv Tutorial
    http://docs.opencv.org/3.1.0/dc/d2c/tutorial_real_time_pose.html#gsc.tab=0
    samples/cpp/tutorial_code/calib3d/real_time_pose_estimation/
*/
#include <opencv2/video/tracking.hpp>
#include <QList>
class LinearKalmanFilter
{
public:
    LinearKalmanFilter();
    static cv::Mat euler2rot(const cv::Mat &euler);
    static cv::Mat rot2euler(const cv::Mat &rotationMatrix);
    void fillMeasurements(const cv::Mat &translation_measured, const cv::Mat &rotation_measured);
    void updateKalmanFilter(cv::Mat &translation_estimated, cv::Mat &rotation_estimated);
    void getLastEstimation(cv::Mat &translation_estimated, cv::Mat &rotation_estimated);
    bool isLastEstimationBasedOnMeasurement(){return estimate_was_measurement_based;}
private:
    float findClosestAngle(float from, float to);
    cv::Mat measurements;
    cv::KalmanFilter KF;         // instantiate Kalman Filter
    int nStates = 18;            // the number of states
    int nMeasurements = 6;       // the number of measured states
    int nInputs = 0;             // the number of control actions
    double dt = 1.0/15.0;           // time between measurements (1/FPS)
    void init();
    bool measurementsAvailable,estimate_was_measurement_based;
    QList<double> a1,a2,a3,x,y,z,sort_list;

    cv::Mat m_last_est_rvec,m_last_est_tvec;

};

#endif // LINEARKALMANFILTER_H
