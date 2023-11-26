using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public class Number : BasicExpression
    {
        public override ExpressionKind Kind { get; set; }
        public override object Value { get; set; }

        public Number(double value)
        {
            Value = value;
            Kind = ExpressionKind.Number;
        }
        public override void Evaluate()
        {
            return;
        }

        public override object GetValue() => Value;
    }
}