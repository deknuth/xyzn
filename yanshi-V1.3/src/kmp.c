#include "../inc/core.h"

static int get_nextval(const char *T,int next[]);
int kmp(const char *Text,const char* Pattern)
{
	if(!Text || !Pattern || Pattern[0] == '\0' || Text[0]=='\0') 
		return -1;										
	int llen=0;   
	const char* c = Pattern;   
	while(*c++ != '\0')									
		++llen;											
	int* next = (int*)malloc(sizeof(int) * (llen + 1));
	// int*  next = new int[len+1];   
	get_nextval(Pattern,next);							
	int index ,i ,j;   
	i = j = index = 0;
	while(Text[i]!='\0' && Pattern[j]!='\0')   
	{   
		if(Text[i] == Pattern[j])   
		{   
			++i;										
			++j;   
		}   
		else   
		{   
			index += j-next[j];   
			if(next[j] != -1)   
				j = next[j];							
			else   
			{   
				j = 0;   
				++i;   
			}   
		}   
	}    
	free (next);
	if(Pattern[j] == '\0')   
		return index;									
	else   
		return -1;   
}

int get_nextval(const char *T,int next[])
{
	int j = 0, k = -1;   
	next[0] = -1;
	while (T[j] != '\0')
	{   
		if (k == -1 || T[j] == T[k])
		{   
			++j;
			++k;
			if(T[j] != T[k])     
				next[j] = k;   
			else     
				next[j] = next[k];   
		}  
		else     
			k = next[k];   
	}  
	return 0;
}
