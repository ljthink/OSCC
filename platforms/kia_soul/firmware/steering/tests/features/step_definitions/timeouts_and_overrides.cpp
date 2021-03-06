WHEN("^the time since the last received controller command exceeds the timeout$")
{
    g_steering_command_last_rx_timestamp = 0;

    g_mock_arduino_millis_return =
        COMMAND_TIMEOUT_IN_MSEC;

    check_for_timeouts();
}


WHEN("^the time since the last received Chassis State 1 report exceeds the timeout$")
{
    g_chassis_state_1_report_last_rx_timestamp = 0;

    g_mock_arduino_millis_return =
        CHASSIS_STATE_1_REPORT_TIMEOUT_IN_MSEC;

    check_for_timeouts();
}


WHEN("^the operator applies (.*) to the steering wheel$")
{
    REGEX_PARAM(int, torque_sensor_val);

    g_mock_arduino_analog_read_return = torque_sensor_val;

    // The exponential filter used requires multiple passes for it to recognize
    // an override
    for( int i = 0; i < 5; ++i )
    {
        check_for_operator_override();
    }
}


THEN("^override flag should be set$")
{
    assert_that(
        g_steering_control_state.operator_override,
        is_equal_to(true));
}


