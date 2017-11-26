# CarND-Controls-MPC
Self-Driving Car Engineer Nanodegree Program

---

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1(mac, linux), 3.81(Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `install-mac.sh` or `install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.

* **Ipopt and CppAD:** Please refer to [this document](https://github.com/udacity/CarND-MPC-Project/blob/master/install_Ipopt_CppAD.md) for installation instructions.
* [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page). This is already part of the repo so you shouldn't have to worry about it.
* Simulator. You can download these from the [releases tab](https://github.com/udacity/self-driving-car-sim/releases).
* Not a dependency but read the [DATA.md](./DATA.md) for a description of the data sent back from the simulator.


## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./mpc`.

## Tips

1. It's recommended to test the MPC on basic examples to see if your implementation behaves as desired. One possible example
is the vehicle starting offset of a straight line (reference). If the MPC implementation is correct, after some number of timesteps
(not too many) it should find and track the reference line.
2. The `lake_track_waypoints.csv` file has the waypoints of the lake track. You could use this to fit polynomials and points and see of how well your model tracks curve. NOTE: This file might be not completely in sync with the simulator so your solution should NOT depend on it.
3. For visualization this C++ [matplotlib wrapper](https://github.com/lava/matplotlib-cpp) could be helpful.)
4.  Tips for setting up your environment are available [here](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/0949fca6-b379-42af-a919-ee50aa304e6a/lessons/f758c44c-5e40-4e01-93b5-1a82aa4e044f/concepts/23d376c7-0195-4276-bdf0-e02f1f3c665d)
5. **VM Latency:** Some students have reported differences in behavior using VM's ostensibly a result of latency.  Please let us know if issues arise as a result of a VM environment.

##Project Summary
1. Model Variables and cost function

 The Variables in the MPC model consists of states and actuators. The states are: x, y, psi,v,cte and espi, where x,y,psi is the coordination and orientation of the vehicle, and the cte and espi are the cross check error and orientation error respectively. the actuators are delta and a, where delta is the steering angle and a is the acceleration.

 The cost function is named as fg and its 0th element contains the sum of all the calculated results. The car's behavior in the simulator determined by the cost function value. Some of the parameters are penalized and wights for those parameters shown below.

 // The part of the cost based on the reference state.

       for (int t = 0; t < N; t++) {
	       fg[0] += 2000*CppAD::pow(vars[cte_start + t], 2);
	       fg[0] += 2000*CppAD::pow(vars[epsi_start + t], 2);
	       fg[0] += CppAD::pow(vars[v_start + t] - ref_v, 2);
	     }

	     // Minimize the use of actuators.
	     for (int t = 0; t < N - 1; t++) {
	       fg[0] += CppAD::pow(vars[delta_start + t], 2);
	       fg[0] += CppAD::pow(vars[a_start + t], 2);
	     }

	     // Minimize the value gap between sequential actuations.
	     for (int t = 0; t < N - 2; t++) {
	       fg[0] += 1500*CppAD::pow(vars[delta_start + t + 1] - vars[delta_start + t], 2);
	       fg[0] += CppAD::pow(vars[a_start + t + 1] - vars[a_start + t], 2);
	     }

2. Kinetic model and the constraints

The following equations describe the kinetic model used in the MPC.

  x_(t+1) = x_t + v_t * cos(psi_t) * dt;

  y_(t+1) = y_t + v_t * sin(psi_t) * dt;

  psi_(t+1) = psi_t + v_t * delta_t * dt / Lf;

  v_(t+1) = v_t + a_t * dt;

  cte_(t+1) = f(x_t) - y_t + (v_t * sin(epsi_t) * dt);

  epsi_(t+1) = psi_t - des_psi_t + (v_t/L_f) * delta_t * dt;

 dt is the timestep. Lf is the distance between front wheels to center of gravity. f(x_t) is the trajectory determined by polynomanial line of waypoints. des_psi_t is the arctan of f(x_t) slope.
 The constraints therefore for the model can be set up as following:


 for (int i = 1; i < N; i++) {



      AD<double> x1 = vars[x_start + i];

      AD<double> y1 = vars[y_start + i];

      AD<double> psi1 = vars[psi_start + i];

      AD<double> v1 = vars[v_start + i];
      AD<double> cte1 = vars[cte_start + i];
      AD<double> epsi1 = vars[epsi_start + i];

      // State at time t
      AD<double> x0 = vars[x_start + i - 1];
      AD<double> y0 = vars[y_start + i - 1];
      AD<double> v0 = vars[v_start + i - 1];
      AD<double> psi0 = vars[psi_start + i - 1];
      AD<double> cte0 = vars[cte_start + i - 1];
      AD<double> epsi0 = vars[epsi_start + i - 1];

      // Actuator at time t
      AD<double> delta0 = vars[delta_start + i - 1];
      AD<double> a0 = vars[a_start + i - 1];

      AD<double> f0 = coeffs[0] + coeffs[1] * x0 + coeffs[2] * pow(x0, 2) + coeffs[3] * pow(x0, 3);
      AD<double> psides0 = CppAD::atan(coeffs[1] + 2 * coeffs[2] * x0 + 3 * coeffs[3] * x0 * x0);

      // Constraints
      fg[1 + x_start + i] = x1 - (x0 + v0 * CppAD::cos(psi0) * dt);
      fg[1 + y_start + i] = y1 - (y0 + v0 * CppAD::sin(psi0) * dt);
      fg[1 + psi_start + i] = psi1 - (psi0 - v0 * delta0 * dt / Lf);
      fg[1 + v_start + i] = v1 - (v0 + a0 * dt);
      fg[1 + cte_start + i] = cte1 - (f0 - y0 + v0 * CppAD::sin(epsi0) * dt);
      fg[1 + epsi_start + i] = epsi1 - (psi0 - psides0 - v0 * delta0 * dt / Lf);
    }

3. Latency

In a real car, an actuation command won't execute instantly - there will be a delay as the command propagates through the system. A realistic delay might be on the order of 100 milliseconds.

This is a problem called "latency", and it's a difficult challenge for some controllers - like a PID controller - to overcome. But a Model Predictive Controller can adapt quite well because we can model this latency in the system. The code to update the states is as following:

// Predicting 100 ms in the future.

    double latency = 0.1;
    const double Lf = 2.67;
    state[0] = v*cos(0) * latency;
    state[1] = v*sin(0) * latency;
    state[2] = (-v*steer_value*latency/Lf);
    state[3] = v + throttle_value*latency;
    state[4] = cte + v*sin(epsi) * latency;
    state[5] = epsi - (v/Lf) * steer_value * latency;

4. Choose N and dt values

I started with N = 20 and dt = 0.5. By having this large N the car got off the track. Since large N and dt cause large computation effort and slower the prediction, I started decreasing the N and dt. After a couple of rounds of trial and errors, with N=10 and  dt=0.15 it get the best results.
