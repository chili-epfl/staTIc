#include "linearkalmanfilter.h"
#include <QDebug>
LinearKalmanFilter::LinearKalmanFilter():
    m_last_est_rvec(3, 1, CV_64F),
    m_last_est_tvec(3, 1, CV_64F)

{
    estimate_was_measurement_based=false;
    measurementsAvailable=false;
    init();
}

void LinearKalmanFilter::init()
{
    KF.init(nStates, nMeasurements, nInputs, CV_64F);                 // init Kalman Filter
    cv::setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-6));       // set process noise
    cv::setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-6));   // set measurement noise
    KF.measurementNoiseCov.at<double>(3,3)=1e-6;
    KF.measurementNoiseCov.at<double>(4,4)=1e-6;
    KF.measurementNoiseCov.at<double>(5,5)=1e-6;

    cv::setIdentity(KF.errorCovPost, cv::Scalar::all(1));             // error covariance


    /** DYNAMIC MODEL **/

    //  [1 0 0 dt  0  0 dt2   0   0 0 0 0  0  0  0   0   0   0]
    //  [0 1 0  0 dt  0   0 dt2   0 0 0 0  0  0  0   0   0   0]
    //  [0 0 1  0  0 dt   0   0 dt2 0 0 0  0  0  0   0   0   0]
    //  [0 0 0  1  0  0  dt   0   0 0 0 0  0  0  0   0   0   0]
    //  [0 0 0  0  1  0   0  dt   0 0 0 0  0  0  0   0   0   0]
    //  [0 0 0  0  0  1   0   0  dt 0 0 0  0  0  0   0   0   0]
    //  [0 0 0  0  0  0   1   0   0 0 0 0  0  0  0   0   0   0]
    //  [0 0 0  0  0  0   0   1   0 0 0 0  0  0  0   0   0   0]
    //  [0 0 0  0  0  0   0   0   1 0 0 0  0  0  0   0   0   0]
    //  [0 0 0  0  0  0   0   0   0 1 0 0 dt  0  0 dt2   0   0]
    //  [0 0 0  0  0  0   0   0   0 0 1 0  0 dt  0   0 dt2   0]
    //  [0 0 0  0  0  0   0   0   0 0 0 1  0  0 dt   0   0 dt2]
    //  [0 0 0  0  0  0   0   0   0 0 0 0  1  0  0  dt   0   0]
    //  [0 0 0  0  0  0   0   0   0 0 0 0  0  1  0   0  dt   0]
    //  [0 0 0  0  0  0   0   0   0 0 0 0  0  0  1   0   0  dt]
    //  [0 0 0  0  0  0   0   0   0 0 0 0  0  0  0   1   0   0]
    //  [0 0 0  0  0  0   0   0   0 0 0 0  0  0  0   0   1   0]
    //  [0 0 0  0  0  0   0   0   0 0 0 0  0  0  0   0   0   1]

    // position
    KF.transitionMatrix.at<double>(0,3) = dt;
    KF.transitionMatrix.at<double>(1,4) = dt;
    KF.transitionMatrix.at<double>(2,5) = dt;
    KF.transitionMatrix.at<double>(3,6) = dt;
    KF.transitionMatrix.at<double>(4,7) = dt;
    KF.transitionMatrix.at<double>(5,8) = dt;
    KF.transitionMatrix.at<double>(0,6) = 0.5*pow(dt,2);
    KF.transitionMatrix.at<double>(1,7) = 0.5*pow(dt,2);
    KF.transitionMatrix.at<double>(2,8) = 0.5*pow(dt,2);

    // orientation
    KF.transitionMatrix.at<double>(9,12) = dt;
    KF.transitionMatrix.at<double>(10,13) = dt;
    KF.transitionMatrix.at<double>(11,14) = dt;
    KF.transitionMatrix.at<double>(12,15) = dt;
    KF.transitionMatrix.at<double>(13,16) = dt;
    KF.transitionMatrix.at<double>(14,17) = dt;
    KF.transitionMatrix.at<double>(9,15) = 0.5*pow(dt,2);
    KF.transitionMatrix.at<double>(10,16) = 0.5*pow(dt,2);
    KF.transitionMatrix.at<double>(11,17) = 0.5*pow(dt,2);


    /** MEASUREMENT MODEL **/

    //  [1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
    //  [0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
    //  [0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
    //  [0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0]
    //  [0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0]
    //  [0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0]

    KF.measurementMatrix.at<double>(0,0) = 1;  // x
    KF.measurementMatrix.at<double>(1,1) = 1;  // y
    KF.measurementMatrix.at<double>(2,2) = 1;  // z
    KF.measurementMatrix.at<double>(3,9) = 1;  // roll
    KF.measurementMatrix.at<double>(4,10) = 1; // pitch
    KF.measurementMatrix.at<double>(5,11) = 1; // yaw

    measurements=cv::Mat(nMeasurements, 1, CV_64F);
    measurements.setTo(cv::Scalar(0));

}


void LinearKalmanFilter::updateKalmanFilter(cv::Mat &translation_estimated, cv::Mat &rotation_estimated )
{

    // First predict, to update the internal statePre variable
    cv::Mat prediction = KF.predict();

    // The "correct" phase that is going to use the predicted value and our measurement

    cv::Mat estimated;
    if(measurementsAvailable){
        estimate_was_measurement_based=true;
        estimated= KF.correct(measurements);
    }
    else{
        estimate_was_measurement_based=false;
        estimated=prediction;
    }
    // Estimated translation
    if(!translation_estimated.empty()){
        translation_estimated.at<double>(0) = estimated.at<double>(0);
        translation_estimated.at<double>(1) = estimated.at<double>(1);
        translation_estimated.at<double>(2) = estimated.at<double>(2);
    }
    m_last_est_tvec.at<double>(0) = estimated.at<double>(0);
    m_last_est_tvec.at<double>(1) = estimated.at<double>(1);
    m_last_est_tvec.at<double>(2) = estimated.at<double>(2);

    // Estimated euler angles
    if(!rotation_estimated.empty()){
        rotation_estimated.at<double>(0) = estimated.at<double>(9);
        rotation_estimated.at<double>(1) = estimated.at<double>(10);
        rotation_estimated.at<double>(2) = estimated.at<double>(11);
    }
    m_last_est_rvec.at<double>(0) = estimated.at<double>(9);
    m_last_est_rvec.at<double>(1) = estimated.at<double>(10);
    m_last_est_rvec.at<double>(2) = estimated.at<double>(11);

    //  double a1_sum=0,a2_sum=0,a3_sum=0;
    //  for(int i=0;i<a1.size();i++){
    //      a1_sum+=a1[i];
    //      a2_sum+=a2[i];
    //      a3_sum+=a3[i];
    //  }
    //  a1_sum/=a1.size();
    //  a2_sum/=a1.size();
    //  a3_sum/=a1.size();
    //  eulers_estimated.at<double>(0) = a1_sum;
    //  eulers_estimated.at<double>(1) = a2_sum;
    //  eulers_estimated.at<double>(2) = a3_sum;
    // Convert estimated quaternion to rotation matrix


    measurementsAvailable=false;

}

void LinearKalmanFilter::getLastEstimation(cv::Mat &translation_estimated, cv::Mat &rotation_estimated)
{
    translation_estimated.at<double>(0) = m_last_est_tvec.at<double>(0);
    translation_estimated.at<double>(1) = m_last_est_tvec.at<double>(1);
    translation_estimated.at<double>(2) = m_last_est_tvec.at<double>(2);

    rotation_estimated.at<double>(0) = m_last_est_rvec.at<double>(0);
    rotation_estimated.at<double>(1) = m_last_est_rvec.at<double>(1);
    rotation_estimated.at<double>(2) = m_last_est_rvec.at<double>(2);
}

float LinearKalmanFilter::findClosestAngle(float from, float to)
{
    return (from + fmod(to - from + 180.f, 360.f)-180.f);
}

/**********************************************************************************************************/
void LinearKalmanFilter::fillMeasurements( const cv::Mat &translation_measured, const cv::Mat &rotation_measured)
{

    // Convert rotation matrix to euler angles
    //  cv::Mat measured_eulers(3, 1, CV_64F);
    //  measured_eulers = rot2euler(rotation_measured);

    // Set measurement to predict
    measurements.at<double>(0) = translation_measured.at<double>(0); // x
    measurements.at<double>(1) = translation_measured.at<double>(1); // y
    measurements.at<double>(2) = translation_measured.at<double>(2); // z
    measurements.at<double>(3) = findClosestAngle(measurements.at<double>(3),rotation_measured.at<double>(0));      // roll
    measurements.at<double>(4) = findClosestAngle(measurements.at<double>(4),rotation_measured.at<double>(1));      // pitch
    measurements.at<double>(5) = findClosestAngle(measurements.at<double>(5),rotation_measured.at<double>(2));      // yaw

    if(a1.size()>16){
        a1.pop_front();
        a2.pop_front();
        a3.pop_front();
        x.pop_front();
        y.pop_front();
        z.pop_front();
    }

    measurementsAvailable=true;

    if(a1.size()==16){
        sort_list.clear();
        sort_list.append(a1);
        std::sort(sort_list.begin(),sort_list.end());
        if(measurements.at<double>(3) > sort_list.at(12)+1.5*(sort_list.at(12)- sort_list.at(3))
                || measurements.at<double>(3)< sort_list.at(3)-1.5*(sort_list.at(12)- sort_list.at(3)) )
            measurementsAvailable=false;
        else{
            sort_list.clear();
            sort_list.append(a2);
            std::sort(sort_list.begin(),sort_list.end());
            if(measurements.at<double>(4)> sort_list.at(12)+1.5*(sort_list.at(12)- sort_list.at(3))
                    || measurements.at<double>(4)< sort_list.at(3)-1.5*(sort_list.at(12)- sort_list.at(3)))
                measurementsAvailable=false;
            else{
                sort_list.clear();
                sort_list.append(a3);
                std::sort(sort_list.begin(),sort_list.end());
                if(measurements.at<double>(5)> sort_list.at(12)+1.5*(sort_list.at(12)- sort_list.at(3))
                        || measurements.at<double>(5)< sort_list.at(3)-1.5*(sort_list.at(12)- sort_list.at(3)))
                    measurementsAvailable=false;
                else{
                    sort_list.clear();
                    sort_list.append(x);
                    std::sort(sort_list.begin(),sort_list.end());
                    if(measurements.at<double>(0)> sort_list.at(12)+1.5*(sort_list.at(12)- sort_list.at(3))
                            || measurements.at<double>(0)< sort_list.at(3)-1.5*(sort_list.at(12)- sort_list.at(3)))
                        measurementsAvailable=false;
                    else{
                        sort_list.clear();
                        sort_list.append(y);
                        std::sort(sort_list.begin(),sort_list.end());
                        if(measurements.at<double>(1)> sort_list.at(12)+1.5*(sort_list.at(12)- sort_list.at(3))
                                || measurements.at<double>(1)< sort_list.at(3)-1.5*(sort_list.at(12)- sort_list.at(3)))
                            measurementsAvailable=false;
                        else{
                            sort_list.clear();
                            sort_list.append(z);
                            std::sort(sort_list.begin(),sort_list.end());
                            if(measurements.at<double>(2)> sort_list.at(12)+1.5*(sort_list.at(12)- sort_list.at(3))
                                    || measurements.at<double>(2)< sort_list.at(3)-1.5*(sort_list.at(12)- sort_list.at(3)))
                                measurementsAvailable=false;
                        }
                    }
                }
            }
        }
    }

        x.append(measurements.at<double>(0));
        y.append(measurements.at<double>(1));
        z.append(measurements.at<double>(2));

        a1.append(measurements.at<double>(3));
        a2.append(measurements.at<double>(4));
        a3.append(measurements.at<double>(5));


}

// Converts a given Rotation Matrix to Euler angles
cv::Mat LinearKalmanFilter::rot2euler(const cv::Mat & rotationMatrix)
{
    cv::Mat euler(3,1,CV_64F);

    double m00 = rotationMatrix.at<double>(0,0);
    double m02 = rotationMatrix.at<double>(0,2);
    double m10 = rotationMatrix.at<double>(1,0);
    double m11 = rotationMatrix.at<double>(1,1);
    double m12 = rotationMatrix.at<double>(1,2);
    double m20 = rotationMatrix.at<double>(2,0);
    double m22 = rotationMatrix.at<double>(2,2);

    double x, y, z;

    // Assuming the angles are in radians.
    if (m10 > 0.998) { // singularity at north pole
        x = 0;
        y = CV_PI/2;
        z = atan2(m02,m22);
    }
    else if (m10 < -0.998) { // singularity at south pole
        x = 0;
        y = -CV_PI/2;
        z = atan2(m02,m22);
    }
    else
    {
        x = atan2(-m12,m11);
        y = asin(m10);
        z = atan2(-m20,m00);
    }

    euler.at<double>(0) = x;
    euler.at<double>(1) = y;
    euler.at<double>(2) = z;
    return euler;
}

// Converts a given Euler angles to Rotation Matrix
cv::Mat LinearKalmanFilter::euler2rot(const cv::Mat & euler)
{
    cv::Mat rotationMatrix(3,3,CV_64F);

    double x = euler.at<double>(0);
    double y = euler.at<double>(1);
    double z = euler.at<double>(2);

    // Assuming the angles are in radians.
    double ch = cos(z);
    double sh = sin(z);
    double ca = cos(y);
    double sa = sin(y);
    double cb = cos(x);
    double sb = sin(x);

    double m00, m01, m02, m10, m11, m12, m20, m21, m22;

    m00 = ch * ca;
    m01 = sh*sb - ch*sa*cb;
    m02 = ch*sa*sb + sh*cb;
    m10 = sa;
    m11 = ca*cb;
    m12 = -ca*sb;
    m20 = -sh*ca;
    m21 = sh*sa*cb + ch*sb;
    m22 = -sh*sa*sb + ch*cb;

    rotationMatrix.at<double>(0,0) = m00;
    rotationMatrix.at<double>(0,1) = m01;
    rotationMatrix.at<double>(0,2) = m02;
    rotationMatrix.at<double>(1,0) = m10;
    rotationMatrix.at<double>(1,1) = m11;
    rotationMatrix.at<double>(1,2) = m12;
    rotationMatrix.at<double>(2,0) = m20;
    rotationMatrix.at<double>(2,1) = m21;
    rotationMatrix.at<double>(2,2) = m22;

    return rotationMatrix;
}


