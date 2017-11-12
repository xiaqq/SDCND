#include "PID.h"

using namespace std;

/*
 * TODO: Complete the PID class.
 */

PID::PID() {
}

PID::~PID() {
}

void PID::Init(double Kp, double Ki, double Kd) {

	this->Kp = Kp;
	this->Ki = Ki;
	this->Kd = Kd;
	this->prev_cte = 0;
	this->p_error = 0;
	this->i_error = 0;
	this->d_error = 0;
}

void PID::UpdateError(double cte) {


	this->d_error = cte - p_error; //p_error is now still the last time stamp's value!! (cte_p_error)/delta_t
	this->p_error = cte;
	this->i_error += p_error; //update i_error with new cte

}

double PID::TotalError() {

	double output = -this->Kp * p_error - this->Ki * this->i_error
			- this->Kd * this->d_error;
	return output;
}
