using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public class String : BasicExpression
    {
        public override ExpressionKind Kind { get; set; }
        public override object Value { get; set; }

        public String(string value)
        {
            Value = value;
            Kind = ExpressionKind.String;
        }
        public override void Evaluate()
        {
            throw new NotImplementedException();
        }

        public override object GetValue() => Value;
    }
}