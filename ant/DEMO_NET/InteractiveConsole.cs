﻿using System;
using System.Collections.Generic;
using ANT_Console.Services;
using ANT_Console.Messages;

namespace ANT_Console
{
    class InteractiveConsole : IReporter
    {
        bool m_inCommand = false;
        DateTime m_lastReport = DateTime.Now;
        AntBikePower m_eMotion;
        AntControl m_control;

        public InteractiveConsole(AntBikePower eMotion, AntControl control)
        {
            m_eMotion = eMotion;
            m_control = control;
        }

        public void Run()
        {
            const string header = "Time         |  mph  | Watts | Watts2| Servo";
            ConsoleKeyInfo cki;

            Console.CursorVisible = false;
            Console.WriteLine("Starting....");
            Console.WriteLine(header);

            do
            {
                cki = Console.ReadKey(true);

                switch (cki.Key)
                {
                    case ConsoleKey.U:
                        m_control.RemoteControl(0x00);
                        WriteCommand("Sent UP command.");
                        break;

                    case ConsoleKey.D:
                        m_control.RemoteControl(0x01);
                        WriteCommand("Sent DOWN command.");
                        break;

                    case ConsoleKey.S:
                        m_control.RemoteControl(0x02);
                        WriteCommand("Sent SELECT command.");
                        break;

                    case ConsoleKey.W:
                        SetWeightCommand();
                        break;

                    case ConsoleKey.M:
                        MoveServoCommand();
                        break;

                    case ConsoleKey.H:
                        ShowHelp();
                        break;

                    case ConsoleKey.F:
                        m_eMotion.SetFirmwareUpdateMode();
                        WriteCommand("Enabling DEVICE FIRMWARE UPDATE MODE...");
                        break;

                    case ConsoleKey.X:
                        WriteCommand("Exiting...");
                        break;

                    case ConsoleKey.V:
                        WriteCommand(string.Format("Firmware version: {0}.{1}", 
                            m_eMotion.FirmwareVerMajor, 
                            m_eMotion.FirmwareVerMinor));
                        break;

                    default:
                        WriteCommand("Unrecognized command.");
                        ShowHelp();
                        break;
                }
            } while (cki.Key != ConsoleKey.X);
        }

        public void Report(DataPoint data)
        {
            if (m_inCommand)
                return;

            string format = "{0:H:mm:ss.fff} | {1,5:N1} | {2,5:N0} | {3,5:N0} | {4,6:N0}";

            // Leave 2 rows at the bottom for command.
            int lastLine = Console.CursorTop;

            // Do we need to scroll?
            if (lastLine > Console.WindowHeight - 2)
            {
                // Buffer space to scroll? 
                if ((Console.WindowTop + Console.WindowHeight) >= Console.BufferHeight - 1)
                {
                    // We're out of buffer space, so flush.
                    Console.MoveBufferArea(Console.WindowLeft,
                        Console.WindowTop,
                        Console.WindowWidth,
                        Console.WindowHeight,
                        Console.WindowLeft,
                        0);

                    Console.SetWindowPosition(Console.WindowLeft, 0);
                    lastLine = Console.WindowHeight - 1;
                }
                else
                {
                    // Scroll
                    Console.SetWindowPosition(Console.WindowLeft, Console.WindowTop + 1);
                }
            }

            // Position the cursor at the bottom of the screen to write the command line.
            Console.SetCursorPosition(Console.WindowLeft, Console.WindowTop + Console.WindowHeight - 1);
            Console.Write("<enter cmd>");
            Console.SetCursorPosition(Console.WindowLeft, lastLine);
            Console.WriteLine(format,
                DateTime.Now,
                data.SpeedEMotion,
                data.PowerEMotion,
                data.PowerReference,
                data.ServoPosition);
        }

        void ShowHelp()
        {
            m_inCommand = true;

            ConsoleColor color = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Green;

            Console.WriteLine("{KEY} [Command]\n" +
                "H [Help]\n" +
                "W [Set Weight]\n" +
                "U [Send Up Command]\n" +
                "D [Send Down Command]\n" +
                "S [Send Select Command]\n" +
                "M [Move Servo to position X]\n" +
                "F [Enable Device Firmware Update Mode]\n" +
                "V [Display Firmware Version]\n" +
                "X [Exit]");

            Console.ForegroundColor = color;

            m_inCommand = false;
        }

        bool InteractiveCommand(string prompt, Func<bool> Parser)
        {
            m_inCommand = true;

            // Propmt user for weight in lbs
            Console.SetCursorPosition(Console.WindowLeft, Console.WindowTop + Console.WindowHeight - 1);
            ConsoleColor lastColor = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Green;
            Console.Write(prompt);
            Console.ForegroundColor = lastColor;
            Console.SetCursorPosition(prompt.Length + 2, Console.CursorTop);
            Console.CursorVisible = true;

            bool result = Parser();

            Console.CursorVisible = false;
            m_inCommand = false;

            return result;
        }

        void MoveServoCommand()
        {
            string prompt = "<enter servo position:>";
            int position = 0;
            bool success = InteractiveCommand(prompt, () =>
            {
                if (int.TryParse(Console.ReadLine(), out position))
                {
                    if (position < 699 || position > 2107)
                    {
                        WriteCommand("Servo position should be > 699 and < 2107.");
                        return false;
                    }
                }
                else
                {
                    WriteCommand("Invalid servo position.");
                    return false;
                }

                return true;
            });

            if (success)
            {
                m_eMotion.MoveServo(position);
                WriteCommand(string.Format("Moving servo to {0}.", position));
            }
        }

        void SetWeightCommand()
        {
            string prompt = "<enter weight in lbs:>";
            float weight = 0.0f;
            bool success = InteractiveCommand(prompt, () =>
            {
                //float weight = float.NaN;

                if (float.TryParse(Console.ReadLine(), out weight))
                {
                    if (weight < 100 || weight > 300)
                    {
                        WriteCommand("Weight in lbs should be > 100 and < 300.");
                        return false;
                    }
                }
                else
                {
                    WriteCommand("Invalid weight value.");
                    return false;
                }

                return true;
            });

            if (success)
            {
                // Convert lb to kg.
                float weightKg = weight / 2.2f;

                m_eMotion.SetWeight(weightKg);
                WriteCommand(string.Format("Set Weight to {0:N1}kg.", weightKg));
            }
        }

        void WriteCommand(string message)
        {
            m_inCommand = true;

            ConsoleColor last = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine(message);
            Console.ForegroundColor = last;

            m_inCommand = false;
        }

        public void Report2(DataPoint data)
        {
            if (data.Timestamp > m_lastReport)
            {
                Console.WriteLine(data);
                m_lastReport = data.Timestamp;
            }
        }
    }
}