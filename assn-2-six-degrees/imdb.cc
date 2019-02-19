using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

//  Here is my homework begin

  /**
   * function: cmp
   * ------------------
   *    This is cmp function,using for bsearch`s helper function
   *    to cmp whether actor1 == actor2 ? 
   *		
   * @param    const void* a is  const char**        
				  *(char**)a point to actorFile  ,actorFile point to actor Raw Date
				  *((char**)a+8) point to playP,  playP point to the actor you look for   											
   * @param    const void* b is const int*  
   * 			         *((int*)b) point to offset in actorFile		
   * @return  strcmp(actor1 ==actor2)  
   *            actor1 < actor2     return negtive number
   *			actor1 == actor2    return 0
   *            actor1 > actor2     return positive number
   */  
int cmp(const void* a,const void* b){
	char** base = (char**)malloc(8);    
	memcpy(base,(char**)a,8); 
	char** playP = (char**)malloc(8);
	memcpy(playP,(char**)a+8,8);	
	const int s2 = *(int*)b; 
	int result = strcmp(*playP,*base+s2); 
	free(base);
	free(playP);
	return result;
}

// The introduce in imdb.h 
/*
*	actor struct:
*			const char* name;  
*			(if len(name) is even:   '\0''\0'   
*            else					'\0')    for bus error 							
*			short number;			
*			(if address here isn`t a multiple of four, pad '\0' '\0' )
*			int offset  .......
*/
void imdb::GetActorfilms_offset(int offset,vector<int>& fileoffset)const {
	int walk = offset;
	const char *name = ((char*)this->actorFile+walk);
	int len = strlen(name);
	walk += (len%2) ? len+1 : len+2;   //'/0'   '/0''/0' 	
	const int number = *(short*)((char*)this->actorFile+walk);
	fileoffset.resize(number);
	walk +=2;   //short
	walk += (walk%4)?2:0;       //  pad '\0''\0'
	for(int j=0;j<number;j++){
		fileoffset[j] = *(int*)((char*)this->actorFile+walk);
		walk+=4;
	}	
}

// The introduce in imdb.h
/*
*			movie struct:
*					const char *title;
*					char year;       actually year must add 1900
*					( if address here is odd, pad '\0')
*					short number;
*					(if address here isn`t a multiple of four, pad '\0' '\0' )
*					int offset  .......		
*/
void imdb::Getmovieinfo(int offset,film& movie)const {
	int walk = offset;	
	movie.title = string((char*)this->movieFile+walk);
	walk +=movie.title.size() +1;
	movie.year = *(char*)((char*)movieFile+walk)+1900;
}

//   Key function,   using bsearch,search films that player had appeared
bool imdb::getCredits(const string& player, vector<film>& films) const { 
	const char* playerP = player.c_str();
	char** base_player = (char**)malloc(200);
	memcpy(base_player,&actorFile,8);
	memcpy(base_player+8,&playerP,8);
	//			bsearch return  int*  is film`s offset,otherwise return NULL
 	const void* result = bsearch(base_player,(int*)actorFile+1,*(int*)actorFile,sizeof(int),cmp);  
	free(base_player);  
	if (result == NULL)
		return false;	
	vector<int> fileoffset;	
	this->GetActorfilms_offset(*(int*)result,fileoffset);
	for (auto i : fileoffset){
		film F ;
		this->Getmovieinfo(i,F);
		films.push_back(F);
	}
	return true;
}

  /**
   * function: cmp1
   * ------------------
   *    This is cmp1 function,using for bsearch`s helper function
   *    to cmp whether movie1 == movie2 ? 
   *		
   * @param    const void* a is  const char**        
   *				  (char**)a point to movieFile  ,movieFile point to movie Raw Date
   *				  ((char**)a+8) point to movieP,  movieP point to the movie you look for   											
   *                  ((char**)a+32) point to year;
   * @param    const void* b is const int*  
   * 			         *((int*)b) point to offset in movieFile		
   * @return    using film::operator==
   *				  film::operator<
   */   
int cmp1(const void* a,const void* b){
	char **base = (char**)malloc(32);
	memcpy(base,(char**)a,8);
	char **movieP = (char**)malloc(32);
	memcpy(movieP,(char**)a+8,8);
	int* year = (int*)malloc(32);		
	memcpy(year,(char**)a+16,sizeof(int));
	const int s2 = *(int*)b; 
	int result = strcmp(*movieP,*base+s2);
	int result2 = !(*year==*((char*)(*base+s2+strlen(*base+s2)+1)));
	free(base);
	free(movieP);
	free(year);
	if (result ==0)		return result2;
	else	return result;
}

// The introduce in imdb.h 
/*
*			movie struct:
*					const char *title;
*					char year;       actually year must add 1900
*					( if address here is odd, pad '\0')
*					short number;
*					(if address here isn`t a multiple of four, pad '\0' '\0' )
*					int offset  .......		
*/
void imdb::GetFilmsactor_offset(int offset,vector<int>& Actoroffset)const {
	int walk = offset;
	const char *name = ((char*)this->movieFile+walk);
	int len = strlen(name);
	walk +=len+2;
	while( *((char*)movieFile+walk) =='\0')
		walk++;
	int number = *(short*)((char*)movieFile+walk);
	walk+=2;
	walk += (walk%4)?2:0;       //  pad '\0''\0' 
	Actoroffset.resize(number);
	for (int j=0;j<number;j++){
		Actoroffset[j] = (*(int*)((char*)movieFile+walk));
		walk+=4;
	}
}

// The introduce in imdb.h
/*
*	actor struct:
*			const char* name;  
*			(if len(name) is even:   '\0''\0'   
*            else					'\0')    for bus error 							
*			short number;			
*			(if address here isn`t a multiple of four, pad '\0' '\0' )
*			int offset  .......
*/
void imdb::Getactorinfo(int offset,string& player)const {
	int walk = offset;
	player = string((char*)this->actorFile+walk);
}


//   Key function,   using bsearch,search player that  had appeared in this movie
bool imdb::getCast(const film& movie, vector<string>& players) const { 
	const char* movieP = movie.title.c_str();
	int year = movie.year-1900; 
	char** base_movie = (char**)malloc(200);
	memcpy(base_movie,&movieFile,8);
	memcpy(base_movie+8,&movieP,8);
	memcpy(base_movie+16,&year,4);
	//			bsearch return  int*  is actor`s offset,otherwise return NULL
 	const void* result = bsearch(base_movie,(int*)movieFile+1,*(int*)movieFile,sizeof(int),cmp1);    	
	free(base_movie);	
	if (result == NULL)
		return false;	
	vector<int> actoroffset;
	this->GetFilmsactor_offset(*(int*)result,actoroffset);
	for (auto i :actoroffset){
		string player;
		this->Getactorinfo(i,player);
		players.push_back(player);
	}
	return true;	
}

// Here is end


imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
