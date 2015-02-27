﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Calibration
{
    /// <summary>
    /// Filters and processes raw data into usable speed/coastdown seconds.
    /// </summary>
    internal class CoastdownData
    {
        private int[] m_timestamp, m_flywheel;

        public CoastdownData(int[] timestamp_2048, int[] flywheel_ticks)
        {
            m_timestamp = timestamp_2048;
            m_flywheel = flywheel_ticks;
        }

        public double[] SpeedMps;
        public double[] CoastdownSeconds;

        public void Evaluate()
        {
            double[] seconds = new double[m_timestamp.Length / 4];
            double[] speed = new double[m_flywheel.Length / 4];

            int ix = 0;
            for (int idx = 0; idx < m_flywheel.Length; idx++)
            {
                int val = m_flywheel[idx];
                int dt, ds;

                // Evaluate every 4th record.
                if (idx > 0 && idx % 4 == 0)
                {
                    if (val < m_flywheel[idx - 4])
                        dt = val + (m_flywheel[idx - 4] ^ 0xFFFF);
                    else
                        dt = val - m_flywheel[idx - 4];

                    if (m_timestamp[idx] < m_timestamp[idx - 4])
                        ds = m_timestamp[idx] + (m_timestamp[idx - 4] ^ 0xFFFF);
                    else
                        ds = m_timestamp[idx] - m_timestamp[idx - 4];

                    if (ix > 0)
                        seconds[ix] = (ds / 2048.0) + seconds[ix - 1];
                    else
                        seconds[ix] = (ds / 2048.0);

                    /* Each flywheel revolution equals 0.1115 meters travelled by the 
                     * bike.  Two ticks are recorded for each revolution.
                     * Time is sent in 1/2048 of a second.
                     */
                    speed[ix++] = (dt * 0.1115 / 2.0) / (ds / 2048.0);
                }
            }

            int max = FindDecelerationIndex(speed);
            int min = FindMinSpeedIndex(speed);
            int len = min - max;

            SpeedMps = new double[len];
            CoastdownSeconds = new double[len];
            Array.Copy(speed, max, SpeedMps, 0, len);

            // Invert the timestamp seconds to record seconds to min speed.
            for (int i = 0; i < len; i++)
            {
                CoastdownSeconds[i] = seconds[len - 1] - seconds[i];
            }
        }

        /// <summary>
        /// Finds the point at which deceleration begins.
        /// </summary>
        /// <param name="values"></param>
        /// <returns></returns>
        private int FindDecelerationIndex(double[] speeds)
        {
            // Progress from the end of the array to the beginning.
            // Ensure that the values continue to increase.
            for (int i = speeds.Length - 1; i > 0; i--)
            {
                if (speeds[i] >= speeds[i - 1])
                {
                    return i + 1;
                }
            }

            // We shouldn't really get here.
            return 0;
        }

        private int FindMinSpeedIndex(double[] speeds)
        {
            // Get the first occurrence of 1 tick delta.
            // Array.IndexOf<double>(speeds, 1.0);

            return speeds.Length;
        }
    }
}
