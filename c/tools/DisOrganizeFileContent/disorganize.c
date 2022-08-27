#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define spaceCnt (11)
#define nzc (7)
#define zc (3)


typedef struct charNode
{
    char ch;
    unsigned long long index;
    struct charNode* next;
} charNode;

typedef struct flag
{
    unsigned long long index;
    int used; // 0:unused , 1:used
} flag;


int getFileContent(const char* filename, char** pContent, long int* pContentLength)
{
    FILE* fObj = fopen(filename, "rb");
    if ( fObj == NULL ) {
        fprintf(stderr,"[ERROR] : Can't open file \"%s\" or file is not existed.\n", filename);
        return -1;
    }

    if ( fseek(fObj, 0L, SEEK_END) != 0 ) {
        fclose(fObj);
        fprintf(stderr,"[ERROR] : Can't seek to the end of file \"%s\"\n", filename);
        return -1;
    }
    
    long int filelen = ftell(fObj);
    fprintf(stderr,"filelen = %ld\n", filelen);

    // move the file position indicator back to the file's beginning
    rewind(fObj);
    if ( filelen == 0L ) {
        if ( pContentLength != NULL ) { 
            *pContentLength = 0L; 
        }
        fclose(fObj);
        fprintf(stderr,"[INFO] : file \"%s\" has 0 byte , is empty\n", filename);
        return 1;
    }

    char* fileBuf = (char*)malloc( filelen * sizeof(char) );
    if ( fileBuf == NULL ) {
        fclose(fObj);
        fprintf(stderr,"[ERROR] : Alloc %ld memory failed\n", filelen);
        return -1;
    }

    size_t succLen = 0;
    if ( ( succLen = fread(fileBuf, sizeof(char), filelen, fObj) ) != filelen ) {
        fclose(fObj);
        fprintf(stderr, "[ERROR] : read file buffer failed , successful read: %d , required read: %ld\n", succLen, filelen);
        return -1;
    }


    if ( pContent != NULL    &&  pContentLength != NULL ) {
        *pContent = fileBuf;
        *pContentLength = filelen;
    }

    fclose(fObj);
    fObj = NULL;

    return 1;
}

charNode* generateDisOrganizedLinkTable(char* pFileContent, unsigned long filelen)
{
    charNode* head = (charNode*)malloc( sizeof(charNode) );
    head->ch = 0;
    head->index = 0L;
    head->next = NULL;

    // init
    flag* flagAry = (flag*)malloc( filelen * sizeof(flag) );
    for( unsigned long long idx = 0; idx < filelen; ++idx ) {
        flagAry[idx].index = idx;
        flagAry[idx].used = 0;
    }

    // random seed
    srand( (unsigned int)time(NULL) );

    unsigned long errorCnt = 0l;
    charNode* curNode = head;
    for( unsigned long time = 0; time < filelen; ++time ) {
        unsigned long remain       = filelen - time;
        int           lastValidIdx = remain - 1;

        int randomIdx = rand() % remain;
        charNode* newNode = malloc( sizeof(charNode) );
        unsigned long long realIdx = flagAry[randomIdx].index;
        if ( flagAry[randomIdx].used == 1 ) {
            printf("%ld.  time = %ld, meet an error.\n", errorCnt+1, time);
            ++errorCnt;
        }

        newNode->ch = pFileContent[realIdx];
        newNode->index = ((unsigned long long)realIdx) & 0xFFFFFFFFULL;
        newNode->next  = NULL;

        // make chain
        curNode->next = newNode;
        curNode = newNode;


        // update array element state
        flagAry[randomIdx].used = 1;

        // swap 2 slot
        if ( randomIdx != lastValidIdx ) {
            unsigned long long unOccupiedIdx = flagAry[lastValidIdx].index;
            flagAry[lastValidIdx].index = flagAry[randomIdx].index;
            flagAry[lastValidIdx].used  = 1;

            flagAry[randomIdx].index = unOccupiedIdx;
            flagAry[randomIdx].used  = 0;
        } 
    }


    free(flagAry);

    return head;
}

char generate1hex()
{
    char cAry[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'A', 'B', 'C', 'D', 'E', 'F' };
    int idx = rand() % ( sizeof(cAry)/sizeof(cAry[0]) );
    return cAry[idx];
}

void createSourceCode(charNode* head, unsigned long flen)
{
// const int spaceCnt=11;
// const int nzc=7;const int zc=3;

/*    

#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
typedef struct charNode{char ch;unsigned long long index;struct charNode* next;}charNode;
void printContent(){int flg[]={10 nums};//7*?,3*0
charNode* h=NULL;charNode* p=NULL;charNode* c=NULL;
 c=(charNode*)malloc(sizeof(charNode));c->ch=0x??;c->index=0x??;c->next=NULL;          p=c; h=c; // 
 c=(charNode*)malloc(sizeof(charNode));c->ch=0x??;c->index=0x??;c->next=NULL;p->next=c;p=c;      // 10 * malloc(...)
 c=(charNode*)malloc(sizeof(charNode));c->ch=0x??;c->index=0x??;c->next=NULL;p->next=c;p=c;      // 10 * malloc(...)
 c=(charNode*)malloc(sizeof(charNode));c->ch=0x??;c->index=0x??;c->next=NULL;p->next=c;p=c;      // 10 * malloc(...)
 c=(charNode*)malloc(sizeof(charNode));c->ch=0x??;c->index=0x??;c->next=NULL;p->next=c;p=c;      // 10 * malloc(...)
                                                                                               
        ...  ...                                                                                             


 charNode** orderary = (charNode**)malloc( sizeof(charNode*) * flen );
 c=h;
 while(c!=NULL) {
    orderary[c->index] = c;
    c=c->next;
 }

 char* fileContent=(char*)malloc( (flen+1) * sizeof(char) );
 for( unsigned long long i=0; i<flen; ++i) {
    fileContent[i]=orderary[i].ch;
 }
 fileContent[flen] = '\0';
 printf("%s\n", fileContent);

 // free link-table
 c=h;
 while(c!=NULL) {
   charNode* nxt=c->next; 
   free(c);
   c=nxt;
 }

 free(orderary)
 free(fileContent);

}


int main(int argc, char* argv[]){
time_t n=time(NULL);
int s=0xc1f;
struct tm* ns=gmtime(&n);
int res=ns->tm_min;if(argc>1)printf("%d\n",res);res=(res==(0121/0x28)?0x3c:(res==(0xFF%2)?073:(res==00?((7<<3)|2):res-2)));
 fprintf(stderr,"Please input pwd:");n-=0170;ns=gmtime(&n);scanf("%d",&s);
 if(ns->tm_min==res)printContent();else fprintf(stderr,":(\n");
 return 0;
}

*/

    printf(
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"#include <time.h>\n"
"#include <string.h>\n"
"#define prompt 6\n");

    printf("typedef struct charNode{char ch;unsigned long long index;struct charNode* next;}charNode;\n");
    printf("void printContent(int keytag,int won){int flg[]={");
    int cnt1=0;int cnt0=0;
    int nzeroIdxAry[nzc] = { 0 };
    int  zeroIdxAry[zc]  = { 0 };
    const char* spaceAry[] = { "", 
                               " ", 
                               "  ", 
                               "   ", 
                               "    ",
                               "     ",
                               "      ",
                               "       ",
                               "        ",
                               "         ",
                               "          "
    };

    int tag=0;
    int pOrN=0;
    int num=0;
    for(int i=0;i<(nzc+zc);++i)
    {
        tag=rand()%2;
        if ( tag ) {
            if (cnt1>=nzc){tag = 0;}
        } else {
            if (cnt0>=zc) {tag = 1;}
        }

        pOrN=rand()%2;
        if( tag ) {
            nzeroIdxAry[cnt1]=i;
            do {
                num = rand();
            } while( num==0 );

            if ( pOrN == 1 ) {
                num *= (-1);
            }
            printf("%d",num);
            ++cnt1;
        }else{
            zeroIdxAry[cnt0]=i;
            printf("0");
            ++cnt0;
        }

        if ( i < (nzc+zc-1) ) {
            printf(",");
        }
    }
    printf("};\n");


    printf("charNode* h=NULL;charNode* p=NULL;charNode* c=NULL;\n");
    int lineIdx = 0;
    int fst = 0;int defsz = 0;int conditionFlg = 0;
    cnt1=0; cnt0=0;
    charNode* curnode = head->next;
    while( curnode!=NULL )
    {
        int spaceIdx = rand() % spaceCnt;
        if ( fst == 0 ) {
            printf("%sif(flg[0x%x]){c=(charNode*)malloc(sizeof(charNode));c->ch=(char)0x%x;c->index=0x%I64x;c->next=NULL;p=c;h=c;}",spaceAry[spaceIdx], nzeroIdxAry[rand()%nzc],  (int)( ((int)curnode->ch) & 0xFF), curnode->index);
            ++cnt1;
            tag = 1;
            fst = 1;
        } else {
            tag = rand()%2;
            if(tag) {
                if(cnt1>=nzc) { tag=0; }
            } else {
                if(cnt0>=zc)  { tag=1; }
            }

            if( tag ){
                printf("if(flg[0x%x]){c=(charNode*)malloc(sizeof(charNode));c->ch=(char)0x%x;c->index=0x%I64x;c->next=NULL;p->next=c;p=c;}", nzeroIdxAry[rand()%nzc], (int)( ((int)curnode->ch) & 0xFF), curnode->index);
                ++cnt1;
            } else {
                // insert dummy
                printf("if(flg[0x%x]){c=(charNode*)malloc(sizeof(charNode));c->ch=(char)0x%c%c;c->index=0x%c%c;c->next=NULL;p->next=c;p=c;}",  zeroIdxAry[rand()%zc], generate1hex(), generate1hex(), generate1hex(), generate1hex() );
                ++cnt0;
                if (!defsz && cnt0 == 1){ printf(" const int fsz=%ld; ",flen);defsz=1;}
                if ( lineIdx==7 && !conditionFlg){printf(" if(((keytag+(0x20>>04))%%(017<<2))!=(won-0x2AD/(024>>2)+0x89)){c=h;while(c!=NULL){charNode* nxt=c->next;free(c);c=nxt;} fprintf(stderr,\"[ERROR] Hack the pwd check :(\");return;}"); conditionFlg=1;}
            }
        }

        if ( (cnt0+cnt1) == (nzc+zc) ) {
            cnt1=0;
            cnt0=0;
            ++lineIdx;
            printf("\n");
            printf("%s", spaceAry[spaceIdx]);
        }

        if( tag ) {
            curnode = curnode->next;
        }
    }
    printf(" charNode** orderary=(charNode**)malloc(sizeof(charNode*)*fsz);\n");

    printf("\n");
    printf("c=h;\n"
"while(c!=NULL){orderary[c->index]=c;c=c->next;}\n"
"char* fileContent=(char*)malloc((fsz+1)*sizeof(char));\n"
"for(unsigned long long i=0;i<fsz;++i){fileContent[i]=orderary[i]->ch;}\n"
"fileContent[fsz]=\'\\0\';if((keytag%%(0x27+((1<<4)|5))) || !keytag)printf(\"%%s\\n\",fileContent);\n"
"c=h;\n"
"while(c!=NULL){orderary[c->index]=c;c=c->next;}\n"
"c=h;\n"
"while(c!=NULL){charNode* nxt=c->next;free(c);c=nxt;}\n"
"free(orderary);free(fileContent);}"
);

    printf("\n\n");

    printf("int main(int argc, char* argv[]){\n"
"int c4=0;time_t n=time(NULL);char match[prompt]={0};int f1=0;match[c4=(0x11%%(012>>1))]=(char)((3<<5)|0xF);\n"
"int e5=0;int s=0xc1f;int b2=0;match[e5=(c4+=2)]=(char)((0x35<<1)+1);int s1=0xb1E29d;\n"
"match[b2=(c4>>2)]=(char)(match[e5/2]-3);int d3=0;match[f1=(0xA/02)]=(char)(0x1C%%03-1);struct tm* ns=gmtime(&n);int a6=0;\n"
"int res=ns->tm_min;int res0=res;match[a6=(0x533%%013)]=(char)(0x21*(2|01));s1=0x1Fb8;match[d3=(c4-=1)]=(char)(0x19*4-(0x13>>4));if(!(argc>=2 && (strcmp(match,argv[1])==0))){fprintf(stderr,\":(\\n\");return -1;}(void)s1;/*-*//*2*/c4=(0100>>5);if(argc>2)printf(\"%%d\\n\",res);res=(res==(0121/0x28)?(0xA2%%066):(res==(0xFF%%0177)?073:(res==(0402%%0x2B)?((7<<3)|2):res-2)));\n"
"fprintf(stderr,\"pwd:\");scanf(\"%%d\",&s);\n"
"if(res==s)printContent(s,res0);else{fprintf(stderr,\":(\\n\");}\n"
"return 0;\n"
"}\n");

}



int main(int argc, char* argv[], char* env[])
{
    if ( argc != 2 ) {
        fprintf(stderr,"[ERROR] : command line is not valid. argument.count != 2 \n");
        return -1;
    }

    char* pFileContent = NULL;
    long int filelen = 0;
    int ret = getFileContent(argv[1], &pFileContent, &filelen);
    if ( ret != 1 ) {
        fprintf(stderr,"[ERROR] : There's an error raised during the dis-organize process.\n");
        return 0;
    }

    fprintf(stderr,"[SUCC] : Read File Done.\n");
    if ( filelen == 0 ) {
        return 0;
    }

    // generate link table
    charNode* head = generateDisOrganizedLinkTable(pFileContent, filelen);
    // check is equal
    charNode* currentNode = head->next;
    int chCnt = 0;
    int errorCnt = 0;
    while ( currentNode != NULL )
    {
        unsigned long long locateIdx = currentNode->index;
        if (  locateIdx>=0 && locateIdx < filelen ) {
            if ( pFileContent[locateIdx] != currentNode->ch ) {
                fprintf(stderr,"[ERROR] : ch1 != ch2,  locateIdx = %I64d. \n", locateIdx );
                ++errorCnt;
            }
        } else {
            fprintf(stderr,"[ERROR] : locateIdx: %I64d is out of range.\n", locateIdx);
            ++errorCnt;
        }

        currentNode = currentNode->next;
        ++chCnt;
    }

    fprintf(stderr,"character.count = %d.\n", chCnt);
    if ( chCnt != ((int)filelen )) {
        fprintf(stderr,"[ERROR] : count check is not equal , filelen(%ld) != ch.count(%d)\n", filelen, chCnt );
    }

    if ( errorCnt == 0 ) {
        fprintf(stderr,"[INFO] : Check Equal ?  OK \n");
    } else {
        fprintf(stderr,"[ERROR] : Check Equal ?  %d error(s) catched \n", errorCnt);
    }

    //
    // generate source code   :   main.c
    //
    createSourceCode(head,filelen);


    currentNode = head;
    while ( currentNode != NULL ) {
        charNode* next = currentNode->next;
        free(currentNode);
        currentNode = next;
    }

    return 0;
}

