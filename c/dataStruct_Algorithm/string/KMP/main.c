#include <stdio.h>
#include <stdlib.h>

/*
**************************************************
  BF algorithm
--------------------------------------------------
arguments :
    s       : search  string
    pattern : pattern string
--------------------------------------------------
return :
         the 1st position that pattern matches inside the search string (the returned index starts from 0) 
         if not found , return -1

**************************************************
*/
int BruteForceSearch(const char* src, const char* pattern)
{
    int retIdx = -1; // assign as not found flag first
    int i = 0;
    int j = 0;
    
    while ( src[i] != '\0'   &&   pattern[j] != '\0' )
    {
        if ( src[i] == pattern[j] ) {
            ++i;
            ++j;
        } else {
            // (i - j)        :  the position which pattern's 1st character is align with search's match head
            // (i - j) + 1    :  the position next to    the head of the matched src
            i = ( i - j ) + 1;
            // jump back to the head pointer of the pattern string
            j = 0; 
        }
    }

    // travesal till the terminate character ('\0') of the pattern string
    if ( src[0] != '\0'   &&   pattern[j] == '\0' ) {
        retIdx = i - j;
    }

    return retIdx;
}







/*
**************************************************
  KMP (Knuth-Morris-Pratt) algorithm
**************************************************
*/
void calcNextArray(const char* pattern, int next[], int aryLen)
{
    int i = 0;
    int k = -1;
    next[0] = -1;
    while ( i < (aryLen-1) )
    {
        if ( k == -1 || pattern[i] == pattern[k] )
        {
            ++i;
            ++k;
            next[i] = k;
        }
        else
        {
            k = next[k];
        }
    }

}

void calcNextValArray(const char* pattern, int nextval[], int aryLen)
{
    int i = 0;
    int k = -1;
    nextval[0] = -1;

    /*  
    compare with calcNextArray(...)

    while ( i < (aryLen-1) )   is an ERROR compare expression  */
    while ( i <  aryLen    )
    {
        if ( k == -1 || pattern[i] == pattern[k] )
        {
            ++i;
            ++k;
            if ( pattern[k] != pattern[i] ) {
                nextval[i] = k;
            } else {
                nextval[i] = nextval[k];
            }
        }
        else
        {
            k = nextval[k];
        }
    }

}


int KMPSearch_NextArray(const char* src, const char* pattern)
{
    int retIdx = -1; // assign as not found flag first
    int patternLen = 0;
    while ( pattern[patternLen] != '\0' ) {
        ++patternLen;
    }

    int* nextAry = (int*)malloc( sizeof(int) * patternLen);
    calcNextArray(pattern, nextAry, patternLen);

    int i = 0;
    int j = 0;

    while ( src[i] != '\0'   &&   pattern[j] != '\0' )
    {
        if ( j == -1    ||  src[i] == pattern[j] ) {
            ++i;
            ++j;
        } else {
            // jump back to the head pointer of the pattern string
            j = nextAry[j]; 
        }
    }

    // travesal till the terminate character ('\0') of the pattern string
    if ( pattern[j] == '\0' ) {
        retIdx = i - j;
    }

    free(nextAry);
    nextAry = NULL;
    return retIdx;
}


int KMPSearch_NextValueArray(const char* src, const char* pattern)
{
    int retIdx = -1; // assign as not found flag first
    int patternLen = 0;
    while ( pattern[patternLen] != '\0' ) {
        ++patternLen;
    }

    int* nextValue_Array = (int*)malloc( sizeof(int) * patternLen);
    calcNextValArray(pattern, nextValue_Array, patternLen);

    int i = 0;
    int j = 0;

    while ( src[i] != '\0'   &&   pattern[j] != '\0' )
    {
        if ( j==-1  ||  src[i] == pattern[j] ) {
            ++i;
            ++j;
        } else {
            // jump back to the head pointer of the pattern string
            j = nextValue_Array[j]; 
        }
    }

    // travesal till the terminate character ('\0') of the pattern string
    if ( pattern[j] == '\0' ) {
        retIdx = i - j;
    }

    free(nextValue_Array);
    nextValue_Array = NULL;
    return retIdx;
}


int main(int argc, char* argv[])
{
    if ( argc != 3 ) {
        printf("[ERROR] : Missing 2 arguments. \n");
        return -1;
    }

    const char* searchString  = argv[1];
    const char* patternString = argv[2];

    int foundPos = 0;   // BruteForce_1(searchString, patternString);
    
    printf("[TEST] : foundPos = %d , found \"\" from \"\"\n", BruteForceSearch("",""));

    printf("[TEST] : foundPos = %d , found \"a\" from \"\"\n", BruteForceSearch("","a"));
    printf("[TEST] : foundPos = %d , found \"ab\" from \"\"\n", BruteForceSearch("","ab"));

    printf("[TEST] : foundPos = %d , found \"\" from \"a\"\n", BruteForceSearch("a",""));
    printf("[TEST] : foundPos = %d , found \"\" from \"ab\"\n", BruteForceSearch("ab",""));

    printf("[TEST] : foundPos = %d , found \"abc\" from \"ab\"\n", BruteForceSearch("ab","abc"));
    printf("[TEST] : foundPos = %d , found \"abc\" from \"adacab\"\n\n", BruteForceSearch("adacab","abc"));

    foundPos = BruteForceSearch(searchString, patternString);
    if ( foundPos == -1 ) {
        printf("[INFO] : Can't found \"%s\" inside \"%s\"\n", patternString, searchString );
    } else {
        printf("[INFO] : Found pattern \"%s\" inside \"%s\" at position [%d]\n", patternString, searchString, foundPos );
    }

    return 0;
}
