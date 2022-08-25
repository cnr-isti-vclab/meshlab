using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using LibUsbDotNet.Main;
using LibUsbDotNet;
using OpenKinect;
using System.Threading;

namespace OpenKinectConsole
{
    class OpenKinectConsole
    {
        static void Usage()
        {
            Console.WriteLine(
               "Usage:\n  " +
                System.IO.Path.GetFileName(Environment.GetCommandLineArgs()[0]) +
                " [--horizon | (pos)]"
            );
        }

        static void Main(string[] args)
        {
            KinectMotor motor = new KinectMotor();

            if( args.Length ==1 )
            {
                if( args[0] == "--horizon" )
                {
                    // Demonstrate horizon following
                    Console.WriteLine("Press any key to exit");
                    while(Console.KeyAvailable == false)
                    {
                        motor.SetTilt(0);
                        Thread.Sleep(TimeSpan.FromMilliseconds(100));
                    }
                }else{
                    // User defined value
                    sbyte pos = sbyte.Parse(args[0]);
                    motor.SetTilt(pos);
                }
            }else{
                Usage();

                Console.WriteLine("Demo sequence started");

                // Sample Sequence
                ExerciseMotor(motor,+50);
                ExerciseMotor(motor,-50);
                ExerciseMotor(motor,+60);
                ExerciseMotor(motor,-60);
                ExerciseMotor(motor, 0);
            }

            Console.WriteLine("done");
        }

        private static void ExerciseMotor(KinectMotor motor, sbyte pos)
        {

            motor.SetLED(KinectLEDStatus.AlternateRedYellow);
            motor.SetTilt(pos);
            Thread.Sleep(TimeSpan.FromSeconds(2));
            motor.SetLED(KinectLEDStatus.Green);
        }
    }
}
