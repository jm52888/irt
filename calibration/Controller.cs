﻿using System;
using System.Linq;
using AntPlus.Profiles.BikePower;
using System.IO;
using IRT.Calibration.Globals;

namespace IRT.Calibration
{
    /// <summary>
    /// Orchestrates and manages the state of the calibration workflow end to end.
    /// </summary>
    public class Controller
    {
        BikePowerDisplay m_emotionPower, m_refPower;
        Model m_model;
        Coastdown m_coastdown;

        // Forms
        CoastdownForm m_coastdownForm;
        CalibrationForm m_calibrationForm;

        public event Action<Coastdown> SetCalibrationValues;
        public event Action<Globals.Stage> StageChanged;

        public Controller()
        {
            Initialize();
        }

        /// <summary>
        /// Creates the controller, should be created on the UI Thread.
        /// </summary>
        /// <param name="emotionPower"></param>
        /// <param name="refPower"></param>
        public Controller(BikePowerDisplay emotionPower, BikePowerDisplay refPower) : this()
        {
            // Listeners for ANT+ events.
            m_emotionPower = emotionPower;
            m_refPower = refPower;

            if (m_emotionPower != null)
            {
                // Register calibration events from the emotion rollers.
                m_emotionPower.CalibrationCustomParameterResponsePageReceived +=
                    m_emotionPower_CalibrationCustomParameterResponsePageReceived;
                m_emotionPower.GeneralCalibrationResponseSuccessReceived +=
                    m_emotionPower_GeneralCalibrationResponseSuccessReceived;
                m_emotionPower.GeneralCalibrationResponseFailPageReceived +=
                    m_emotionPower_GeneralCalibrationResponseFailPageReceived;
            }

            if (m_refPower != null)
            {
                // Register standard power messages from the external power meter.
                m_refPower.StandardPowerOnlyPageReceived += m_refPower_StandardPowerOnlyPageReceived;
            }
        }

        public Stage Stage 
        { 
            get { return m_model.Stage; }
            private set
            {
                m_model.Stage = value;
                if (StageChanged != null)
                {   
                    StageChanged(value);
                }
            }
        }

        public Model Model { get { return m_model; } }

        /// <summary>
        /// Displays the main calibration form during coast down.
        /// </summary>
        public void DisplayCalibrationProgress()
        {
            m_calibrationForm = new CalibrationForm(m_model);
            m_calibrationForm.Show();
        }

        /// <summary>
        /// Closes the main calibration form if open and displays the calibration results.
        /// </summary>
        public void DisplayCalibrationResults()
        {
            if (m_calibrationForm != null && !m_calibrationForm.IsDisposed)
            {
                // Close calibration form.
                m_calibrationForm.Close();
            }

            // Open form to show results.
            m_coastdownForm = new CoastdownForm(m_coastdown);
            m_coastdownForm.Show();
        }

        private void Initialize()
        {
            m_model = new Model();
            m_model.Stage = Stage.Ready;
            m_coastdown = new Coastdown();
        }

        void m_refPower_StandardPowerOnlyPageReceived(StandardPowerOnlyPage arg1, uint arg2)
        {
            OnPowerEvent(arg1.EventCount, arg1.AccumulatedPower);
        }

        void m_emotionPower_GeneralCalibrationResponseFailPageReceived(
            GeneralCalibrationResponseFailPage arg1, uint arg2)
        {
            OnFail("General calibration fail, please retry.");
        }

        void m_emotionPower_GeneralCalibrationResponseSuccessReceived(
            GeneralCalibrationResponseSuccessPage arg1, uint arg2)
        {
            if (m_model.Stage == Stage.Coasting)
            {
                OnProcessing();
            }
            else
            {
                // Raise an event that we ended prematurely.
                OnFail("Premature exit, please retry.");
            }
        }

        void m_emotionPower_CalibrationCustomParameterResponsePageReceived(
            CustomCalibrationParameterResponsePage arg1, uint arg2)
        {
            // Generates 2 events from the buffer.
            TickEvent[] events = TickEvent.FromBuffer(arg1.CalibrationDataArray.ToArray());
            
            for (int i = 0; i < 2; i++)
            {
                OnCalibrationEvent(events[i]);
            }

        }

        /// <summary>
        /// Add tick events to the model which recalculates state.
        /// </summary>
        /// <param name="tickEvent"></param>
        private void OnCalibrationEvent(TickEvent tickEvent)
        {
            m_model.AddSpeedEvent(tickEvent);

            if (m_calibrationForm != null && !m_calibrationForm.IsDisposed)
            {
                m_calibrationForm.UpdateValues(m_model.StableSeconds,
                    m_model.SpeedMps * 2.23694,
                    (ushort)m_model.StableWatts,
                    m_model.Motion);
            }

            // 
            // Determine stage and process state transitions.
            //
            if(m_model.Stage == Stage.Ready)
            {
                OnStarted();
            }
            else if (m_model.Stage == Stage.Started &&
                m_model.Motion == Motion.Stable &&
                m_model.StableSeconds >= Settings.StableThresholdSeconds)
            {
                OnStable();
            }
            else if (m_model.Stage == Stage.Stable &&
                m_model.Motion == Motion.Accelerating)
            {
                OnAccelerating();
            }
            else if (m_model.Stage == Stage.Accelerating &&
                m_model.SpeedMps >= Settings.MinAccelerationSpeedMps)
            {
                OnSpeedThresholdReached();
            }
            else if (m_model.Stage == Stage.SpeedThresholdReached &&
                m_model.Motion == Motion.Decelerating)
            {
                OnCoasting();
            }
        }

        private void OnPowerEvent(int eventCount, ushort accumPower)
        {
            m_model.AddPowerEvent(eventCount, accumPower);
        }



        /*
         * Manage state transitions.
         * 
         */

        private void OnStarted()
        {
            this.Stage = Stage.Started;
        }

        private void OnStable()
        {
            // Called when we've reached stable speed for threshold time.
            this.Stage = Stage.Stable;

            // Indicate to user it's time to accelerate to threshold speed.
        }

        private void OnAccelerating()
        {
            // Called when we end the acceleration stage.
            this.Stage = Stage.Accelerating;

            // Stop recording power events.
        }

        private void OnSpeedThresholdReached()
        {
            this.Stage = Stage.SpeedThresholdReached;
        }

        private void OnCoasting()
        {
            this.Stage = Stage.Coasting;
        }

        private void OnProcessing()
        {
            this.Stage = Stage.Processing;
            
            // Kick off processing.
            try
            {
                m_coastdown.Calculate(m_model);
                OnFinished();
            }
            catch (Exception e)
            {
                OnFail(e.Message);
            }
        }

        private void OnFinished()
        {
            this.Stage = Stage.Finished;

            if (SetCalibrationValues != null)
            {
                // Kick off a timer, countdown from n seconds and then invoke.
                //SetCalibrationValues(m_coastdown);
            }
        }

        private void OnFail(string message)
        {
            this.Stage = Stage.Failed;

            // reset
            Initialize();
        }

        /// <summary>
        /// Closes the dialogs and resets the state.
        /// </summary>
        public void Cancel()
        {
            if (m_calibrationForm != null && !m_calibrationForm.IsDisposed)
            {
                // Execute on the UI thread.
                Action a = () =>
                {
                    m_calibrationForm.Close();
                };

                if (m_calibrationForm.InvokeRequired)
                {
                    m_calibrationForm.BeginInvoke(a);
                }
                else
                {
                    m_calibrationForm.Invoke(a);
                }
            }
        }
    }
}
