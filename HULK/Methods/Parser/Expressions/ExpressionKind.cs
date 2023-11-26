namespace HULK
{
    public enum ExpressionKind
    {
        //Most basic expressions
        Number,
        String,
        Boolean,

        //Binary expressions
            //Numeric expressions
        Adition,
        Subtraction,
        Multiplication,
        Division,
        Modulus,
        Power,

            //Boolean expressions
        And,
        Or,
        EqualTo,
        GreaterEqualThan,
        GreaterThan,
        LesserEqualThan,
        LesserThan,
        Concatenation
    }
}