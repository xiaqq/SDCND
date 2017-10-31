/*
 * particle_filter.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: Tiffany Huang
 */

#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h> 
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>

#include "particle_filter.h"

using namespace std;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
	// TODO: Set the number of particles. Initialize all particles to first position (based on estimates of 
	//   x, y, theta and their uncertainties from GPS) and all weights to 1. 
	// Add random Gaussian noise to each particle.
	// NOTE: Consult particle_filter.h for more information about this method (and others in this file).
	double std_x, std_y, std_theta;

	num_particles = 250; //defined in the header file
	default_random_engine gen;

	//Set standard deviations for x, y, and theta
	std_x = std[0];
	std_y = std[1];
	std_theta = std[2];

	// This line creates a normal (Gaussian) distribution for x,y and theta
	normal_distribution<double> dist_x(x, std_x);
	normal_distribution<double> dist_y(y, std_y);
	normal_distribution<double> dist_theta(theta, std_theta);

	for (int i = 0; i < num_particles; ++i) {
		Particle temp;
		temp.id = i;
		temp.x = dist_x(gen);
		temp.y = dist_y(gen);
		temp.theta = dist_theta(gen);
		temp.weight = 1;
		particles.push_back(temp); //add the newly initialized particle to the particle pool

	}
	is_initialized = true;

}

void ParticleFilter::prediction(double delta_t, double std_pos[],
		double velocity, double yaw_rate) {
	// TODO: Add measurements to each particle and add random Gaussian noise.
	// NOTE: When adding noise you may find std::normal_distribution and std::default_random_engine useful.
	//  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
	//  http://www.cplusplus.com/reference/random/default_random_engine/
	default_random_engine gen;
	double std_x, std_y, std_theta;
	std_x = std_pos[0];
	std_y = std_pos[1];
	std_theta = std_pos[2];

	// This line creates a normal (Gaussian) distribution for noise of x,y and theta
	normal_distribution<double> noise_x(0, std_x);
	normal_distribution<double> noise_y(0, std_y);
	normal_distribution<double> noise_theta(0, std_theta);
	//cout << "number of particles is"<<num_particles << endl;
	for (int i = 0; i < num_particles; ++i) {

		if (fabs(yaw_rate > 0.01)) {
			particles[i].x = particles[i].x + (velocity/yaw_rate)
							* (sin(particles[i].theta + yaw_rate * delta_t)
									- sin(particles[i].theta));
			particles[i].y = particles[i].y + (velocity/yaw_rate)
							* (cos(particles[i].theta)
									- cos(particles[i].theta + yaw_rate * delta_t));
		}
		else {
			particles[i].x = particles[i].x
					+ velocity * delta_t * cos(particles[i].theta);
			particles[i].y = particles[i].y
					+ velocity * delta_t * sin(particles[i].theta);

		}
		particles[i].theta = particles[i].theta + yaw_rate * delta_t;

        //add noise to the predicted particle position
		particles[i].x += noise_x(gen);	//add noise to x
		particles[i].y += noise_y(gen);	//add noise to y
		particles[i].theta += noise_theta(gen);	//add noise to theta
	}

}

void ParticleFilter::dataAssociation(std::vector<LandmarkObs> predicted,
		std::vector<LandmarkObs>& observations) {
	// TODO: Find the predicted measurement that is closest to each observed measurement and assign the 
	//   observed measurement to this particular landmark.
	// NOTE: this method will NOT be called by the grading code. But you will probably find it useful to 
	//   implement this method and use it as a helper during the updateWeights phase.

}

/*for each particle:
 for each observation:
 transform_observation_to_map
 for each landmark:
 calculate euclidean distance and associate to particle the landmark id with the min distance
 */

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[],
		const std::vector<LandmarkObs> &observations,
		const Map &map_landmarks) {

	// TODO: Update the weights of each particle using a mult-variate Gaussian distribution. You can read
	//   more about this distribution here: https://en.wikipedia.org/wiki/Multivariate_normal_distribution
	// NOTE: The observations are given in the VEHICLE'S coordinate system. Your particles are located
	//   according to the MAP'S coordinate system. You will need to transform between the two systems.
	//   Keep in mind that this transformation requires both rotation AND translation (but no scaling).
	//   The following is a good resource for the theory:
	//   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
	//   and the following is a good resource for the actual equation to implement (look at equation 
	//   3.33
	//   http://planning.cs.uiuc.edu/node99.html
	weights.clear();

	double sig_x = std_landmark[0];
	double sig_y = std_landmark[1];






	for (int i = 0; i < num_particles; ++i) {
		vector<LandmarkObs> FilteredLandmarksInRange;
			//cout<< "enter particle loop"<< endl;
		// List of landmarks within sensor range
		for (int l = 0; l < map_landmarks.landmark_list.size(); ++l) {
			double distance = dist(particles[i].x, particles[i].y,map_landmarks.landmark_list[l].x_f,map_landmarks.landmark_list[l].y_f);

			if (distance <= sensor_range) {
				LandmarkObs temp;
				temp.x = map_landmarks.landmark_list[l].x_f;
				temp.y = map_landmarks.landmark_list[l].y_f;
				temp.id = map_landmarks.landmark_list[l].id_i;
				FilteredLandmarksInRange.push_back(temp); //add the temp to the landmarks in Range vector
			}
		}
		vector<LandmarkObs> predicted; // has to be declared here instead of the outside of the particle loop!!!!
		vector<LandmarkObs> nearest_neighbor_landmark;//has to be declared here!!
		for (int j = 0; j < observations.size(); ++j) {
			//cout<< "enter observations loop"<< endl;
			//transform to map x coordinate
			double x_map;
			double y_map;
		    x_map = particles[i].x + (cos(particles[i].theta) * observations[j].x)
					- (sin(particles[i].theta) * observations[j].y); //observed measurement in global coordinate

			// transform to map y coordinate
			y_map = particles[i].y + (sin(particles[i].theta) * observations[j].x)
					+ (cos(particles[i].theta) * observations[j].y);

			//cout <<"observation" <<  j << "("<< observations[j].x << ","<< observations[j].y << ")"<< "transformed to "<<"("<<x_map<<","<<y_map   <<")"<<endl;

			double min_temp = INFINITY;
			double min_distance_x;
		    double min_distance_y;
		    int id;

		    min_distance_x=0.0;
		    min_distance_y=0.0;


			//for (int n = 0; n < map_landmarks.landmark_list.size(); ++n) { //find the nearst landmark match that measurement(observation)
		    for (int n = 0; n < FilteredLandmarksInRange.size(); ++n) { //find the nearst landmark match that measurement(observation)

				double temp = dist( FilteredLandmarksInRange[n].x,
								FilteredLandmarksInRange[n].y,x_map,y_map);
				//cout<< "enter filtered landmark loop"<< endl;
				if (temp < min_temp) {
					min_temp = temp;
					min_distance_x = FilteredLandmarksInRange[n].x;
					min_distance_y = FilteredLandmarksInRange[n].y;
					id = FilteredLandmarksInRange[n].id;


				}

			}

			LandmarkObs predict_temp;
			predict_temp.x = x_map;
			predict_temp.y = y_map;
			predict_temp.id = id;
			predicted.push_back(predict_temp);
			LandmarkObs nearest_landmark;
			nearest_landmark.x = min_distance_x;
			nearest_landmark.y = min_distance_y;
			nearest_neighbor_landmark.push_back(nearest_landmark);

			cout << "the closest landmark is" << id <<endl;
		}
		double updated_weight = 1.0;

		for (int w = 0; w < predicted.size(); ++w) {
			//cout << "enter update weight loop" << endl;
			double dx = predicted[w].x - nearest_neighbor_landmark[w].x;
			double dy = predicted[w].y - nearest_neighbor_landmark[w].y;
			updated_weight = updated_weight *( 1 / (2.0 * M_PI * sig_x * sig_y))
					* exp(
							-(dx * dx / (2 * sig_x * sig_x))
									- (dy * dy / (2 * sig_y * sig_y)));

		}
		particles[i].weight = updated_weight;
		weights.push_back(particles[i].weight);

	}

}

void ParticleFilter::resample() {
// TODO: Resample particles with replacement with probability proportional to their weight.
// NOTE: You may find std::discrete_distribution helpful here.
//   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
	//cout << "enter resample" << endl;
	discrete_distribution<int> d(weights.begin(), weights.end());

	vector<Particle> new_particles;

	default_random_engine gen;

	for (int n = 0; n < num_particles; ++n) {
		//cout << "enter resampled particles loop" << endl;
		new_particles.push_back(particles[d(gen)]);
	}
	particles = new_particles;
}

Particle ParticleFilter::SetAssociations(Particle particle,
		std::vector<int> associations, std::vector<double> sense_x,
		std::vector<double> sense_y) {
//particle: the particle to assign each listed association, and association's (x,y) world coordinates mapping to
// associations: The landmark id that goes along with each listed association
// sense_x: the associations x mapping already converted to world coordinates
// sense_y: the associations y mapping already converted to world coordinates

//Clear the previous associations
	particle.associations.clear();
	particle.sense_x.clear();
	particle.sense_y.clear();

	particle.associations = associations;
	particle.sense_x = sense_x;
	particle.sense_y = sense_y;

	return particle;
}

string ParticleFilter::getAssociations(Particle best) {
	vector<int> v = best.associations;
	stringstream ss;
	copy(v.begin(), v.end(), ostream_iterator<int>(ss, " "));
	string s = ss.str();
	s = s.substr(0, s.length() - 1);  // get rid of the trailing space
	return s;
}
string ParticleFilter::getSenseX(Particle best) {
	vector<double> v = best.sense_x;
	stringstream ss;
	copy(v.begin(), v.end(), ostream_iterator<float>(ss, " "));
	string s = ss.str();
	s = s.substr(0, s.length() - 1);  // get rid of the trailing space
	return s;
}
string ParticleFilter::getSenseY(Particle best) {
	vector<double> v = best.sense_y;
	stringstream ss;
	copy(v.begin(), v.end(), ostream_iterator<float>(ss, " "));
	string s = ss.str();
	s = s.substr(0, s.length() - 1);  // get rid of the trailing space
	return s;
}
