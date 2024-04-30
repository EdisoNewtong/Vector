using System;

namespace CSharp_KeyWordList
{
    class KK {
        public static void List_KeyWords() 
        {
            string[] keyWordsArray = [
                                        "abstract", "as", "base", "bool", "break",
                                        "byte", "case", "catch", "char", "checked",
                                        "class", "const", "continue", "decimal", "default",
                                        "delegate", "do", "double", "else", "enum",
                                        "event", "explicit", "extern", "false", "finally",
                                        "fixed", "float", "for", "foreach", "goto", 
                                        "if", "implicit", "in", "int", "interface",
                                        "internal", "is", "lock", "long", "namespace",
                                        "new", "null", "object", "operator", "out",
                                        "override", "params", "private", "protected", "public",
                                        "readonly", "record", "ref", "return", "sbyte",
                                        "sealed", "short", "sizeof", "stackalloc", "static",
                                        "string", "struct", "switch", "this", "throw",
                                        "true", "try", "typeof", "uint", "ulong",
                                        "unchecked", "unsafe", "ushort", "using", "virtual",
                                        "void", "volatile", "while" 
            ];

            Console.WriteLine("---------- C# Key Words List : ----------");
            for ( int i = 0; i < keyWordsArray.Length; ++i ) {
                Console.WriteLine("\t{0}.  {1}", (i+1), keyWordsArray[i] );
            }
            Console.WriteLine("--------------------------------------------------");
        }
    }

}
