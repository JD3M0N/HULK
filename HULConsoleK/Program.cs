using System.Runtime.CompilerServices;
using HULK;

namespace HULK
{
    public class ConsoleInterface
    {
        static void Main(string[] args)
        {
            Auto();

           
        }

        void Run(){
             System.Console.WriteLine("WELCOME TO HULK, Havana University Laguage for Kompilers.");
            System.Console.WriteLine("Please enter your input!");
            System.Console.Write(">");

        }

        static void Auto()
        {
            string[] tests = {""};

            for (int i = 0; i < tests.Length; i++)
            {
                Lexer lexer = new Lexer(tests[i]);
                List<Token> tokens = lexer.Tokenizer();
                Console.WriteLine(string.Join('\n',tokens));
            }

        }
    }

}
