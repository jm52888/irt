﻿using System;
using System.IO;
using ANT_Console.Messages;

namespace ANT_Console
{
    public interface IReporter
    {
        void Report(DataPoint data);
    }

    public class LogReporter : IReporter, IDisposable
    {
        StreamWriter m_logFileWriter;
        System.Timers.Timer m_timer;
        const string report_format = "{0:H:mm:ss.fff}, {1:N4}, {2:N4}, {3:N1}, {4:g}, {5:g}, {6:g}, {7:g}, {8:g}, {9:g}";

        public LogReporter()
        {
            string filename = string.Format("log-{0:yyyyMMdd-HHmmss-F}.csv",
                    DateTime.Now);

            m_logFileWriter = new StreamWriter(filename);
            m_logFileWriter.AutoFlush = true;
            m_logFileWriter.WriteLine("event_time, bike_speed_mps, emotion_speed_mps, emotion_speed_mph, emotion_power, quarq_power, calc_power, servo_pos, accelerometer_y, temperature");
        }

        public void Report(DataPoint data)
        {
            string message = String.Format(report_format,
                DateTime.Now,
                data.SpeedReference,
                0 /* mps */,
                data.SpeedEMotion,
                data.PowerEMotion,
                data.PowerReference,
                0 /* calc */,
                data.ServoPosition,
                data.Accelerometer_y,
                data.Temperature
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length - 1].CumulativeWheelRevs : 0,
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length-1].EventTime : 0
                );

            m_logFileWriter.WriteLine(message);
        }

        public void Dispose()
        {
            m_timer.Stop();
            m_logFileWriter.Flush();
            m_logFileWriter.Close();
        }
    }
}
