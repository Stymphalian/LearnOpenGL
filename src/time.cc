#include "time.h"

#include <GLFW/glfw3.h>

double GlfwClock::getTimeSecs() {
  return glfwGetTime();
}

double GlfwClock::getTimeMillis() {
  return glfwGetTime()*1000;
}

double VirtualClock::getTimeSecs() {
  return getTimeMillis() / 1000;
}

double VirtualClock::getTimeMillis() {
  // get the real time
  double time_millis = GlfwClock::getTimeMillis();
  double real_delta = time_millis - _real_last_time_millis;
  _real_last_time_millis = time_millis;
  //_real_current_time_millis = time_millis;

  // Get the scaled delta time
  double virtual_delta = real_delta * _rate;

  // update the virtual time
  _virtual_current_time_millis += virtual_delta;
  return _virtual_current_time_millis;
}

void VirtualClock::setRate(float rate) {
  _rate = rate;
  if (_rate <= 0 ){ _rate= 1.0f;}
}
float VirtualClock::getRate() {
  return _rate;
}


ITimer& Timer::setIntervalMillis(double time_msecs) {
  _interval_msecs = time_msecs;
  return *this;
}
double Timer::getIntervalMillis() {
  return _interval_msecs;
  return _time_delta;
}

ITimer& Timer::tick() {
  double time_msecs = glfwGetTime()*1000;
  return tickWithTime(time_msecs);
}

ITimer& Timer::tickWithClock(IClock& clock) {
  double time_msecs = clock.getTimeMillis();
  return tickWithTime(time_msecs);
}

ITimer& Timer::tickWithTime(double time_msecs) {
  _current_time_msecs = time_msecs;
  _time_delta = time_msecs - _last_time_msecs;
  _last_time_msecs = time_msecs;

  _time_collected_msecs += _time_delta;
  if (_time_collected_msecs >= _interval_msecs) {
    _time_collected_msecs -= _interval_msecs;
    if (_time_collected_msecs < 0) {
      _time_collected_msecs = 0;
    }
    _passed += 1;
  }
  return *this;
}


int Timer::passed() {
  int result = _passed;
  _passed = 0;
  return result;
}
