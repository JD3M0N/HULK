using System.Globalization;
namespace HULK
{
    public enum TokenKind
    {
        // Basic Data Types
        Identifier,
        Number,
        String,
        Bool,

        // Punctuators
        LeftParenthesis,
        RightParenthesis,
        Semicolon,
        Comma,


        // Arithmetic Operators
        Plus,
        Minus,
        Multiply,
        Divide,
        Modulus,
        Power,
        Concatenate,

        // Boolean Operators
        EqualsTo,
        GreatherOrEquals,
        GreatherThan,
        LesserOrEquals,
        LesserThan,
        And,
        Or,
        Not,
        NotEqualsTo,

        // Assignment Operators
        Lambda,
        Equals,

        // KeyWords
        FunctionKeyWord,
        LetKeyWord,
        InKeyWord,
        IfKeyWord,
        ElseKeyWord,

        // Utility Token
        EOF
    }
}