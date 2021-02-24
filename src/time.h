#pragma once

#include <chrono>

class IClock {
public:
  virtual double getTimeSecs() = 0;
  virtual double getTimeMillis() = 0;
};

class GlfwClock : public IClock {
public:
  virtual double getTimeSecs();
  virtual double getTimeMillis();
};

class VirtualClock : public GlfwClock {
public:
  virtual double getTimeSecs();
  virtual double getTimeMillis();
  void setRate(float rate);
  float getRate();

  float _rate = 1.0f;
  double _real_last_time_millis = 0;
  double _virtual_current_time_millis = 0;
};

class ITimer {
public:
  virtual ITimer &setIntervalMillis(double time_msecs) = 0;
  virtual double getIntervalMillis() = 0;

  virtual ITimer &tick() = 0;
  virtual ITimer &tickWithTime(double time_msecs) = 0;
  virtual ITimer &tickWithClock(IClock& clock) = 0;
  virtual int passed() = 0;

  // start or restart the timer
  // virtual ITimer& start();
  // virtual ITimer& pause();
  // virtual ITimer& stop();
};

class Timer : public ITimer {
public:
  ITimer &setIntervalMillis(double time_msecs);
  double getIntervalMillis();

  ITimer &tick();
  ITimer &tickWithTime(double time_msecs);
  ITimer &tickWithClock( IClock& clock);
  int passed();

  double getCurrentMillis() { return _current_time_msecs; }

protected:
  double _interval_msecs = 0;
  double _scaling = 1;

  double _current_time_msecs = 0;
  double _last_time_msecs = 0;
  double _time_delta = 0;
  double _time_collected_msecs = 0;

  int _passed = 0;
};
