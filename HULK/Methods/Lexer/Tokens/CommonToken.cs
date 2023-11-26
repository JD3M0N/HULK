using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public class CommonToken : Token
    {
        public override TokenKind Kind { get; set; }
        public CommonToken(TokenKind kind) : base(kind) { }
        public override object GetValue() => throw new Exception();

        public override string ToString() => $"{Kind}";
    }
}