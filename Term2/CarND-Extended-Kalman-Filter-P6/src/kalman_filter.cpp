#include "kalman_filter.h"
#include "tools.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;


KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
  TODO:
    * predict the state
  */
	x_ = F_ * x_;
	MatrixXd Ft = F_.transpose();
	P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Kalman Filter equations
  */
	VectorXd z_pred = H_ * x_;
	VectorXd y = z - z_pred;
	MatrixXd Ht = H_.transpose();
	MatrixXd S = H_ * P_ * Ht + R_;
	MatrixXd Si = S.inverse();
	MatrixXd PHt = P_ * Ht;
	MatrixXd K = PHt * Si;

	//new estimate
	x_ = x_ + (K * y);
	long x_size = x_.size();
	MatrixXd I = MatrixXd::Identity(x_size, x_size);
	P_ = (I - K * H_) * P_;

}

VectorXd CartesianToPolar(const VectorXd &x_state){

  // convert radar measurements from cartesian coordinates (x, y, vx, vy) to polar (rho, phi, rho_dot) coordinates
	    VectorXd h_x =VectorXd(3);


		float px=x_state(0);
		float py=x_state(1);
		float vx=x_state(2);
		float vy=x_state(3);


	    float rho=sqrt(px*px+py*py);

	    if(rho < 0.0001){
	    	cout << "Rho too small - Division by Zero" << endl;
	    	rho = 0.0001; }


	    float phi = atan2(py, px);
	    phi = atan2(sin(phi),cos(phi)); //normalize angle

		float dot_rho =(px*vx+py*vy)/rho;

		h_x << rho,phi, dot_rho;

		return h_x;

}



void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Extended Kalman Filter equations
  */

    VectorXd h_x;
    h_x = CartesianToPolar(x_);

	VectorXd y = z - h_x;
	float angle=y(1);
	y(1) =  atan2(sin(angle),cos(angle)); //normalize the angle
	MatrixXd Ht = H_.transpose();
	MatrixXd S = H_ * P_ * Ht + R_;
	MatrixXd Si = S.inverse();
	MatrixXd PHt = P_ * Ht;
	MatrixXd K = PHt * Si;

	//new estimate
	x_ = x_ + (K * y);
	long x_size = x_.size();
	MatrixXd I = MatrixXd::Identity(x_size, x_size);
	P_ = (I - K * H_) * P_;


}
