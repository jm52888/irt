﻿using System;
using System.Collections.Generic;
using System.Linq;

namespace IRT.Calibration
{
    public class PowerFit
    {
        private double[] m_coeff = { 0.0, 0.0 };
        private double m_inertia = 0;
        private DecelerationFit m_decelFit;

        public static double Power(double speedMph, double drag, double rr)
        {
            double speedMps = speedMph * 0.44704;
            return fit_drag_rr(speedMps, drag, rr);
        }

        public PowerFit()
        {
            m_decelFit = null;
        }

        public PowerFit(DecelerationFit decelFit)
        {
            m_decelFit = decelFit;
        }

        public double Drag { get { return m_coeff[0]; } }

        public double RollingResistance { get { return m_coeff[1]; } }

        public double Inertia {  get { return m_inertia; } set { m_inertia = value; } }

        public virtual double Watts(double speedMps)
        {
            return fit_drag_rr(speedMps, Drag, RollingResistance);
        }

        /// <summary>
        /// Calculates the moment of inertia a.k.a mass in F=ma.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <param name="watts"></param>
        /// <returns></returns>
        public double CalculateInteria(double speedMps, double watts)
        {
            if (m_decelFit == null)
            {
                throw new InvalidOperationException(
                    "PowerFit must be constructed with DecelerationFit to use this method.");
            }

            // Where P = F*v, F=ma

            double f = watts / speedMps;
            double a = m_decelFit.Rate(speedMps);

            // Get the rate of deceleration (a) for a given velocity.

            // Solve for m = f/a
            Inertia = f / a;

            return Inertia;
        }

        public virtual void Fit()
        {
            if (double.IsNaN(m_inertia) || m_inertia == 0)
                throw new InvalidOperationException("Inertia must be calculated first.");

            double[,] speed;
            double[] watts;

            // Generate power : speed data.
            GeneratePowerData(out speed, out watts);
            Fit(speed, watts);
        }

        public virtual void Fit(double[] speedMph, double[] watts)
        {
            double[,] speed;
            speed = new double[speedMph.Length, 1];

            for (int i = 0; i < speedMph.Length; i++)
            {
                speed[i, 0] = speedMph[i] * 0.44704;
            }

            Fit(speed, watts);
        }

        private void Fit(double[,] speed, double[] watts)
        {
            int info;

            alglib.lsfitstate state;
            alglib.lsfitreport report;
            alglib.lsfitcreatef(speed, watts, m_coeff, 0.0001, out state);
            alglib.lsfitsetbc(state, new double[] { 0.0, 0.0 }, new double[] { 5.0, 50.0 });
            alglib.lsfitfit(state, fit_func, null, null);
            alglib.lsfitresults(state, out info, out m_coeff, out report);
        }

        public virtual void GeneratePowerData(out double[,] speed, out double[] watts)
        {
            if (m_decelFit == null)
            {
                throw new InvalidOperationException(
                    "PowerFit must be constructed with DecelerationFit to use this method.");
            }

            speed = new double[30, 1];
            watts = new double[30];

            int ix = 0;
            // Generate power data.
            foreach (var i in Enumerable.Range(5, 30))
            {
                double v = i * 0.44704;
                double a = m_decelFit.Rate(v);

                if (a == 0)
                {
                    // If we got 0 for acceleration, 0 both out.
                    speed[ix, 0] = 0;
                    watts[ix] = 0;
                }
                else
                {
                    double f = Inertia * a;

                    speed[ix, 0] = v;
                    watts[ix] = f * v;
                }

                ix++;
            }
        }

        /// <summary>
        /// Model for how power from rolling resistance and drag (no magnet) is calculated.
        /// </summary>
        /// <param name="v"></param>
        /// <param name="K"></param>
        /// <param name="rr"></param>
        /// <returns></returns>
        static double fit_drag_rr(double v, double K, double rr)
        {
            double p = (K * Math.Pow(v, 2) + rr) * v; // / contact_patch());

            return p;
        }

        static void fit_func(double[] c, double[] x, ref double func, object obj)
        {
            func = fit_drag_rr(x[0], c[0], c[1]);
        }

        static double contact_patch()
        {
            //# 'Using this method: http://bikeblather.blogspot.com/2013/02/tire-crr-testing-on-rollers-math.html
            double wheel_diameter_cm = 210.7 / Math.PI;
            double drum_diameter_cm = 26 / Math.PI;
            double patch = Math.Pow((1 / (1 + (wheel_diameter_cm / drum_diameter_cm))), 0.7);

            return patch;
        }
    }
}
