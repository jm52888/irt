﻿using System;
using System.IO;

namespace ANT_Console_Demo
{

    public class Reporter : IDisposable
    {
        Collector m_collector;
        Calculator m_calculator;
        StreamWriter m_logFileWriter;
        const string report_format = "{0:H:mm:ss.fff}, {1:N4}, {2:N4}, {3:N1}, {4:N0}, {5:N0}, {6:N0}";

        public Reporter(Collector collector)
        {
            m_calculator = new Calculator();
            m_collector = collector;
            m_logFileWriter = new StreamWriter("log.csv");
            m_logFileWriter.AutoFlush = true;
        }

        public void Start()
        {
            Console.WriteLine("Starting....");
            m_logFileWriter.WriteLine("event_time, bike_speed_mps, emotion_speed_mps, emotion_speed_mph, emotion_power, quarq_power, mag_level");
            System.Timers.Timer timer = new System.Timers.Timer(1000);
            timer.Elapsed += Reporter_Elapsed;
            timer.Start();
        }

        void Reporter_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            Report(m_collector.EventData);
        }

        public void Report(CollectorEventData eventData)
        {
            eventData.bike_speed = m_calculator.GetBikeSpeed(
                m_collector.PopBikeSpeedEvents());

            eventData.emotion_speed = m_calculator.GetSpeed(
                m_collector.PopTorqueSpeedEvents());

            string data = String.Format(report_format,
                DateTime.Now,
                eventData.bike_speed,
                eventData.emotion_speed,
                eventData.emotion_speed * 2.23693629f,
                eventData.emotion_power,
                eventData.quarq_power,
                eventData.resistance_level);
            m_logFileWriter.WriteLine(data);

            Console.WriteLine(data);
        }

        public void Dispose()
        {
            m_logFileWriter.Flush();
            m_logFileWriter.Close();
        }
    }
}