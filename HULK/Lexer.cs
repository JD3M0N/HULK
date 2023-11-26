
using System.Text;
using System.ComponentModel.DataAnnotations;
using System.Data;

namespace HULK
{

    public class Lexer
    {
        //The variable "input" is the sentence that is going to be split or scanned.
        public string Input;
        //The following list is the splited list once the main method of  the Lexer is used.
        public List<string> SemiTokens = new List<string>();

        //This is the list of stop keys, the character that means its the end of a string, ready to tokemize
        public List<char> StopKeys = new List<char>(); //Works!

        //Makin' the constructor
        public Lexer(string input)
        {
            Input = input;

            #region Making the list
            List<char> stopKeys = new List<char>();
            stopKeys.Add(' ');
            stopKeys.Add('(');
            stopKeys.Add('+');
            stopKeys.Add('=');
            stopKeys.Add('-');
            stopKeys.Add('*');
            stopKeys.Add('^');
            stopKeys.Add(';');
            //stopKeys.Add('');
            #endregion

            StopKeys = stopKeys;
        }

        public void Spliter()
        {
            System.Console.WriteLine("Using Spliter");
            //I'll use 3 index since this laguange is LL(1)
            //The starter index(s), the actual(a) index and the next one(n)
            int s = 0;
            int n = 1;

            string temporal = "";

            for (int a = 0; a < Input.Length - 1; a++, n++)
            {
                if (Input[n] == ' ')
                {
                    temporal = Input.Substring(s, a - s + 1);
                    SemiTokens.Add(temporal);
                    while(Input[n] == ' ' || Input[a] == ' ')
                    {
                        n++;
                        a++;
                    }
                    s = a;
                    continue;
                }
                if (Input[a] == ' ')
                {
                    continue;
                }
                if (StopKeys.Contains(Input[a]))
                {
                    temporal = Input.Substring(a, a - s + 1);
                    continue;
                }
                if (StopKeys.Contains(Input[n]))
                {
                    temporal = Input.Substring(s, a - s);
                    SemiTokens.Add(temporal);
                    s = n;
                }
            }

            temporal = Input.Substring(s, Input.Length - s);
            SemiTokens.Add(temporal);
        }
    }
}