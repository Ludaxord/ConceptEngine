using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConceptEngineCSharp
{
    public static class CEConsumer
    {
        //Debug
        public static CECSPlayground GetPlayground()
        {
            return new CECSPlayground();
        }

        public static void GeneratePlayground()
        {
            Console.WriteLine("Generating New Playground...");
        }

        public static void CompilePlayground()
        {
            Console.WriteLine("Compiling Playground...");
        }

        public static void CleanPlayground()
        {
            Console.WriteLine("Cleaning Playground...");
        }

        public static void GenerateProject()
        {
            Console.WriteLine("Generating New Project...");
        }

        public static void CompileProject()
        {
            Console.WriteLine("Compiling Project...");
        }

        public static void CleanProject()
        {
            Console.WriteLine("Cleaning Project...");
        }
    }
}