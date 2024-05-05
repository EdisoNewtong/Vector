// See https://aka.ms/new-console-template for more information

using System;

namespace MyApp {
    class Program {
        static void Main(string[] args) {
            CSharp_KeyWordList.KK.List_KeyWords();

            /** 
                    The following statement is valid since .net 8.0

                typeName[]  aryName = [ value1, value2, value3,   ... , valueN ];

                 ** But ** It's Not valid before .net 8.0




                    The following statement is valid  Downward Compatibility

                typeName[]  aryName = { value1, value2, value3, ... , valueN };

            */
            int[] array1 = [ 1,    2,  3 ];  // [ ... ]
            int[] array2 = { 100,200,300 };  // { ... }

            Console.WriteLine("---------- Array1 ----------");
            for( int i = 0; i < array1.Length; ++i ) {
                Console.WriteLine("\t{0}. {1}", (i+1), array1[i]);
            }
            Console.WriteLine("");

            Console.WriteLine("---------- Array2 ----------");
            for( int i = 0; i < array2.Length; ++i ) {
                Console.WriteLine("\t{0}. {1}", (i+1), array2[i]);
            }
        }
    }
}



