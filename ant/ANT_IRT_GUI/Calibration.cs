﻿using AntPlus.Profiles.BikePower;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace IRT_GUI
{
    public enum CalibrationFormState
    {
        BelowSpeed,
        Stabilizing,
        Ready,
        Coasting,
        Done
    }

    public class TickEvent
    {
        const string format = "{0:g}, {1:g}, {2:g}, {3:g}, {4:g}";

        public long TimestampMS;
        public byte Sequence;
        public byte TickDelta;
        public ushort Watts;
        public byte PowerEventCount;

        public override string ToString()
        {
            return string.Format(format, 
                TimestampMS,
                Sequence,
                TickDelta, 
                Watts,
                PowerEventCount);
        }
    }

    public class Calibration : IDisposable
    {
        private StreamWriter m_logFileWriter;
        private CalibrationForm m_form;

        private bool m_inCalibrationMode = false;
        private byte m_lastCount = 0;
        private BikePowerDisplay m_refPower;
        private Stopwatch m_stopwatch;
        private List<TickEvent> m_tickEvents;

        CalibrationFormState m_calibrationState;

        public Calibration()
        {
            m_calibrationState = CalibrationFormState.BelowSpeed;

            m_tickEvents = new List<TickEvent>();
            m_stopwatch = new Stopwatch();

            m_inCalibrationMode = true;
            m_stopwatch.Start();

            m_inCalibrationMode = true;
        }

        public void ShowCalibration(BikePowerDisplay refPower)
        {
            m_form = new CalibrationForm();
            m_refPower = refPower;
            m_form.Show();
        }

        public void ExitCalibration()
        {
            // open up a stream to start logging
            string filename = string.Format("calib_{0}_{1:yyyyMMdd-HHmmss-F}.csv",
                typeof(Calibration).Assembly.GetName().Version.ToString(3),
                DateTime.Now);

            using (m_logFileWriter = new StreamWriter(filename))
            {
                m_logFileWriter.WriteLine("timestamp_ms, count, ticks, watts, pwr_events");

                foreach (var tick in m_tickEvents)
                {
                    m_logFileWriter.WriteLine(tick);
                }

                m_logFileWriter.Flush();
                m_logFileWriter.Close();
            }

            if (m_stopwatch != null)
            {
                m_stopwatch.Stop();
                m_stopwatch = null;
            }

            m_calibrationState = CalibrationFormState.Done;
            m_form.Update(0, 0, 0, m_calibrationState);
        }

        public void LogCalibration(byte[] buffer)
        {
            long ms = 0;
            TickEvent tick = null;
            byte pwrEventCount = 0;
            ushort watts = 0;

            // Single entrance.
            lock (this)
            {
                ms = m_stopwatch.ElapsedMilliseconds;

                if (m_refPower != null && m_refPower.StandardPowerOnly != null)
                {
                    watts = m_refPower.StandardPowerOnly.InstantaneousPower;
                    pwrEventCount = m_refPower.StandardPowerOnly.EventCount;
                }

                // If we already saw this message, skip it.
                if (m_lastCount > 0 && m_lastCount == buffer[0])
                {
                    return;
                }

                for (int i = 0; i < buffer.Length - 1; i++)
                {
                    // Each one came at 50ms intervals.
                    long timestamp = ms - ( 200 - (i*50) );

                    if (timestamp < 0)
                        timestamp = 0;

                    tick = new TickEvent() { 
                        TimestampMS = timestamp, 
                        Sequence = buffer[0], 
                        TickDelta = buffer[1 + i], 
                        Watts = watts, 
                        PowerEventCount = pwrEventCount };

                    m_tickEvents.Add(tick);
                }

                // Byte 0 has the event count, store it.
                m_lastCount = buffer[0];
            }

            if (m_form != null)
            {
                bool stable = false;

                double mph = CalculateSpeed(m_tickEvents);

                m_form.Update(mph, ms, 0, m_calibrationState);
            }
        }

        public void Dispose()
        {
            ExitCalibration();
        }

        private double CalculateSpeed(List<TickEvent> events)
        {
            byte tickDelta;
            long ms;

            // 1/2 second Average 
            if (events.Count < 5)
                return 0.0;

            var last20 = events.Skip(Math.Max(0, events.Count() - 5)).Take(5);
            var sum = last20.Sum(e => e.TickDelta);

            // Take into consideration that the first data point assumes it was read after 50ms of reading.
            ms = (last20.Last().TimestampMS - events[events.Count()-5].TimestampMS) + 50;

            // double mph = (tickDelta * 20 * 0.11176 / 2) * 2.23694;
            double distance_M = (sum / 2.0f) * 0.11176f;
            double mps = distance_M / (ms / 1000.0f);

            double mph = mps * 2.23694f;

            //
            // Determine if speed has decreased
            //

            // Calculate the state based on speed.
            if (m_calibrationState != CalibrationFormState.Coasting &&
                mph < 30.0f)
            {
                m_calibrationState = CalibrationFormState.BelowSpeed;
            }
            else
            {
                m_calibrationState = CalibrationFormState.Ready;
            }

            return mph;
        }
    }
}
