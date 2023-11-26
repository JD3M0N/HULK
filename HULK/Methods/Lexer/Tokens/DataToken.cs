using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public class DataToken : Token
    {
        // Number string bool
        public override TokenKind Kind { get; set; }
        private object? Value { get; set; }


        public DataToken(TokenKind kind, object? value) : base (kind)
        {
            Value = value;
        }

        public override object GetValue() => Value!;

        public override string ToString() => $"{Kind} : {Value}";
    }
}