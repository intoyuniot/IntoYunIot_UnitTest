#include "Interactive.h"

IntoRobotTestRunner _runner;

unsigned count_tests()
{
    unsigned total = 0;
    auto count = [&total](Test& t) {
        total++;
    };
    Test::for_each(count);
    return total;
}

void print_status(Print& out)
{
    out.printlnf("%d tests available. Press 't' to begin, or 'i'/'e' to include/exclude tests matching a pattern.", count_tests());
}

/**
 * A convenience method to setup serial.
 */
void unit_test_setup()
{
    Serial.begin(115200);
    Test::out = &Serial;
    print_status(Serial);
    _runner.begin();
}

String readLine(Stream& stream)
{
    char buf[80];
    serialReadLine(&stream, buf, sizeof(buf), 0);
    return String(buf);
}

bool isStartRequested(bool runImmediately) {
    if (runImmediately)
        return true;

    if (Serial.available()) {
        char c = Serial.read();
        if (c=='t') {
            return true;
        }
        static bool first_time = true;
        if (c=='i') {
            Serial.print("Glob me for tests to include: ");
            String s = readLine(Serial);
            if (s.length()) {
                if (first_time) {
                    first_time = false;
                    Test::exclude("*");
                }
                unsigned count = Test::include(s.c_str());
                Serial.println();
                Serial.printlnf("Included %d tests matching '%s'.", count, s.c_str());
            }
            else {
                Serial.println("no changes made.");
            }
        }
        else if (c=='e') {
            Serial.print("Glob me for tests to exclude:");
            String s = readLine(Serial);
            if (s.length()) {
                first_time = false;
                unsigned count = Test::exclude(s.c_str());
                Serial.println();
                Serial.printlnf("Excluded %d tests matching '%s'.", count, s.c_str());
            }
            else {
                Serial.println("no changes made.");
            }
        }
        else if (c=='E') {
            Serial.println("Excluding all tests.");
            Test::exclude("*");
        }
        else if (c=='I') {
            Serial.println("Including all tests.");
            Test::include("*");
        }
        else if (c=='l') {
            Serial.println("Current tests included:");
            unsigned count = 0;
            auto print_and_count = [&count](Test& t) {
                if (t.is_enabled()) {
                    t.get_name().printTo(Serial);
                    Serial.println();
                    count++;
                }
            };
            Test::for_each(print_and_count);
            Serial.printlnf("%d test(s) included.", count);
        }
        else if (c=='c') {
            print_status(Serial);
        }
        else if (c=='h') {
            Serial.println("Commands:");
            Serial.println(" i <glob>: include tests matching the glob");
            Serial.println(" e <glob>: exclude tests matching the glob");
            Serial.println(" I: include all tests");
            Serial.println(" E: exclude all tests");
            Serial.println(" c: count the tests");
            Serial.println(" l: list the tests");
            Serial.println(" t: start the tests");
            Serial.println(" h: get help. It looks a lot like what you're seeing now.");
            Serial.println("So...what'll it be, friend?");
        }
        else {
            Serial.print(c);
            Serial.println(": Not sure what you mean. Press 'h' for help.");
            print_status(Serial);
        }
    }

    return false;
}

/*
 * A convenience method to run tests as part of the main loop after a character
 * is received over serial.
 **/
void unit_test_loop(bool runImmediately, bool runTest)
{
    if (!_runner.isStarted() && isStartRequested(runImmediately)) {
        Serial.println("Running tests");
        _runner.start();
    }

    if (runTest && _runner.isStarted()) {
        Test::run();
    }
}

int IntoRobotTestRunner::testStatusColor() {
    if (Test::failed>0)
        return RGB_COLOR_RED;
    else if (Test::skipped>0)
        return RGB_COLOR_ORANGE;
    else
        return RGB_COLOR_GREEN;
}

void IntoRobotTestRunner::begin() {
    setState(WAITING);
}

