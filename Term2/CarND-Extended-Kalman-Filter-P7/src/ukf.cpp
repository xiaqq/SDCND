#include "ukf.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/**
 * Initializes Unscented Kalman filter
 */
UKF::UKF() {
	is_initialized_ = false;

	// if this is false, laser measurements will be ignored (except during init)
	use_laser_ = true;

	// if this is false, radar measurements will be ignored (except during init)
	use_radar_ = true;

	// initial state vector
	x_ = VectorXd(5);

	// initial covariance matrix
	P_ = MatrixXd(5, 5);

	// Process noise standard deviation longitudinal acceleration in m/s^2
	std_a_ = 0.5;

	// Process noise standard deviation yaw acceleration in rad/s^2
	std_yawdd_ = 0.5;

	// Laser measurement noise standard deviation position1 in m
	std_laspx_ = 0.15;

	// Laser measurement noise standard deviation position2 in m
	std_laspy_ = 0.15;

	// Radar measurement noise standard deviation radius in m
	std_radr_ = 0.3;

	// Radar measurement noise standard deviation angle in rad
	std_radphi_ = 0.03;

	// Radar measurement noise standard deviation radius change in m/s
	std_radrd_ = 0.3;

	/**
	 TODO:

	 Complete the initialization. See ukf.h for other member properties.

	 Hint: one or more values initialized above might be wildly off...
	 */

	P_ << 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1;
	n_x_ = 5; //CTRV model
	n_aug_ = 7;
	weights_ = VectorXd(2 * n_aug_ + 1);

	Xsig_pred_ = MatrixXd(n_x_, 2 * n_aug_ + 1);
	Xsig_aug_ = MatrixXd(n_aug_, 2 * n_aug_ + 1);

}

UKF::~UKF() {
}

/**
 * @param {MeasurementPackage} meas_package The latest measurement data of
 * either radar or laser.
 */
void UKF::ProcessMeasurement(MeasurementPackage meas_package) {
	/**
	 TODO:

	 Complete this function! Make sure you switch between lidar and radar
	 measurements.
	 */

	/*****************************************************************************
	 *  Initialization
	 ****************************************************************************/
	if (!is_initialized_) {
		/**
		 TODO:
		 * Initialize the state x_ with the first measurement.
		 * Create the covariance matrix.
		 * Remember: you'll need to convert radar from polar to cartesian coordinates.
		 */
		// first measurement
		if (meas_package.sensor_type_ == MeasurementPackage::RADAR) {
			/**
			 Convert radar from polar to cartesian coordinates and initialize state.
			 */
			cout << "start radar init" << endl;
			float rho = meas_package.raw_measurements_(0);
			float phi = meas_package.raw_measurements_(1);
			float dot_rho = meas_package.raw_measurements_(2);
			phi = atan2(sin(phi), cos(phi)); //normalize the angle

			x_(0) = rho * cos(phi); //px
			x_(1) = rho * sin(phi); //py
			x_(2) = 0; //couldn't use phi, since this phi is not the angle between the dot_rho and vx
			x_(3) = 0;
			x_(4) = 0;
			cout << "end radar init" << endl;

		}

		else if (meas_package.sensor_type_ == MeasurementPackage::LASER) {
			/**
			 Initialize state.
			 */
			cout << "start laser init" << endl;
			x_ << meas_package.raw_measurements_[0], meas_package.raw_measurements_[1], 0, 0, 0;

			cout << "end laser init" << endl;
		}
		time_us_ = meas_package.timestamp_;
		// done initializing, no need to predict or update
		is_initialized_ = true;

		return;

	}
	/* call the UKF prediction and Update*/

	float delta_t = (meas_package.timestamp_ - time_us_) / 1000000.0; //dt - expressed in seconds
	time_us_ = meas_package.timestamp_;
	cout << "UKF start" << endl;
	GenerateSigmal();
	cout << "PredictSigmal" << endl;
	PredictSigmal(delta_t);
	cout << "Prediction" << endl;
	Prediction(delta_t);

	if (meas_package.sensor_type_ == MeasurementPackage::RADAR && use_radar_ == true){
		cout << "radar Update" << endl;
		UpdateRadar(meas_package);
	}

	else if (meas_package.sensor_type_ == MeasurementPackage::LASER && use_laser_ == true){
		cout << "lidar Update using EKF" << endl;
		UpdateLidar(meas_package);
	}
	return;
}

/**
 * Predicts sigma points, the state, and the state covariance matrix.
 * @param {double} delta_t the change in time (in seconds) between the last
 * measurement and this one.
 */
void UKF::GenerateSigmal() {
	//set state dimension

	//define spreading parameter

	cout << "start generate" << endl;
	//create augmented mean vector
	VectorXd x_aug = VectorXd(7);
	//create augmented state covariance
	MatrixXd P_aug = MatrixXd(7, 7);

	//MatrixXd Xsig_aug_ = MatrixXd(n_aug_, 2 * n_aug_ + 1);
	lambda_ = 3 - n_aug_;

	//create augmented mean state
	x_aug.head(5) = x_;
	x_aug(5) = 0;
	x_aug(6) = 0;
	cout << "x_aug init finished" << endl;

	//create augmented covariance matrix
	P_aug.fill(0.0);
	P_aug.topLeftCorner(5, 5) = P_;
	P_aug(5, 5) = std_a_ * std_a_;
	P_aug(6, 6) = std_yawdd_ * std_yawdd_;
	cout << "p_aug init finished" << endl;
	//create square root matrix

	MatrixXd L = P_aug.llt().matrixL();
	cout << "L init finished" << endl;
	//create augmented sigma points
	cout << "start Xsig_aug" << endl;
	//cout << Xsig_aug<<endl;
	Xsig_aug_.col(0) = x_aug;
	cout << "Xsig_aug_ init finished" << endl;
	for (int i = 0; i < n_aug_; i++) {
		Xsig_aug_.col(i + 1) = x_aug + sqrt(lambda_ + n_aug_) * L.col(i);
		Xsig_aug_.col(i + 1 + n_aug_) = x_aug
				- sqrt(lambda_ + n_aug_) * L.col(i);
	}

}

void UKF::PredictSigmal(double delta_t) {

	//predict sigma points
	for (int i = 0; i < 2 * n_aug_ + 1; i++) {
		//extract values for better readability
		double p_x = Xsig_aug_(0, i);
		double p_y = Xsig_aug_(1, i);
		double v = Xsig_aug_(2, i);
		double yaw = Xsig_aug_(3, i);
		double yawd = Xsig_aug_(4, i);
		double nu_a = Xsig_aug_(5, i);
		double nu_yawdd = Xsig_aug_(6, i);

		//predicted state values
		double px_p, py_p;

		//avoid division by zero
		if (fabs(yawd) > 0.001) {
			px_p = p_x + v / yawd * (sin(yaw + yawd * delta_t) - sin(yaw));
			py_p = p_y + v / yawd * (cos(yaw) - cos(yaw + yawd * delta_t));
		} else {
			px_p = p_x + v * delta_t * cos(yaw);
			py_p = p_y + v * delta_t * sin(yaw);
		}

		double v_p = v;
		double yaw_p = yaw + yawd * delta_t;
		double yawd_p = yawd;

		//add noise
		px_p = px_p + 0.5 * nu_a * delta_t * delta_t * cos(yaw);
		py_p = py_p + 0.5 * nu_a * delta_t * delta_t * sin(yaw);
		v_p = v_p + nu_a * delta_t;

		yaw_p = yaw_p + 0.5 * nu_yawdd * delta_t * delta_t;
		yawd_p = yawd_p + nu_yawdd * delta_t;

		//write predicted sigma point into right column
		Xsig_pred_(0, i) = px_p;
		Xsig_pred_(1, i) = py_p;
		Xsig_pred_(2, i) = v_p;
		Xsig_pred_(3, i) = yaw_p;
		Xsig_pred_(4, i) = yawd_p;
	}

}

void UKF::Prediction(double delta_t) {

	/*TODO:
	 Complete this function! Estimate the object's location. Modify the state
	 vector, x_. Predict sigma points, the state, and the state covariance matrix.
	 */

	//create vector for weights
	// set weights
	double weight_0 = lambda_ / (lambda_ + n_aug_);
	weights_(0) = weight_0;
	for (int i = 1; i < 2 * n_aug_ + 1; i++) {  //2n+1 weights
		double weight = 0.5 / (n_aug_ + lambda_);
		weights_(i) = weight;
	}

	//predicted state mean
	x_.fill(0.0);
	for (int i = 0; i < 2 * n_aug_ + 1; i++) {  //iterate over sigma points
		x_ = x_ + weights_(i) * Xsig_pred_.col(i);
	}

	//predicted state covariance matrix
	P_.fill(0.0);
	for (int i = 0; i < 2 * n_aug_ + 1; i++) {  //iterate over sigma points

		// state difference
		VectorXd x_diff = Xsig_pred_.col(i) - x_;
		//angle normalization
		while (x_diff(3) > M_PI)
			x_diff(3) -= 2. * M_PI;
		while (x_diff(3) < -M_PI)
			x_diff(3) += 2. * M_PI;

		P_ = P_ + weights_(i) * x_diff * x_diff.transpose();

	}
}

/**
 * Updates the state and the state covariance matrix using a laser measurement.
 * @param {MeasurementPackage} meas_package
 */
void UKF::UpdateLidar(MeasurementPackage meas_package) {
	/**
	 TODO:

	 Complete this function! Use lidar data to update the belief about the object's
	 position. Modify the state vector, x_, and covariance, P_.

	 You'll also need to calculate the lidar NIS.
	 */
	cout << "update lidar using EKF" << endl;
	MatrixXd H_ = MatrixXd(2, 5);
	H_ << 1, 0, 0, 0, 0, 0, 1, 0, 0, 0;
	//Measurement update //
	VectorXd y = meas_package.raw_measurements_ - H_ * x_;

	// Lidar measurement noise //
	MatrixXd R_ = MatrixXd(2, 2);
	R_ << std_laspx_ * std_laspx_, 0, 0, std_laspy_ * std_laspy_;
	// measurement covariance //
	MatrixXd S = H_ * P_ * H_.transpose() + R_;
	// kalman gain //
	MatrixXd K_ = P_ * H_.transpose() * S.inverse();
	// State update //
	x_ = x_ + K_ * y;
	// Identity Matrix //
	MatrixXd I_ = MatrixXd::Identity(n_x_, n_x_);
	// State covariance update //
	P_ = (I_ - K_ * H_) * P_;

	// NIS //
	nis_laser_ = y.transpose() * S.inverse() * y;

}

/**
 * Updates the state and the state covariance matrix using a radar measurement.
 * @param {MeasurementPackage} meas_package
 */
void UKF::UpdateRadar(MeasurementPackage meas_package) {
	/**
	 TODO:

	 Complete this function! Use radar data to update the belief about the object's
	 position. Modify the state vector, x_, and covariance, P_.

	 You'll also need to calculate the radar NIS.
	 */
	//create matrix for sigma points in measurement space
	int n_z = 3;
	MatrixXd Zsig = MatrixXd(n_z, 2 * n_aug_ + 1);
	cout << "Zsig " << endl;
	//transform sigma points into measurement space
	for (int i = 0; i < 2 * n_aug_ + 1; i++) {  //2n+1 simga points

		// extract values for better readibility
		double p_x = Xsig_pred_(0, i);
		double p_y = Xsig_pred_(1, i);
		double v = Xsig_pred_(2, i);
		double yaw = Xsig_pred_(3, i);

		double v1 = cos(yaw) * v;
		double v2 = sin(yaw) * v;

		// measurement model
		Zsig(0, i) = sqrt(p_x * p_x + p_y * p_y);                        //r
		Zsig(1, i) = atan2(p_y, p_x);                                 //phi
		Zsig(2, i) = (p_x * v1 + p_y * v2) / sqrt(p_x * p_x + p_y * p_y); //r_dot
	}

	//mean predicted measurement
	VectorXd z_pred = VectorXd(n_z);
	z_pred.fill(0.0);
	for (int i = 0; i < 2 * n_aug_ + 1; i++) {
		z_pred = z_pred + weights_(i) * Zsig.col(i);
	}
	cout << "Zpred " << endl;
	//measurement covariance matrix S
	MatrixXd S = MatrixXd(n_z, n_z);
	S.fill(0.0);
	for (int i = 0; i < 2 * n_aug_ + 1; i++) {  //2n+1 simga points
		//residual
		VectorXd z_diff = Zsig.col(i) - z_pred;

		//angle normalization
		while (z_diff(1) > M_PI)
			z_diff(1) -= 2. * M_PI;
		while (z_diff(1) < -M_PI)
			z_diff(1) += 2. * M_PI;

		S = S + weights_(i) * z_diff * z_diff.transpose();
	}
	cout << "S" << endl;
	//add measurement noise covariance matrix
	MatrixXd R = MatrixXd(n_z, n_z);
	R << std_radr_ * std_radr_, 0, 0, 0, std_radphi_ * std_radphi_, 0, 0, 0, std_radrd_
			* std_radrd_;
	S = S + R;

	cout << "S+R " << endl;

	//create matrix for cross correlation Tc
	MatrixXd Tc = MatrixXd(n_x_, n_z);
	//calculate cross correlation matrix
	Tc.fill(0.0);
	for (int i = 0; i < 2 * n_aug_ + 1; i++) {  //2n+1 simga points

		//residual
		VectorXd z_diff = Zsig.col(i) - z_pred;
		//angle normalization
		while (z_diff(1) > M_PI)
			z_diff(1) -= 2. * M_PI;
		while (z_diff(1) < -M_PI)
			z_diff(1) += 2. * M_PI;

		// state difference
		VectorXd x_diff = Xsig_pred_.col(i) - x_;
		//angle normalization
		while (x_diff(3) > M_PI)
			x_diff(3) -= 2. * M_PI;
		while (x_diff(3) < -M_PI)
			x_diff(3) += 2. * M_PI;

		Tc = Tc + weights_(i) * x_diff * z_diff.transpose();
	}
	cout << "Tc" << endl;
	//Kalman gain K;
	MatrixXd K = Tc * S.inverse();
	cout << "k " << endl;
	//residual
	VectorXd z_diff = meas_package.raw_measurements_ - z_pred;

	//angle normalization
	while (z_diff(1) > M_PI)
		z_diff(1) -= 2. * M_PI;
	while (z_diff(1) < -M_PI)
		z_diff(1) += 2. * M_PI;
	cout << "z_diff " << endl;

	nis_radar_ = z_diff.transpose() * S.inverse() * z_diff;

	//update state mean and covariance matrix
	x_ = x_ + K * z_diff;
	P_ = P_ - K * S * K.transpose();

}