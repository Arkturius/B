


%macro PARAM	1.nolist
	%1, 
%endmacro

%macro	CLASS 1-*.nolist

#define	%1_constructor(_x)
(
	%rep %0 - 1
		%rotate 1
		PARAM %1
	%endrep
)
#define	%1_destructor(_y)	OHOHO

%2
%3

%endmacro

CLASS truc, int a, int b

int
main(void)
{

}
