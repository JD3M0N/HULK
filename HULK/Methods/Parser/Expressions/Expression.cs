using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public abstract class Expression
    {
        abstract public ExpressionKind Kind { get; set; }
        abstract public object Value { get; set; }

        public abstract void Evaluate();

        public abstract object GetValue();
    }
}