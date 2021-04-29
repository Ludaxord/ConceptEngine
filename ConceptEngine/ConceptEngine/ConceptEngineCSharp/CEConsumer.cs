﻿using System;

namespace ConceptEngineCSharp
{
    public class CEConsumer
    {
        public static CECSPlayground GetPlayground()
        {
            return new CECSPlayground();
        }

        [DllExport]
        public static void GeneratePlayground()
        {
            Console.WriteLine("Generating New Playground...");
        }

        [DllExport]
        public static void CompilePlayground()
        {
            Console.WriteLine("Compiling Playground...");
        }

        [DllExport]
        public static void CleanPlayground()
        {
            Console.WriteLine("Cleaning Playground...");
        }

        [DllExport]
        public static void GenerateProject()
        {
            Console.WriteLine("Generating New Project...");
        }

        [DllExport]
        public static int CompileProject(int x, int y)
        {
            Console.WriteLine("Compiling Project...");
            return x * y;
        }

        [DllExport]
        public static void CleanProject()
        {
            Console.WriteLine("Cleaning Project...");
        }
    }
}