/**
 * Copyright 2014  Matthew McGowan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
   This is modification of ArduinoUnit to compile on the intorobot. The required headers and cpp files have been combined into a single
   header.
   */

#pragma once

#include "IntoYunIot_UnitTest.h"

enum RunnerState {
    INIT,
    WAITING,
    RUNNING,
    COMPLETE
};

class IntoRobotTestRunner {

    private:
        int _state;

    public:
        IntoRobotTestRunner() : _state(INIT) {

        }

        void begin();

        bool isStarted() {
            return _state>=RUNNING;
        }

        bool isComplete() {
            return _state==COMPLETE;
        }

        void start() {
            if (!isStarted())
                setState(RUNNING);
        }

        const char* nameForState(RunnerState state) {
            switch (state) {
                case INIT: return "init";
                case WAITING: return "waiting";
                case RUNNING: return "running";
                case COMPLETE: return "complete";
                default:
                               return "";
            }
        }

        int testStatusColor();

        void updateLEDStatus() {
            int rgb = testStatusColor();
            RGB.control(true);
            RGB.color(rgb);
        }

        RunnerState state() const { return (RunnerState)_state; }

        void setState(RunnerState newState) {
            if (newState!=_state) {
                _state = newState;
                const char* stateName = nameForState((RunnerState)_state);
                if (isStarted())
                    updateLEDStatus();
            }
        }

        void testDone() {
            updateLEDStatus();
        }
};

extern IntoRobotTestRunner _runner;

/**
 * A convenience method to setup serial.
 */
void unit_test_setup();
/*
 * A convenience method to run tests as part of the main loop after a character
 * is received over serial.
 *
 * @param runImmediately    When true, the test runner is started on first call to this function.
 *  Otherwise the test runner is only started when an external start signal is received.
 * @param
 **/
void unit_test_loop(bool runImmediately=false, bool runTest=true);


#define UNIT_TEST_SETUP() \
    void setup() { unit_test_setup(); }

#define UNIT_TEST_LOOP() \
    void loop() { unit_test_loop(); }

#define UNIT_TEST_APP() \
    UNIT_TEST_SETUP(); UNIT_TEST_LOOP();

