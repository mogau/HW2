#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

pthread_mutex_t mlog;

typedef struct dict {
  char *word;
  int count;
  struct dict *next;
} dict_t;
dict_t *dic;
char *
make_word( char *word ) {
  return strcpy( malloc( strlen( word )+1 ), word );
}

dict_t *
make_dict(char *word) {
  dict_t *nd = (dict_t *) malloc( sizeof(dict_t) );
  nd->word = make_word( word );
  nd->count = 1;
  nd->next = NULL;
  return nd;
}

dict_t *
insert_word( dict_t *dic, char *word ) {
  
  //   Insert word into dict or increment count if already there
  //   return pointer to the updated dict
  
  dict_t *nd;
  dict_t *pd = NULL;		// prior to insertion point 
  dict_t *di = dic;		// following insertion point
  // Search down list to find if present or point of insertion
  while(di && ( strcmp(word, di->word ) >= 0) ) { 
    if( strcmp( word, di->word ) == 0 ) { 
      di->count++;		// increment count 
      return dic;			// return head 
    }
    pd = di;			// advance ptr pair
    di = di->next;
  }
  nd = make_dict(word);		// not found, make entry 
  nd->next = di;		// entry bigger than word or tail 
  if (pd) {
    pd->next = nd;
    return dic;			// insert beond head 
  }
  return nd;
}

void print_dict(dict_t *dic) {
  while (dic) {
    printf("[%d] %s\n", dic->count, dic->word);
    dic = dic->next;
    
  }
}

int
get_word( char *buf, int n, FILE *infile) {
  int inword = 0;
  int c;  
  while( (c = fgetc(infile)) != EOF ) {
    if (inword && !isalpha(c)) {
      buf[inword] = '\0';	// terminate the word string
      return 1;
    } 
    if (isalpha(c)) {
      buf[inword++] = c;
    }
  }
  return 0;			// no more words
}

#define MAXWORD 1024
void * words(void *args) {
  FILE *infile =(FILE *)args;
  char wordbuf[MAXWORD];
//pthread_mutex_init();
  while(get_word( wordbuf, MAXWORD, infile) ) {
   
     pthread_mutex_lock(&mlog);      
 
    dic= insert_word(dic, wordbuf); // add to dict
    
pthread_mutex_unlock(&mlog);
 }
     
}

int
main( int argc, char *argv[] ) {

 if(pthread_mutex_init(&mlog,NULL)!=0){
    printf("Could not lock");
    return 1; 
 }  

  dic = NULL;
  FILE *infile = stdin;
  if (argc >= 2) {
    infile = fopen (argv[1],"r");
  }
  if( !infile ) {
    printf("Unable to open %s\n",argv[1]);
    exit( EXIT_FAILURE );
  }

  pthread_t thread[4];

  for (int i=0;i<4;i++){
  pthread_create(&thread[i],NULL,&words,infile);
   pthread_join(thread[i],NULL);
 
}

  print_dict(dic);

  pthread_mutex_destroy(&mlog);
  fclose( infile );
}

